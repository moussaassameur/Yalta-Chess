#include "model/Case.hpp"
#include "pieces/Piece.hpp"
#include <cstdlib>  // pour std::abs

Case::Case(int q, int r, const std::string& couleur, int secteur)
    : q(q), r(r), piece(nullptr), couleur(couleur), secteur(secteur) {
}

int Case::getQ() const {
    return q;
}

int Case::getR() const {
    return r;
}

int Case::getS() const {
    return -q - r;  // règle fondamentale : q + r + s = 0
}

int Case::getSecteur() const {
    return secteur;
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

int Case::distance(const Case& a, const Case& b) {
    return (std::abs(a.q - b.q) + std::abs(a.r - b.r) + std::abs(a.getS() - b.getS())) / 2;
}

bool Case::operator==(const Case& autre) const {
    return q == autre.q && r == autre.r;
}