#include "pieces/Reine.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Reine::Reine(Couleur couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position) {
}

std::vector<std::shared_ptr<Case>> Reine::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (position == nullptr) return coups;

    // Reine = Tour + Fou : 6 directions droites + 6 diagonales = 12 directions
    const int directions[12][2] = {
        // Directions "Tour"
        {+1,  0}, {-1,  0}, { 0, +1}, { 0, -1}, {+1, -1}, {-1, +1},
        // Directions "Fou"
        {+1, +1}, {-1, -1}, {+2, -1}, {-2, +1}, {+1, -2}, {-1, +2}
    };

    int q = position->getQ();
    int r = position->getR();

    // Glisse dans chaque direction comme une Tour
    for (const auto& dir : directions) {
        int nq = q;
        int nr = r;
        while (true) {
            nq += dir[0];
            nr += dir[1];
            std::shared_ptr<Case> cible = plateau.getCase(nq, nr);
            if (cible == nullptr) break;

            if (!cible->estOccupee()) {
                coups.push_back(cible);
            } else {
                if (cible->getPiece()->getCouleur() != couleur) {
                    coups.push_back(cible);
                }
                break;
            }
        }
    }

    return coups;
}

std::string Reine::getType() const { return "Reine"; }