#ifndef COUP_ROQUE_HPP
#define COUP_ROQUE_HPP

#include "coup/Coup.hpp"
#include <memory>
#include <string>

class Case;
class Piece;

/**
 * @file CoupRoque.hpp
 * @brief Roque (kingside) : le Roi et la Tour bougent simultanement.
 *
 * Pattern Command : executer() deplace les deux pieces, annuler() les
 * remet exactement a leur position d'origine.
 *
 * Seul le roque "petit" (kingside) est implemente : le Roi et la Tour
 * sont dans le MEME sextant impair, back rank a xLocal=0.
 *   Blanc  : Roi (4,3) -> (4,1)   Tour (4,0) -> (4,2)
 *   Rouge  : Roi (0,7) -> (0,5)   Tour (0,4) -> (0,6)
 *   Noir   : Roi (8,11)-> (8,9)   Tour (8,8) -> (8,10)
 *
 * Notation algebrique : "O-O".
 */
class CoupRoque : public Coup {
public:
    /**
     * @param roiDepart   Case actuelle du Roi (doit contenir un Roi).
     * @param roiArrivee  Case cible du Roi (doit etre libre).
     * @param tourDepart  Case actuelle de la Tour (doit contenir une Tour).
     * @param tourArrivee Case cible de la Tour (doit etre libre).
     */
    CoupRoque(std::shared_ptr<Case> roiDepart,
              std::shared_ptr<Case> roiArrivee,
              std::shared_ptr<Case> tourDepart,
              std::shared_ptr<Case> tourArrivee);

    ~CoupRoque() override = default;

    void        executer(Plateau& plateau)              override;
    void        annuler(Plateau& plateau)               override;
    bool        estValide(const Plateau& plateau) const override;
    std::string getNotation() const                     override;

private:
    std::shared_ptr<Case>  roiDepart;
    std::shared_ptr<Case>  roiArrivee;
    std::shared_ptr<Case>  tourDepart;
    std::shared_ptr<Case>  tourArrivee;

    std::shared_ptr<Piece> roi;
    std::shared_ptr<Piece> tour;
    bool roiDejaBougeAvant;
    bool tourDejaBougeAvant;
    bool execute;
};

#endif // COUP_ROQUE_HPP
