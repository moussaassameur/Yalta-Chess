#ifndef COUPPROMOTION_HPP
#define COUPPROMOTION_HPP

#include "coup/Coup.hpp"
#include <memory>
#include <string>

class Case;
class Piece;
class Pion;

// Mouvement spécial : un pion atteint la dernière rangée et est promu
class CoupPromotion : public Coup {
private:
    std::shared_ptr<Pion>  pion;
    std::string            typePromotion; // "Reine", "Tour", "Fou" ou "Cavalier"
    std::shared_ptr<Case>  caseArrivee;
    std::shared_ptr<Piece> pieceCapturee; // null si pas de capture sur la case de promotion
    std::shared_ptr<Piece> nouvellePiece; // la pièce créée lors de la promotion

public:
    CoupPromotion(std::shared_ptr<Pion>  pion,
                  std::shared_ptr<Case>  caseArrivee,
                  const std::string&     typePromotion,
                  std::shared_ptr<Piece> pieceCapturee = nullptr);

    void executer(Plateau& plateau) override;
    void annuler(Plateau& plateau) override;
    bool estValide(const Plateau& plateau) const override;
    std::string getNotation() const override;
};

#endif // COUPPROMOTION_HPP
