#include "joueur/JoueurIA.hpp"
#include "ia/MinMax.hpp"

JoueurIA::JoueurIA(const std::string& nom, Couleur couleur, int profondeur)
    : Joueur(nom, couleur),
      ia(std::make_shared<MinMax>(profondeur)) {
}

std::shared_ptr<Coup> JoueurIA::jouerTour(ModeleJeu& modele) {
    return ia->getMeilleurCoup(modele);
}
