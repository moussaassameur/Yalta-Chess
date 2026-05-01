#include "pieces/Reine.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

/**
 * @file Reine.cpp
 * @brief Implementation des deplacements de la Reine (Tour + Fou).
 */

static const int OFFSET_X[6] = { 0, 0, 8, 8, 4, 4 };
static const int OFFSET_Y[6] = { 0, 4, 4, 8, 8, 0 };

Reine::Reine(Couleur couleur) : Piece(couleur) {}

std::string Reine::getType() const { return "Reine"; }

static bool estCentreSextant(const std::shared_ptr<Case>& c) {
    if (!c) return false;
    const int s = c->getSextant();
    return (c->getX() - OFFSET_X[s] == 3
         && c->getY() - OFFSET_Y[s] == 3);
}

std::vector<std::shared_ptr<Case>>
Reine::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (!position) return coups;

    // 8 directions = 4 orthogonales + 4 diagonales.
    static const int dirs[8][2] = {
        {+1,  0}, {-1,  0}, { 0, +1}, { 0, -1},
        {+1, +1}, {+1, -1}, {-1, +1}, {-1, -1}
    };

    auto ajouterSiPossible = [&](const std::shared_ptr<Case>& cible) -> bool {
        if (!cible) return false;
        if (!cible->estOccupee()) {
            coups.push_back(cible);
            return true;
        }
        if (cible->getPiece()->getCouleur() != couleur) {
            coups.push_back(cible);  // capture
        }
        return false;
    };

    auto proposerCenterCross = [&](const std::shared_ptr<Case>& centre) {
        if (!centre) return;
        const int sext = centre->getSextant();
        for (int delta : {2, 4}) {
            const int sDest = (sext + delta) % 6;
            auto apex = plateau.getCase(OFFSET_X[sDest] + 3,
                                        OFFSET_Y[sDest] + 3);
            if (!ajouterSiPossible(apex)) continue;
            auto courant = apex;
            int sdx = -1, sdy = -1;
            while (true) {
                auto suiv = plateau.voisinDiagonal(courant, sdx, sdy);
                if (!ajouterSiPossible(suiv)) break;
                courant = suiv;
            }
        }
    };

    for (const auto& d : dirs) {
        int dx = d[0];
        int dy = d[1];
        const bool diagonal = (dx != 0 && dy != 0);
        std::shared_ptr<Case> courante = position;

        while (true) {
            // Diagonales -> reflexion aux frontieres (voisinDiagonal).
            // Orthogonales -> rotation (-dy,-dx) (voisinAvecDir).
            auto suivante = diagonal
                ? plateau.voisinDiagonal(courante, dx, dy)
                : plateau.voisinAvecDir(courante, dx, dy);
            if (!ajouterSiPossible(suivante)) break;
            courante = suivante;

            // Center-cross same-color uniquement pour les diagonales
            // arrivant en (3, 3). Pour les orthogonales, le bending de
            // voisinAvecDir suffit a passer dans le sextant suivant.
            if (diagonal && estCentreSextant(courante)) {
                proposerCenterCross(courante);
                break;
            }
        }
    }

    // Si la Reine est deja sur (3, 3), elle peut directement faire un
    // center-cross same-color (comme le Fou).
    if (estCentreSextant(position)) {
        proposerCenterCross(position);
    }

    return coups;
}
