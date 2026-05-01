#include "joueur/Joueur.hpp"

Joueur::Joueur(const std::string& nom, Couleur couleur)
    : nom(nom), couleur(couleur), estElimine(false), score(0.0) {
}

std::string Joueur::getNom() const          { return nom; }
Couleur     Joueur::getCouleur() const      { return couleur; }
bool        Joueur::getEstElimine() const   { return estElimine; }
void        Joueur::eliminer()              { estElimine = true; }
double      Joueur::getScore() const        { return score; }
void        Joueur::ajouterScore(double s)  { score += s; }
