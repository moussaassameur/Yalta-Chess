#include "../../include/model/Case.hpp"

namespace Yalta {

// Constructeur
Case::Case(int ligne, int colonne, bool couleur) {
    this->ligne = ligne;
    this->colonne = colonne;
    this->couleur = couleur;
    this->piece = nullptr;
}

// Destructeur
Case::~Case() {
    piece = nullptr;
}

// Getters
int Case::getLigne() const {
    return ligne;
}

int Case::getColonne() const {
    return colonne;
}

bool Case::getCouleur() const {
    return couleur;
}

Piece* Case::getPiece() const {
    return piece;
}

// Setters
void Case::setPiece(Piece* piece) {
    this->piece = piece;
}

// Methodes
bool Case::estOccupee() const {
    return piece != nullptr;
}

void Case::vider() {
    piece = nullptr;
}

} // namespace Yalta