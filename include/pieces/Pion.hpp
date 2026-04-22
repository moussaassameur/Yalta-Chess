#ifndef PION_HPP
#define PION_HPP

#include "pieces/Piece.hpp"

class Pion : public Piece {
private:
    bool aDejaBouge;  // false au départ, true après le premier coup

public:
    Pion(Couleur couleur, std::shared_ptr<Case> position);
    ~Pion() override = default;

    std::vector<std::shared_ptr<Case>> getDeplacements(const Plateau& plateau) const override;
    std::string getType() const override;

    bool getADejaBouge() const;
    void setADejaBouge(bool valeur);

private:
    // Direction d'avancement (dx, dy) selon le sextant.
    // Sextants pairs (0, 2, 4) avancent en +y ; impairs (1, 3, 5) en +x.
    static void getDirectionAvancement(int sextant, int& dx, int& dy);

    // Diagonales de capture, perpendiculaires a la direction d'avancement.
    static void getDirectionsCaptures(int sextant,
                                      int& dx1, int& dy1,
                                      int& dx2, int& dy2);
};

#endif
