#include "ia/MinMax.hpp"
#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "coup/Coup.hpp"
#include "coup/CoupSimple.hpp"
#include "pieces/Piece.hpp"

#include <algorithm>
#include <limits>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

/**
 * @file MinMax.cpp
 * @brief Implementation de l'algorithme MinMax pour Yalta (paranoiaque).
 *
 * Strategie suivie  :
 *   1. On parcourt l'arbre des coups possibles en post-ordre (DFS).
 *   2. Aux feuilles (profondeur atteinte ou plus de coups) on appelle
 *      la fonction d'evaluation.
 *   3. On remonte les valeurs : noeud MAX = max des fils, noeud MIN = min.
 *   4. Dans la recursion, on ne copie jamais le plateau : on modifie via
 *      Coup::executer puis on restaure via Coup::annuler.
 *   5. En multi-thread, la racine est parallelisee avec un pattern THREAD
 *      POOL : nbThreads threads se partagent les coups candidats via une
 *      file commune protegee par des mutex. Chaque thread clone le plateau
 *      pour avoir sa propre copie de travail.
 */

namespace {

/// Valeur materielle de chaque type de piece (echecs standard).
int valeurPiece(const std::string& type) {
    if (type == "Pion")     return 1;
    if (type == "Cavalier") return 3;
    if (type == "Fou")      return 3;
    if (type == "Tour")     return 5;
    if (type == "Reine")    return 9;
    if (type == "Roi")      return 1000;
    return 0;
}

/// Genere tous les coups legaux disponibles pour `couleur` sur `plateau`.
std::vector<std::shared_ptr<Coup>>
genererCoupsLegaux(Plateau& plateau, Couleur couleur) {
    std::vector<std::shared_ptr<Coup>> coups;
    for (const auto& piece : plateau.getPiecesDeCouleur(couleur)) {
        auto depart = piece->getPosition();
        if (!depart) continue;
        for (const auto& arrivee : piece->getCoupsLegaux(plateau)) {
            // On utilise CoupSimple pour tout, y compris les captures :
            // l'IA n'a pas besoin de roque ou promotion explicites pour
            // estimer la valeur d'une position (resterait du materiel).
            coups.push_back(std::make_shared<CoupSimple>(depart, arrivee));
        }
    }
    return coups;
}

}  // namespace

MinMax::MinMax(int profondeur, int nbThreads, Couleur couleurIA)
    : profondeur(profondeur),
      nbThreads(nbThreads),
      couleurIA(couleurIA) {
}

int     MinMax::getProfondeur() const { return profondeur; }
int     MinMax::getNbThreads()  const { return nbThreads; }
Couleur MinMax::getCouleurIA()  const { return couleurIA; }

Couleur MinMax::joueurSuivant(Couleur c) {
    switch (c) {
        case Couleur::BLANC: return Couleur::ROUGE;
        case Couleur::ROUGE: return Couleur::NOIR;
        case Couleur::NOIR:  return Couleur::BLANC;
    }
    return Couleur::BLANC;
}

std::shared_ptr<Coup> MinMax::getMeilleurCoup(Plateau& plateau) {
    auto coups = genererCoupsLegaux(plateau, couleurIA);
    if (coups.empty()) return nullptr;

    // Generateur aleatoire pour le tie-breaking entre coups equivalents.
    // Sans ca, l'IA prend toujours le premier coup de la liste, ce qui
    // produit du "shuffle" (meme piece deplacee en boucle).
    static thread_local std::mt19937 gen(std::random_device{}());

    // Mode mono-thread
    if (nbThreads <= 1) {
        std::vector<std::shared_ptr<Coup>> meilleurs;
        int meilleureValeur = std::numeric_limits<int>::min();
        for (auto& coup : coups) {
            coup->executer(plateau);
            const int v = minMax(plateau, profondeur - 1,
                                 /*maximisant=*/false,
                                 joueurSuivant(couleurIA));
            coup->annuler(plateau);
            if (v > meilleureValeur) {
                meilleureValeur = v;
                meilleurs      = {coup};
            } else if (v == meilleureValeur) {
                meilleurs.push_back(coup);
            }
        }
        if (meilleurs.empty()) return nullptr;
        return meilleurs[std::uniform_int_distribution<size_t>(0, meilleurs.size() - 1)(gen)];
    }

    //  Mode multi-thread : pattern THREAD POOL (bassin de taches) 
    //
    // On parallelise la racine : chaque coup candidat est une "tache" a
    // evaluer. nbThreads threads "workers" identiques se partagent ces
    // taches via une file commune.
    //
    // Deux donnees sont PARTAGEES entre les threads -> protegees par mutex :
    //   - prochaineTache            : index du prochain coup a distribuer ;
    //   - meilleureValeur/meilleurs : le meilleur resultat trouve.
    // Le calcul lui-meme (clone + minMax) tourne HORS verrou : c'est la
    // partie qui doit s'executer en parallele. Chaque thread clone le
    // plateau pour travailler sur sa propre copie (aucun plateau partage).

    // Pre-extraction des coordonnees : les Case du plateau d'origine ne
    // sont pas valides sur un clone, on transmet donc des (x, y).
    struct Tache { int xDep, yDep, xArr, yArr; };
    std::vector<Tache> taches;
    taches.reserve(coups.size());
    for (const auto& c : coups) {
        auto cs = std::dynamic_pointer_cast<CoupSimple>(c);
        taches.push_back({cs->getDepart()->getX(),  cs->getDepart()->getY(),
                          cs->getArrivee()->getX(), cs->getArrivee()->getY()});
    }

    std::mutex mutexFile;      // protege la distribution des taches
    std::mutex mutexResultat;  // protege le meilleur resultat partage
    size_t     prochaineTache  = 0;
    int        meilleureValeur = std::numeric_limits<int>::min();
    std::vector<size_t> meilleurs;

    // Fonction executee par chaque thread du pool.
    auto worker = [&]() {
        while (true) {
            // ── Section critique 1 : prendre une tache dans la file ──
            size_t i;
            {
                std::lock_guard<std::mutex> verrou(mutexFile);
                if (prochaineTache >= taches.size()) return;  // file vide
                i = prochaineTache;
                ++prochaineTache;
            }

            // ── Calcul en parallele, hors verrou, sur un clone prive ──
            const Tache& t = taches[i];
            auto clone = plateau.clone();
            CoupSimple c(clone->getCase(t.xDep, t.yDep),
                         clone->getCase(t.xArr, t.yArr));
            c.executer(*clone);
            const int v = minMax(*clone, profondeur - 1,
                                 /*maximisant=*/false,
                                 joueurSuivant(couleurIA));

            // Section critique 2 : publier dans le resultat partage 
            {
                std::lock_guard<std::mutex> verrou(mutexResultat);
                if (v > meilleureValeur) {
                    meilleureValeur = v;
                    meilleurs       = {i};
                } else if (v == meilleureValeur) {
                    meilleurs.push_back(i);
                }
            }
        }
    };

    // Lancement du pool : nbThreads threads identiques.
    std::vector<std::thread> pool;
    for (int t = 0; t < nbThreads; ++t) {
        pool.emplace_back(worker);
    }
    // Synchronisation : on attend la fin de tous les threads (join).
    for (auto& th : pool) {
        th.join();
    }

    // Tie-breaking aleatoire pour eviter le "shuffle" (toujours le 1er coup).
    if (meilleurs.empty()) return nullptr;
    const size_t choix =
        std::uniform_int_distribution<size_t>(0, meilleurs.size() - 1)(gen);
    return coups[meilleurs[choix]];
}

