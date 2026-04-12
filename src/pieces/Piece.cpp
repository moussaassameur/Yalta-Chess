#include "../../include/pieces/Piece.hpp"
#include "../../include/strategy/StrategieDeplacement.hpp"

namespace Yalta {

// Constructeur
Piece::Piece(Couleur couleur, std::string nom) {
    this->couleur = couleur;
    this->nom = nom;
    this->caseActuelle = nullptr;
    this->estVivante = true;
    this->strategie = nullptr;
}

// Destructeur
Piece::~Piece() {
    delete strategie;
    strategie = nullptr;
}

// getDeplacements délègue à la stratégie
std::vector<Case*> Piece::getDeplacements(
    std::vector<std::vector<Case*>>& plateau) {
    return strategie->calculerDeplacements(this, plateau);
}

// Getters
Piece::Couleur Piece::getCouleur() const {
    return couleur;
}

Case* Piece::getCaseActuelle() const {
    return caseActuelle;
}

bool Piece::getEstVivante() const {
    return estVivante;
}

std::string Piece::getNom() const {
    return nom;
}

StrategieDeplacement* Piece::getStrategie() const {
    return strategie;
}

// Setters
void Piece::setCaseActuelle(Case* c) {
    this->caseActuelle = c;
}

void Piece::setEstVivante(bool vivante) {
    this->estVivante = vivante;
}

void Piece::setStrategie(StrategieDeplacement* s) {
    this->strategie = s;
}

} 