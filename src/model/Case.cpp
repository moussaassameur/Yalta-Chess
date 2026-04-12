#include "model/Case.hpp"
#include "pieces/Piece.hpp"  // ici on inclut le header complet (forward decl ne suffit plus)

Case::Case(int ligne, int colonne, const std::string& couleur)
    : ligne(ligne), colonne(colonne), piece(nullptr), couleur(couleur) {
}

int Case::getLigne() const {
    return ligne;
}

int Case::getColonne() const {
    return colonne;
}

std::string Case::getCouleur() const {
    return couleur;
}

std::shared_ptr<Piece> Case::getPiece() const {
    return piece;
}

void Case::setPiece(std::shared_ptr<Piece> nouvellePiece) {
    piece = nouvellePiece;
}

void Case::retirerPiece() {
    piece = nullptr;
}

bool Case::estOccupee() const {
    return piece != nullptr;
}