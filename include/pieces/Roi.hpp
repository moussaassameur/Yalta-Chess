#ifndef ROI_HPP
#define ROI_HPP

#include "pieces/Piece.hpp"

/**
 * @file Roi.hpp
 * @brief Roi du jeu Yalta -- 1 case dans 8 directions + regle centre.
 *
 * Le Roi se deplace d'une case dans n'importe laquelle des 8 directions
 * (4 orthogonales + 4 diagonales). Le bending de Plateau s'applique
 * normalement.
 *
 * Specifite Yalta -- restriction au centre :
 *   Depuis la case centrale (3, 3) d'un sextant, le Roi ne peut pas
 *   aller directement sur une case (3, 3) d'un autre sextant si celle-ci
 *   est de couleur damier opposee. Concretement :
 *     - les sextants pairs (0, 2, 4) ont leur (3, 3) clair
 *     - les sextants impairs (1, 3, 5) ont leur (3, 3) fonce
 *   donc une transition (3,3) S0 -> (3,3) S5 (couleur opposee, accessible
 *   en theorie via le bending orthogonal) est interdite.
 *
 * Le Roi peut en revanche faire le "center-cross same-color" : depuis
 * (3, 3), il atteint les (3, 3) des sextants (i+2)%6 et (i+4)%6 qui
 * ont la meme couleur damier.
 *
 * Le drapeau aDejaBouge (herite de Piece) sera utilise pour le roque
 * (Phase 7).
 */
class Roi : public Piece {
public:
    Roi(Couleur couleur);
    ~Roi() override = default;

    std::string getType() const override;

    std::vector<std::shared_ptr<Case>>
    getDeplacements(const Plateau& plateau) const override;
};

#endif // ROI_HPP
