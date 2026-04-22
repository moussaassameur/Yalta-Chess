#include "ia/MinMax.hpp"
#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "coup/Coup.hpp"
#include <limits>
#include <thread>
#include <future>
#include <vector>

MinMax::MinMax(int profondeur, int nbThreads)
    : profondeur(profondeur), nbThreads(nbThreads) {
}

std::shared_ptr<Coup> MinMax::getMeilleurCoup(ModeleJeu& modele) {
    // TODO : récupérer tous les coups possibles du joueur actuel,
    // lancer minMax sur chacun (en parallèle avec nbThreads),
    // retourner celui qui a le meilleur score
    return nullptr;
}

int MinMax::minMax(ModeleJeu& modele, int profondeur, bool maximisant) {
    // Cas de base : profondeur 0 ou partie terminée
    if (profondeur == 0) {
        return evaluer(*modele.getPlateau());
    }

    // TODO : générer tous les coups, executer/annuler pour explorer l'arbre
    // Le plateau n'est PAS cloné — on utilise executer() pour descendre
    // et annuler() pour remonter (performance critique pour Min-Max)

    if (maximisant) {
        int meilleur = std::numeric_limits<int>::min();
        // TODO : pour chaque coup : executer, récursion, annuler, garder le max
        return meilleur;
    } else {
        int meilleur = std::numeric_limits<int>::max();
        // TODO : pour chaque coup : executer, récursion, annuler, garder le min
        return meilleur;
    }
}

int MinMax::evaluer(const Plateau& plateau) const {
    // Valeurs standard des pièces aux échecs
    // Reine=9, Tour=5, Fou=3, Cavalier=3, Pion=1, Roi=1000
    // TODO : parcourir toutes les pièces vivantes et sommer les valeurs
    // (positif pour le joueur courant, négatif pour les adversaires)
    (void)plateau;
    return 0;
}
