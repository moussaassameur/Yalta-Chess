#ifndef FOU_HPP
#define FOU_HPP

#include "pieces/Piece.hpp"

/**
 * @file Fou.hpp
 * @brief Fou du jeu Yalta : glissade diagonale + traversee du centre.
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
