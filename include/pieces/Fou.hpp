#ifndef FOU_HPP
#define FOU_HPP

#include "pieces/Piece.hpp"

class Fou : public Piece {
public:
    Fou(Couleur couleur, std::shared_ptr<Case> position);
    ~Fou() override = default;

    std::vector<std::shared_ptr<Case>> getDeplacements(const Plateau& plateau) const override;
    std::string getType() cons