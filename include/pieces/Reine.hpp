#ifndef REINE_HPP
#define REINE_HPP

#include "pieces/Piece.hpp"

class Reine : public Piece {
public:
    Reine(const std::string& couleur, std::shared_ptr<Case> position);
    ~Reine() override = default;

    std::vector<std::shared_ptr<Case>> getDeplacements(const Plateau& plateau) const override;
    std::string getType() const override;
};

#endif