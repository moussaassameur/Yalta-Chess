#ifndef PION_HPP
#define PION_HPP

#include "pieces/Piece.hpp"

/**
 * @file Pion.hpp
 * @brief Pion du jeu Yalta : avance tout droit, capture en diagonale.
 */
class Pion : public Piece {
public:
    Pion(Couleur couleur);

    ~Pion() override = default;

    std::string getType() const override;

    std::vector<std::shared_ptr<Case>>
    getDeplacements(const Plateau& plateau) const override;

    // true si 'dest' est une case de promotion pour un pion de cette couleur.
    static bool estCaseDePromotion(const std::shared_ptr<Case>& dest,
                                   Couleur couleur);

private:
    // Calcule la direction d'avancement (dx, dy) du pion.
    void calculerDirection(int& dx, int& dy) const;
};

#endif // PION_HPP
