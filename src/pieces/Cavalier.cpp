#include "pieces/Cavalier.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Cavalier::Cavalier(Couleur couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position) {
}

// Cavalier : 8 sauts en L, saute les pieces, sur la grille 12x12.
std::vector<std::shared_ptr<Case>> Cavalier::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (position == nullptr) return coups;

    const int sauts[8][2] = {
        {+1, +2}, {-1, +2}, {+1, -2}, {-1, -2},
        {+2, +1}, {-2, +1}, {+2, -1}, {-2, -1}
    };

    int x = position->getX();
    int y = position->getY();

    for (const auto& s : sauts) {
        int nx = x + s[0];
        int ny = y + s[1];
        if (nx < 0 || nx >= 12 || ny < 0 || ny >= 12) continue;

        auto cible = plateau.getCase(nx, ny);
        if (!cible) continue;

        if (!cible->estOccupee() || cible->getPiece()->getCouleur() != couleur) {
            coups.push_back(cible);
        }
    }

    return coups;
}

std::string Cavalier::getType() const { return "Cavalier"; }
