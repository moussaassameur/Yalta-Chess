#include "joueur/JoueurHumain.hpp"

JoueurHumain::JoueurHumain(const std::string& nom, Couleur couleur)
    : Joueur(nom, couleur) {
}

std::shared_ptr<Coup> JoueurHumain::jouerTour(Plateau& /*plateau*/) {
    return nullptr;
}
