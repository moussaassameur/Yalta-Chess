#ifndef TOUR_HPP
#define TOUR_HPP

#include "pieces/Piece.hpp"

/**
 * @file Tour.hpp
 * @brief Tour du jeu Yalta : glissade dans les 4 directions droites.
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
