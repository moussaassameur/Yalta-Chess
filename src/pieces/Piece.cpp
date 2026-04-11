#include "../../include/pieces/Piece.hpp"

namespace Yalta {

// Constructeur
Piece::Piece(Couleur couleur, std::string nom) {
    this->couleur = couleur;
    this->nom = nom;
    this->caseActuelle = nullptr;
    this->estVivante = true;
}

// Destructeur
Piece::~Piece() {
    caseActuelle = nullptr;
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

// Setters
void Piece::setCaseActuelle(Case* c) {
    this->caseActuelle = c;
}

void Piece::setEstVivante(bool vivante) {
    this->estVivante = vivante;
}

} // namespace Yalta