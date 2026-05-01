#include "pieces/Tour.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

/**
 * @file Tour.cpp
 * @brief Implementation des deplacements de la Tour.
 *
 * Algorithme : pour chacune des 4 directions cardinales, on glisse case
 * par case en utilisant Plateau::voisinAvecDir, qui met a jour (dx, dy)
 * lors d'un eventuel bending. On s'arrete des qu'on rencontre :
 *   - un bord exterieur (voisin = nullptr),
 *   - une piece amie (case non ajoutee, on stoppe la diagonale),
 *   - une piece ennemie (case ajoutee comme capture, on stoppe).
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
