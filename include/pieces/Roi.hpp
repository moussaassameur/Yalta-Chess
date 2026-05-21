#ifndef ROI_HPP
#define ROI_HPP

#include "pieces/Piece.hpp"

/**
 * @file Roi.hpp
 * @brief Roi du jeu Yalta : se deplace d'une case dans les 8 directions.
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
