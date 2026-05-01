#ifndef REINE_HPP
#define REINE_HPP

#include "pieces/Piece.hpp"

/**
 * @file Reine.hpp
 * @brief Reine du jeu Yalta = Tour + Fou.
 *
 * Glissade dans les 8 directions (4 orthogonales + 4 diagonales) avec
 * bending Yalta. Le center-cross same-color depuis (3, 3) ne s'applique
 * qu'aux diagonales (cf. Fou) -- pour les directions orthogonales,
 * le bending classique de Plateau::voisinAvecDir suffit a faire passer
 * la Reine d'un sextant a l'autre.
 */
class Reine : public Piece {
public:
    Reine(Couleur couleur);
    ~Reine() override = default;

    std::string getType() const override;

    std::vector<std::shared_ptr<Case>>
    getDeplacements(const Plateau& plateau) const override;
};

#endif // REINE_HPP
