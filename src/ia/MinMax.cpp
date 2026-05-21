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
 * @brief MinMax paranoiaque pour 3 joueurs avec multi-threading.
 *
 * L'idee : l'IA maximise son score, les 2 adversaires minimisent ensemble.
 * On evite de cloner le plateau grace au pattern Command (executer/annuler).
 * En multi-thread, on parallelise juste la racine avec un Thread Pool.
 */

namespace {

// Valeur de chaque piece (standard echecs)
int valeurPiece(const std::string& type) {
    if (type == "Pion")     return 1;
    if (type == "Cavalier") return 3;
    if (type == "Fou")      return 3;
    if (type == "Tour")     return 5;
    if (type == "Reine")    return 9;
    if (type == "Roi")      return 1000;  // le perdre = game over
    return 0;
}

// cette fonction elle donne ou genere tout les coups possibles pour la couleur sur ce plateau
std::vector<std::shared_ptr<Coup>> 
genererCoupsLegaux(Plateau& plateau, Couleur couleur) {
    std::vector<std::shared_ptr<Coup>> coups;
    for (const auto& piece : plateau.getPiecesDeCouleur(couleur)) {   //Boucle sur toutes les pièces de la couleur
        auto depart = piece->getPosition(); // on prend la position de la pièce pour générer les coups possibles
        if (!depart) continue;  // piece morte ou capturee
        
        for (const auto& arrivee : piece->getCoupsLegaux(plateau)) {
            // On cree un CoupSimple pour chaque destination possible
            // L'IA n'a pas besoin des coups speciaux (roque, promotion) pour evaluer
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

// Rotation des joueurs : BLANC -> ROUGE -> NOIR -> BLANC
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

    // Generateur aleatoire pour choisir entre coups equivalents
    // Sans ca, l'IA joue toujours le meme coup et fait du shuffle
    static thread_local std::mt19937 gen(std::random_device{}());

    // MODE MONO-THREAD
    if (nbThreads <= 1) {
        std::vector<std::shared_ptr<Coup>> meilleurs;
        int meilleureValeur = std::numeric_limits<int>::min();
        
        // On teste chaque coup
        for (auto& coup : coups) {
            coup->executer(plateau);  // on joue le coup
            const int v = minMax(plateau, profondeur - 1, false, joueurSuivant(couleurIA));
            coup->annuler(plateau);   // on restore le plateau
            
            // On garde les meilleurs
            if (v > meilleureValeur) {
                meilleureValeur = v;
                meilleurs = {coup};
            } else if (v == meilleureValeur) {
                meilleurs.push_back(coup);
            }
        }
        
        if (meilleurs.empty()) return nullptr;
        // Choix aleatoire entre les meilleurs
        return meilleurs[std::uniform_int_distribution<size_t>(0, meilleurs.size() - 1)(gen)];
    }

    //  MULTI-THREAD : Thread Pool
    // Chaque thread evalue des coups en parallele
    // 2 mutex : un pour distribuer les taches, un pour le resultat

    // On extrait les coordonnees avant (les pointeurs Case* ne marchent pas sur un clone)
    struct Tache { int xDep, yDep, xArr, yArr; };
    std::vector<Tache> taches;
    taches.reserve(coups.size());
    for (const auto& c : coups) {
        auto cs = std::dynamic_pointer_cast<CoupSimple>(c);
        taches.push_back({cs->getDepart()->getX(), cs->getDepart()->getY(),
                          cs->getArrivee()->getX(), cs->getArrivee()->getY()});
    }

    // Variables partagees entre threads
    std::mutex mutexFile;       // protege la file de taches
    std::mutex mutexResultat;   // protege le meilleur resultat
    size_t prochaineTache = 0;
    int meilleureValeur = std::numeric_limits<int>::min();
    std::vector<size_t> meilleurs;

    // Fonction executee par chaque thread
    auto worker = [&]() {
        while (true) {
            // SECTION CRITIQUE 1 : prendre une tache
            size_t i;
            {
                std::lock_guard<std::mutex> verrou(mutexFile);
                if (prochaineTache >= taches.size()) return;  // plus de boulot
                i = prochaineTache;
                ++prochaineTache;
            }  // le mutex se libere ici

            // CALCUL PARALLELE (hors mutex = vraie parallelisation)
            const Tache& t = taches[i];
            auto clone = plateau.clone();  // chaque thread a son propre plateau
            CoupSimple c(clone->getCase(t.xDep, t.yDep),
                         clone->getCase(t.xArr, t.yArr));
            c.executer(*clone);
            const int v = minMax(*clone, profondeur - 1, false, joueurSuivant(couleurIA));

            // SECTION CRITIQUE 2 : publier le resultat
            {
                std::lock_guard<std::mutex> verrou(mutexResultat);
                if (v > meilleureValeur) {
                    meilleureValeur = v;
                    meilleurs = {i};
                } else if (v == meilleureValeur) {
                    meilleurs.push_back(i);
                }
            }  // le mutex se libere ici
        }
    };

    // Lancement des threads
    std::vector<std::thread> pool;
    for (int t = 0; t < nbThreads; ++t) {
        pool.emplace_back(worker);
    }
    
    // On attend que tous finissent
    for (auto& th : pool) {
        th.join();
    }

    // Choix aleatoire parmi les meilleurs
    if (meilleurs.empty()) return nullptr;
    const size_t choix = std::uniform_int_distribution<size_t>(0, meilleurs.size() - 1)(gen);
    return coups[meilleurs[choix]];
}

int MinMax::minMax(Plateau& plateau, int profondeur,
                   bool maximisant, Couleur joueurCourant) {
    
    // Cas terminal 1 : profondeur atteinte
    if (profondeur <= 0) {
        return evaluer(plateau, couleurIA);
    }

    auto coups = genererCoupsLegaux(plateau, joueurCourant);

    // Cas terminal 2 : aucun coup possible
    if (coups.empty()) {
        if (plateau.estEnEchec(joueurCourant)) {
            // Echec et mat
            if (joueurCourant == couleurIA) return -100000;  // l'IA perd
            else return +100000;  // un adversaire perd, l'IA gagne
        }
        // Pat (egalite)
        return evaluer(plateau, couleurIA);
    }

    if (maximisant) {
        // Tour de l'IA : on cherche le max
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
        // Tour d'un adversaire : on cherche le min (mode parano)
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
    // Calcul du score : materiel de l'IA - materiel des adversaires
    int score = 0;
    for (const auto& cas : plateau.getToutesLesCases()) {
        auto p = cas->getPiece();
        if (!p || !p->estVivante()) continue;
        
        const int v = valeurPiece(p->getType());
        if (p->getCouleur() == couleur) score += v;  // piece de l'IA
        else score -= v;  // piece adverse
    }

    // Bonus : mettre un adversaire en echec (encourage l'attaque)
    for (Couleur c : {Couleur::BLANC, Couleur::ROUGE, Couleur::NOIR}) {
        if (c == couleur) continue;
        if (plateau.estEnEchec(c)) score += 30;
    }
    
    // Malus : etre soi-meme en echec (danger)
    if (plateau.estEnEchec(couleur)) score -= 30;

    return score;
}