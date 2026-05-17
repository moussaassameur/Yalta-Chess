#ifndef COUP_PRISE_EN_PASSANT_HPP
#define COUP_PRISE_EN_PASSANT_HPP

#include "coup/Coup.hpp"
#include <memory>

class Case;
class Piece;

/**
 * @file CoupPriseEnPassant.hpp
 * @brief Prise en passant : capture speciale du pion qui vient d'avancer
 *        de 2 cases.
 *
 * Particularite : le pion capture n'est PAS sur la case d'arrivee du pion
 * qui capture -- il est sur une case adjacente (celle qu'il a atteinte
 * apres son bond de 2). La case d'arrivee est la case qu'il a "sautee".
 */
class CoupPriseEnPassant : public Coup {
public:
    /**
     * @param depart        Case du pion qui capture.
     * @param arrivee       Case sautee par le pion adverse (destination).
     * @param casePionPris  Case du pion adverse a retirer.
     */
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
