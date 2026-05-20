#include "ia/MinMax.hpp"
#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "coup/Coup.hpp"
#include "coup/CoupSimple.hpp"
#include "pieces/Piece.hpp"

#include <algorithm>
#include <future>
#include <limits>
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
 *   4. On ne copie jamais le plateau : on modifie via Coup::executer puis
 *      on restaure via Coup::annuler.
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

    // Mode multi-thread
    // On parallelise au niveau racine : chaque coup candidat est evalue
    // dans un thread separe, qui travaille sur son propre clone du plateau.
    // L'exploration recursive interne reste mono-thread (suffisant pour
    // beneficier d'un speed-up significatif a la racine).
    //
    // Pour respecter nbThreads, on lance les futures par batches de
    // nbThreads et on attend chaque batch avant le suivant.
    std::vector<int> valeurs(coups.size());
    const int batchSize = std::max(1, nbThreads);

    for (size_t debut = 0; debut < coups.size(); debut += batchSize) {
        const size_t fin = std::min(debut + batchSize, coups.size());
        std::vector<std::future<int>> futures;

        for (size_t i = debut; i < fin; ++i) {
            auto coupSimple = std::dynamic_pointer_cast<CoupSimple>(coups[i]);
            // Coordonnees a transmettre au thread (les Case du plateau
            // original ne sont PAS valides sur le clone).
            const int xDep = coupSimple->getDepart()->getX();
            const int yDep = coupSimple->getDepart()->getY();
            const int xArr = coupSimple->getArrivee()->getX();
            const int yArr = coupSimple->getArrivee()->getY();

            futures.push_back(std::async(std::launch::async,
                [this, &plateau, xDep, yDep, xArr, yArr]() {
                    auto clone = plateau.clone();
                    CoupSimple c(clone->getCase(xDep, yDep),
                                 clone->getCase(xArr, yArr));
                    c.executer(*clone);
                    return minMax(*clone, profondeur - 1,
                                  /*maximisant=*/false,
                                  joueurSuivant(couleurIA));
                }));
        }

        for (size_t i = debut; i < fin; ++i) {
            valeurs[i] = futures[i - debut].get();
        }
    }

    // Selection du meilleur coup parmi les valeurs collectees,
    // avec tie-breaking aleatoire pour eviter le shuffle.
    std::vector<std::shared_ptr<Coup>> meilleurs;
    int meilleureValeur = std::numeric_limits<int>::min();
    for (size_t i = 0; i < coups.size(); ++i) {
        if (valeurs[i] > meilleureValeur) {
            meilleureValeur = valeurs[i];
            meilleurs      = {coups[i]};
        } else if (valeurs[i] == meilleureValeur) {
            meilleurs.push_back(coups[i]);
        }
    }
    if (meilleurs.empty()) return nullptr;
    return meilleurs[std::uniform_int_distribution<size_t>(0, meilleurs.size() - 1)(gen)];
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
        // Si le joueur est en echec sans coup -> il est mat.
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
