#ifndef COUP_PROMOTION_HPP
#define COUP_PROMOTION_HPP

#include "coup/Coup.hpp"
#include <memory>
#include <string>

class Case;
class Piece;

/**
 * @file CoupPromotion.hpp
 * @brief Promotion : un pion atteint le bord et devient une autre piece.
 */
class CoupPromotion : public Coup {
public:
    // typePiece : "Reine", "Tour", "Fou" ou "Cavalier".
    CoupPromotion(std::shared_ptr<Case> depart,
                  std::shared_ptr<Case> arrivee,
                  const std::string&    typePiece);

    ~CoupPromotion() override = default;

    void        executer(Plateau& plateau)              override;
    void        annuler(Plateau& plateau)               override;
    bool        estValide(const Plateau& plateau) const override;

private:
    std::shared_ptr<Case>  depart;
    std::shared_ptr<Case>  arrivee;
    std::string            typePiece;

    std::shared_ptr<Piece> pion;           ///< Pion deplace (sauvegarde pour annuler).
    std::shared_ptr<Piece> piecePromotion; ///< Piece cree a la promotion.
    std::shared_ptr<Piece> pieceCapturee;  ///< Eventuelle piece adverse sur arrivee.
    bool                   aDejaBougeAvant;
    bool                   execute;
};

#endif // COUP_PROMOTION_HPP
