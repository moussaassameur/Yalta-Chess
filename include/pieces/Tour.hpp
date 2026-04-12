#ifndef TOUR_HPP
#define TOUR_HPP

#include "pieces/Piece.hpp"

class Tour : public Piece {
private:
    bool aDejaBouge;  // pour le roque

public:
    Tour(const std::string& couleur, std::shared_ptr<Case> position);
    ~Tour() override = default;

    std::vector<std::shared_ptr<Case>> getDeplacements(const Plateau& plateau) const override;
    std::string getType() const override;

    bool getADejaBouge() const;
    void setADejaBouge(bool valeur);
};

#endif