int MinMax::minMax(Plateau& plateau, int profondeur,
                   bool maximisant, Couleur joueurCourant) {
    // Cas terminal : on a atteint la profondeur maximale.
    if (profondeur <= 0) {
        return evaluer(plateau, couleurIA);
    }

    auto coups = genererCoupsLegaux(plateau, joueurCourant);

    // Cas terminal : joueur sans coup possible.
    if (coups.empty()) {
        // Si le joueur est en echec sans coup  il est mat.
        // Selon que c'est l'IA ou un adversaire, c'est une perte ou un gain.
        if (plateau.estEnEchec(joueurCourant)) {
            if (joueurCourant == couleurIA) return -100000;  // -infini
            else                            return +100000;  // +infini
        }
        // Sinon : pat (egalite pour ce joueur, neutre).
        return evaluer(plateau, couleurIA);
    }

    if (maximisant) {
        // Noeud MAX : tour de l'IA, on prend le meilleur fils.
        int meilleur = std::numeric_limits<int>::min();
        for (auto& coup : coups) {
            coup->executer(plateau);
            const Couleur prochain = joueurSuivant(joueurCourant);
            const bool prochainMax = (prochain == couleurIA);
            const int v = minMax(plateau, profondeur - 1, prochainMax, prochain);
            coup->annuler(plateau);
            if (v > meilleur) meilleur = v;
        }
        return meilleur;
    } else {
        // Noeud MIN : tour d'un adversaire. En parano, on suppose que les
        // deux adversaires jouent contre l'IA -> on minimise.
        int pire = std::numeric_limits<int>::max();
        for (auto& coup : coups) {
            coup->executer(plateau);
            const Couleur prochain = joueurSuivant(joueurCourant);
            const bool prochainMax = (prochain == couleurIA);
            const int v = minMax(plateau, profondeur - 1, prochainMax, prochain);
            coup->annuler(plateau);
            if (v < pire) pire = v;
        }
        return pire;
    }
}

int MinMax::evaluer(const Plateau& plateau, Couleur couleur) const {
    // Base : F(E) = somme(valeur pieces de `couleur`) - somme(adverses).
    // C'est la forme proposee par le prof pour Tic-Tac-Toe, adaptee aux
    // echecs. On ajoute ensuite plusieurs bonus/malus pour donner a l'IA
    // un objectif clair (pas juste eviter de perdre du materiel).
    int score = 0;
    for (const auto& cas : plateau.getToutesLesCases()) {
        auto p = cas->getPiece();
        if (!p || !p->estVivante()) continue;
        const int v = valeurPiece(p->getType());
        if (p->getCouleur() == couleur) score += v;
        else                            score -= v;
    }

    // Bonus si on met un adversaire en echec : c'est un signe que l'on
    // attaque -- guide l'IA vers l'aggressivite plutot que le shuffle.
    for (Couleur c : {Couleur::BLANC, Couleur::ROUGE, Couleur::NOIR}) {
        if (c == couleur) continue;
        if (plateau.estEnEchec(c)) score += 30;
    }
    // Malus si on est nous-meme en echec.
    if (plateau.estEnEchec(couleur)) score -= 30;

    return score;
}
