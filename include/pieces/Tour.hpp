#ifndef TOUR_HPP
#define TOUR_HPP

#include "pieces/Piece.hpp"

/**
 * @file Tour.hpp
 * @brief Tour du jeu Yalta -- glissade orthogonale.
 *
 * La Tour glisse dans les 4 directions cardinales (xLocal+/-, yLocal+/-)
 * et utilise la topologie de bending de Plateau aux frontieres internes.
 * Elle s'arrete sur :
 *   - un bord exterieur du plateau (case nullptr renvoyee par voisin),
 *   - une piece amie (la case n'est pas atteignable),
 *   - une piece ennemie (case ajoutee comme capture, puis on s'arrete).
 *
 * Le drapeau aDejaBouge (herite de Piece) servira pour le roque en
 * Phase 7.
 */
class Tour : public Piece {
public:
    Tour(Couleur couleur);
    ~Tour() override = default;

    std::string getType() const override;

    std::vector<std::shared_ptr<Case>>
    getDeplacements(const Plateau& plateau) const override;
};

#endif // TOUR_HPP
