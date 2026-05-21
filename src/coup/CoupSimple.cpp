#include "coup/CoupSimple.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "pieces/Piece.hpp"

/**
 * @file CoupSimple.cpp
 * @brief Implementation de CoupSimple : deplacement avec capture eventuelle.
 */

CoupSimple::CoupSimple(std::shared_ptr<Case> depart,
                       std::shared_ptr<Case> arrivee)
    : depart(depart),
      arrivee(arrivee),
      pieceDeplacee(nullptr),
      pieceCapturee(nullptr),
      aDejaBougeAvant(false),
      execute(false) {
}

void CoupSimple::executer(Plateau& /*plateau*/) {
    if (!depart || !arrivee) return;
    if (!depart->estOccupee()) return;

    pieceDeplacee   = depart->getPiece();
    aDejaBougeAvant = pieceDeplacee->getADejaBouge();

    if (arrivee->estOccupee()) {
        pieceCapturee = arrivee->getPiece();
        // On ne capture qu'une piece adverse.
        if (pieceCapturee->getCouleur() != pieceDeplacee->getCouleur()) {
            pieceCapturee->capturer();
        }
    }

    depart->retirerPiece();
    arrivee->setPiece(pieceDeplacee);
    pieceDeplacee->setPosition(arrivee);
    pieceDeplacee->setADejaBouge(true);

    execute = true;
}

void CoupSimple::annuler(Plateau& /*plateau*/) {
    if (!execute) return;
    if (!depart || !arrivee || !pieceDeplacee) return;

    // 1. Sortir l'attaquant de l'arrivee, le replacer au depart.
    arrivee->retirerPiece();
    depart->setPiece(pieceDeplacee);
    pieceDeplacee->setPosition(depart);
    pieceDeplacee->setADejaBouge(aDejaBougeAvant);

    // 2. Restaurer la victime eventuelle.
    if (pieceCapturee) {
        pieceCapturee->ressusciter();
        arrivee->setPiece(pieceCapturee);
        pieceCapturee->setPosition(arrivee);
    }

    execute = false;
}

bool CoupSimple::estValide(const Plateau& /*plateau*/) const {
    if (!depart || !arrivee) return false;
    if (!depart->estOccupee()) return false;
    // Pas de capture d'une piece amie.
    if (arrivee->estOccupee()
        && arrivee->getPiece()->getCouleur() == depart->getPiece()->getCouleur()) {
        return false;
    }
    return true;
}

std::string CoupSimple::getNotation() const {
    if (!depart || !arrivee) return "?";
    const char sep = (pieceCapturee != nullptr) ? 'x' : '-';
    std::string s;
    s += depart->getNotation();
    s += sep;
    s += arrivee->getNotation();
    return s;
}

std::shared_ptr<Case>  CoupSimple::getDepart()        const { return depart; }
std::shared_ptr<Case>  CoupSimple::getArrivee()       const { return arrivee; }
std::shared_ptr<Piece> CoupSimple::getPiece()         const { return pieceDeplacee; }
std::shared_ptr<Piece> CoupSimple::getPieceCapturee() const { return pieceCapturee; }
bool                   CoupSimple::estCapture()       const { return pieceCapturee != nullptr; }
