#ifndef CAVALIER_HPP
#define CAVALIER_HPP

#include "pieces/Piece.hpp"

class Cavalier : public Piece {
public:
    Cavalier(const std::string& couleur, std::shared_ptr<Case> position);
    ~Cavalier() override = default;

    std::vector<std::shared_ptr<Case>> getDeplacements(const Plateau& plateau) const override;
    std::string getType() const override;
};

#endif