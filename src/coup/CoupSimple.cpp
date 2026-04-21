#include "coup/CoupSimple.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "pieces/Piece.hpp"

CoupSimple::CoupSimple(std::shared_ptr<Case>  depart,
                       std::shared_ptr<Case>  arrivee,
                       std::shared_ptr<Piece> piece)
    : depart(depart),
      arrivee(arrivee),
      piece(piece),
      pieceCapturee(nullptr),
      aDejaBougeAvant(false) {
}

void CoupSimple::executer(Plateau& /*plateau*/) {
    if (!depart || !arrivee || !piece) return;

    // On sauvegarde la pièce éventuellement capturée
    if (arrivee->estOccupee()) {
        pieceCapturee = arrivee->getPiece();
        if (pieceCapturee->getCouleur() != piece->getCouleur()) {
            pieceCapturee->capturer();
        }
    }

    depart->retirerPiece();
    arrivee->setPiece(piece);
    piece->setPosition(arrivee);
}

void CoupSimple::annuler(Plateau& /*plateau*/) {
    if (!depart || !arrivee || !piece) return;

    arrivee->retirerPiece();
    depart->setPiece(piece);
    piece->setPosition(depart);

    if (pieceCapturee) {
        arrivee->setPiece(pieceCapturee);
        pieceCapturee->ressusciter();
        pieceCapturee->setPosition(arrivee);
        pieceCapturee = nullptr;
    }
}

bool CoupSimple::estValide(const Plateau& /*plateau*/) const {
    if (!depart || !arrivee || !piece) return false;
    if (depart->getPiece() != piece) return false;
    if (arrivee->estOccupee()
        && arrivee->getPiece()->getCouleur() == piece->getCouleur()) return false;
    return true;
}

std::string CoupSimple::getNotation() const {
    if (!piece || !depart || !arrivee) return "?";
    return piece->getType()
         + "(" + std::to_string(depart->getQ())  + "," + std::to_string(depart->getR())  + ")"
         + "-"
         + "(" + std::to_string(arrivee->getQ()) + "," + std::to_string(arrivee->getR()) + ")";
}

std::shared_ptr<Case>  CoupSimple::getDepart()        const { return depart; }
std::shared_ptr<Case>  CoupSimple::getArrivee()       const { return arrivee; }
std::shared_ptr<Piece> CoupSimple::getPiece()         const { return piece; }
std::shared_ptr<Piece> CoupSimple::getPieceCapturee() const { return pieceCapturee; }
