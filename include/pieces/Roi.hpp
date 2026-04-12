#ifndef ROI_HPP
#define ROI_HPP

#include "pieces/Piece.hpp"

class Roi : public Piece {
private:
    bool aDejaBouge;  // utile pour le roque

public:
    Roi(const std::string& couleur, std::shared_ptr<Case> position);
    ~Roi() override = default;

    std::vector<std::shared_ptr<Case>> getDeplacements(const Plateau& plateau) const override;
    std::string getType() const override;

    bool getADejaBouge() const;
    void setADejaBouge(bool valeur);
};

#endif