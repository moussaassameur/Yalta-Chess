#include "pieces/Roi.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Roi::Roi(Couleur couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position), aDejaBouge(false) {
}

// Roi : 1 case dans les 8 directions, sur la grille 12x12.
std::vector<std::shared_ptr<Case>> Roi::getDeplacements(const Plateau& plateau) const {
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
        if (nx < 0 || nx >= 12 || ny < 0 || ny >= 12) continue;

        auto cible = plateau.getCase(nx, ny);
        if (!cible) continue;

        if (!cible->estOccupee() || cible->getPiece()->getCouleur() != couleur) {
            coups.push_back(cible);
        }
    }

    return coups;
}

std::string Roi::getType() const { return "Roi"; }

bool Roi::getADejaBouge() const { return aDejaBouge; }
void Roi::setADejaBouge(bool valeur) { aDejaBouge = valeur; }
