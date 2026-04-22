#include "joueur/Joueur.hpp"

Joueur::Joueur(const std::string& nom, Couleur couleur)
    : nom(nom), couleur(couleur), estElimine(false) {
}

void Joueur::eliminer() {
    estElimine = true;
}

std::string Joueur::getNom()        const { return nom; }
Couleur     Joueur::getCouleur()    const { return couleur; }
bool        Joueur::getEstElimine() const { return estElimine; }
