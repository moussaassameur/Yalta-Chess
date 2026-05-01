#ifndef CAVALIER_HPP
#define CAVALIER_HPP

#include "pieces/Piece.hpp"

/**
 * @file Cavalier.hpp
 * @brief Cavalier du jeu Yalta -- saut en L decompose pas a pas.
 *
 * Sur un echiquier classique le Cavalier saute en L : 2 cases dans une
 * direction puis 1 case perpendiculaire (8 destinations possibles). En
 * Yalta, comme la grille a des frontieres internes courbees, on ne peut
 * pas calculer les destinations par un simple offset (dx=2, dy=1) -- il
 * faut suivre chaque pas avec le bending.
 *
 * Algorithme :
 *   - 4 combinaisons de signes (sx, sy) ∈ {+1, -1}^2.
 *   - Pour chaque combinaison, on essaie les 4 sequences :
 *       1 (sx,0)+2 (0,sy)   "axe x d'abord, puis 2 cases axe y"
 *       1 (0,sy)+2 (sx,0)   "axe y d'abord, puis 2 cases axe x"
 *       2 (sx,0)+1 (0,sy)   "2 cases axe x, puis 1 axe y"
 *       2 (0,sy)+1 (sx,0)   "2 cases axe y, puis 1 axe x"
 *   - On suit chaque pas via Plateau::voisinAvecDir (avec bending).
 *   - On deduplique les destinations (les chemins 1+2 et 2+1 peuvent
 *     converger ou diverger selon la geometrie).
 *
 * Le cavalier saute par-dessus les pieces : seule la case d'arrivee est
 * verifiee pour l'occupation.
 */
class Cavalier : public Piece {
public:
    Cavalier(Couleur couleur);
    ~Cavalier() override = default;

    std::string getType() const override;

    std::vector<std::shared_ptr<Case>>
    getDeplacements(const Plateau& plateau) const override;
};

#endif // CAVALIER_HPP
