#include "joueur/JoueurHumain.hpp"

JoueurHumain::JoueurHumain(const std::string& nom, Couleur couleur)
    : Joueur(nom, couleur) {
}

std::shared_ptr<Coup> JoueurHumain::jouerTour(ModeleJeu& /*modele*/) {
    // Le coup est transmis par ControleurJeu via un clic Qt
    // Cette méthode ne fait rien ici — c'est le contrôleur qui appelle modele.jouerCoup()
    return nullptr;
}
