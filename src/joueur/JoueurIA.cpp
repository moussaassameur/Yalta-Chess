#include "joueur/JoueurIA.hpp"

JoueurIA::JoueurIA(const std::string& nom, Couleur couleur,
                   int profondeur, int nbThreads)
    : Joueur(nom, couleur),
      ia(profondeur, nbThreads, couleur) {
}

std::shared_ptr<Coup> JoueurIA::jouerTour(Plateau& plateau) {
    return ia.getMeilleurCoup(plateau);
}

const MinMax& JoueurIA::getIA() const {
    return ia;
}
