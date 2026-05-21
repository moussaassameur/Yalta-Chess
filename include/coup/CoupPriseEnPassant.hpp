#ifndef COUP_PRISE_EN_PASSANT_HPP
#define COUP_PRISE_EN_PASSANT_HPP

#include "coup/Coup.hpp"
#include <memory>

class Case;
class Piece;

/**
 * @file CoupPriseEnPassant.hpp
 * @brief Prise en passant : capture du pion adverse qui vient de faire
 *        un bond de 2 cases.
 */
class CoupPriseEnPassant : public Coup {
public:
    // depart : pion qui capture. arrivee : case sautee. casePionPris : pion retire.
    CoupPriseEnPassant(std::shared_ptr<Case> depart,
                       std::shared_ptr<Case> arrivee,
                       std::shared_ptr<Case> casePionPris);

    ~CoupPriseEnPassant() override = default;

    void        executer(Plateau& plateau)              override;
    void        annuler(Plateau& plateau)               override;
    bool        estValide(const Plateau& plateau) const override;
    std::string getNotation() const                     override;

private:
    std::shared_ptr<Case>  depart;
    std::shared_ptr<Case>  arrivee;
    std::shared_ptr<Case>  casePionPris;

    std::shared_ptr<Piece> pieceDeplacee;
    std::shared_ptr<Piece> pieceCapturee;
    bool aDejaBougeAvant;
    bool execute;
};

#endif // COUP_PRISE_EN_PASSANT_HPP
