#include "model/Case.hpp"
#include "pieces/Piece.hpp"
#include <cstdlib>
#include <string>

Case::Case(int x, int y, int sextant, const std::string& couleur)
    : x(x), y(y), sextant(sextant),
      piece(nullptr), couleur(couleur) {
}

int Case::getX() const { return x; }
int Case::getY() const { return y; }
int Case::getSextant() const { return sextant; }

int Case::getQ() const { return x; }
int Case::getR() const { return y; }
int Case::getS() const { return 0; }
int Case::getSecteur() const { return sextant; }

std::string Case::getPosition() const {
    return std::to_string(x) + "," + std::to_string(y);
}

std::string Case::getCouleur() const { return couleur; }

std::shared_ptr<Piece> Case::getPiece() const { return piece; }

void Case::setPiece(std::shared_ptr<Piece> nouvellePiece) { piece = nouvellePiece; }
void Case::retirerPiece() { piece = nullptr; }

bool Case::estOccupee() const { return piece != nullptr; }

int Case::distance(const Case& a, const Case& b) {
    int dx = std::abs(a.x - b.x);
    int dy = std::abs(a.y - b.y);
    return std::max(dx, dy);  // distance de Chebyshev
}

bool Case::operator==(const Case& autre) const {
    return x == autre.x && y == autre.y;
}
