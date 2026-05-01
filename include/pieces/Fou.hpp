#ifndef FOU_HPP
#define FOU_HPP

#include "pieces/Piece.hpp"

/**
 * @file Fou.hpp
 * @brief Fou du jeu Yalta -- glissade diagonale + center-cross.
 *
 * Le Fou glisse dans les 4 directions diagonales et utilise la topologie
 * de bending de Plateau. Il reste sur des cases de la meme couleur damier
 * que sa case de depart (propriete classique des fous aux echecs).
 *
 * Specifite Yalta -- center-cross :
 *   - Lorsque le Fou atteint la case centrale (3, 3) d'un sextant
 *     (= origine geometrique du plateau), il peut "sauter" sur les cases
 *     (3, 3) des deux autres sextants same-color, soit (i+2)%6 et
 *     (i+4)%6 ou i est le sextant courant.
 *   - Cette regle modelise les "deux diagonales offertes au centre" qui
 *     existent sur l'hexagone Yalta.
 *
 * Dans cette version, le Fou s'arrete sur la case (3, 3) cible apres
 * un center-cross -- il ne continue pas a glisser dans le sextant
 * d'arrivee (a affiner ulterieurement si besoin).
 */
class Fou : public Piece {
public:
    Fou(Couleur couleur);
    ~Fou() override = default;

    std::string getType() const override;

    std::vector<std::shared_ptr<Case>>
    getDeplacements(const Plateau& plateau) const override;
};

#endif // FOU_HPP
