#ifndef REINE_HPP
#define REINE_HPP

#include "pieces/Piece.hpp"

/**
 * @file Reine.hpp
 * @brief Reine du jeu Yalta = Tour + Fou (glissade dans 8 directions).
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
