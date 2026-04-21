#ifndef COUPSIMPLE_HPP
#define COUPSIMPLE_HPP

#include "coup/Coup.hpp"
#include <memory>

class Case;
class Piece;

// Déplacement standard d'une pièce, avec capture éventuelle
class CoupSimple : public Coup {
private:
    std::shared_ptr<Case>  depart;
    std::shared_ptr<Case>  arrivee;
    std::shared_ptr<Piece> piece;
    std::shared_ptr<Piece> pieceCapturee; // null si pas de capture (stockée pour annuler)
    bool aDejaBougeAvant;                 // état de aDejaBouge avant le coup

public:
    CoupSimple(std::shared_ptr<Case>  depart,
               std::shared_ptr<Case>  arrivee,
               std::shared_ptr<Piece> piece);

    void executer(Plateau& plateau) override;
    void annuler(Plateau& plateau) override;
    bool estValide(const Plateau& plateau) const override;
    std::string getNotation() const override;

    std::shared_ptr<Case>  getDepart()        const;
    std::shared_ptr<Case>  getArrivee()       const;
    std::shared_ptr<Piece> getPiece()         const;
    std::shared_ptr<Piece> getPieceCapturee() const;
};

#endif // COUPSIMPLE_HPP
