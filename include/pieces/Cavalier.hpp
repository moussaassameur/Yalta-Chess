#ifndef CAVALIER_HPP
#define CAVALIER_HPP

#include "pieces/Piece.hpp"

/**
 * @file Cavalier.hpp
 * @brief Cavalier du jeu Yalta : saut en L, suit le bending pas a pas.
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
