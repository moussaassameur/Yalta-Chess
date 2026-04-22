#include "pieces/Reine.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Reine::Reine(Couleur couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position) {
}

// Reine = Tour + Fou : glissade dans les 8 directions sur la grille 12x12.
std::vector<std::shared_ptr<Case>> Reine::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (position == nullptr) return coups;

    const int dirs[8][2] = {
        {+1,  0}, {-1,  0}, { 0, +1}, { 0, -1},
        {+1, +1}, {+1, -1}, {-1, +1}, {-1, -1}
    };

    int x = position->getX();
    int y = position->getY();

    for (const auto& d : dirs) {
        int nx = x + d[0];
        int ny = y + d[1];
        while (nx >= 0 && nx < 12 && ny >= 0 && ny < 12) {
            auto cible = plateau.getCase(nx, ny);
            if (!cible) break;

            if (!cible->estOccupee()) {
                coups.push_back(cible);
            } else {
                if (cible->getPiece()->getCouleur() != couleur) {
                    coups.push_back(cible);
                }
                break;
            }
            nx += d[0];
            ny += d[1];
        }
    }

    return coups;
}

std::string Reine::getType() const { return "Reine"; }
