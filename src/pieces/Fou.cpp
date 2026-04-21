#include "pieces/Fou.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Fou::Fou(Couleur couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position) {
}

std::vector<std::shared_ptr<Case>> Fou::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (position == nullptr) return coups;

    // Les 6 directions diagonales hexagonales du Fou
    // Sur un hexagone, une "diagonale" passe par les coins entre 2 hexagones voisins
    const int directions[6][2] = {
        {+1, +1}, {-1, -1},
        {+2, -1}, {-2, +1},
        {+1, -2}, {-1, +2}
    };

    int q = position->getQ();
    int r = position->getR();

    // Le Fou glisse jusqu'à un obstacle (comme la Tour, mais en diagonale)
    for (const auto& dir : directions) {
        int nq = q;
        int nr = r;
        while (true) {
            nq += dir[0];
            nr += dir[1];
            std::shared_ptr<Case> cible = plateau.getCase(nq, nr);
            if (cible == nullptr) break;  // hors plateau

            if (!cible->estOccupee()) {
                coups.push_back(cible);  // case vide → on continue
            } else {
                if (cible->getPiece()->getCouleur() != couleur) {
                    coups.push_back(cible);  // capture possible
                }
                break;  // dans tous les cas, on s'arrête
            }
        }
    }

    return coups;
}

std::string Fou::getType() const { return "Fou"; }