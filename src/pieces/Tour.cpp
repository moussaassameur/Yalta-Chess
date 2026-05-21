#include "pieces/Tour.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

/**
 * @file Tour.cpp
 * @brief Implementation de la Tour : glissade orthogonale avec bending.
 */

Tour::Tour(Couleur couleur) : Piece(couleur) {}

std::string Tour::getType() const { return "Tour"; }

std::vector<std::shared_ptr<Case>>
Tour::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (!position) return coups;

    static const int dirs[4][2] = {
        {+1,  0}, {-1,  0},
        { 0, +1}, { 0, -1}
    };

    for (const auto& d : dirs) {
        int dx = d[0];
        int dy = d[1];
        std::shared_ptr<Case> courante = position;

        while (true) {
            auto suivante = plateau.voisinAvecDir(courante, dx, dy);
            if (!suivante) break;

            if (!suivante->estOccupee()) {
                coups.push_back(suivante);
                courante = suivante;
                continue;
            }

            // Case occupee : capture si ennemi, sinon on s'arrete.
            if (suivante->getPiece()->getCouleur() != couleur) {
                coups.push_back(suivante);
            }
            break;
        }
    }

    return coups;
}
