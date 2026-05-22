#include "coup/CoupPriseEnPassant.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "pieces/Piece.hpp"

/**
 * @file CoupPriseEnPassant.cpp
 * @brief Capture en passant -- Pattern Command.
 *
 * Specificite : la case d'arrivee est vide pendant tout le coup. La piece
 * capturee est sur une case adjacente (casePionPris), pas sur l'arrivee.
 */

CoupPriseEnPassant::CoupPriseEnPassant(std::shared_ptr<Case> depart,
                                       std::shared_ptr<Case> arrivee,
                                       std::shared_ptr<Case> casePionPris)
    : depart(depart),
      arrivee(arrivee),
      casePionPris(casePionPris),
      pieceDeplacee(nullptr),
      pieceCapturee(nullptr),
      aDejaBougeAvant(false),
      execute(false) {
}

void CoupPriseEnPassant::executer(Plateau& /*plateau*/) {
    if (!depart || !arrivee || !casePionPris) return;
    if (!depart->estOccupee() || !casePionPris->estOccupee()) return;

    pieceDeplacee   = depart->getPiece();
    pieceCapturee   = casePionPris->getPiece();
    aDejaBougeAvant = pieceDeplacee->getADejaBouge();

    // Retire le pion adverse de sa case (pas celle d'arrivee !).
    pieceCapturee->capturer();
    casePionPris->retirerPiece();

    // Deplace notre pion sur la case sautee.
    depart->retirerPiece();
    arrivee->setPiece(pieceDeplacee);
    pieceDeplacee->setPosition(arrivee);
    pieceDeplacee->setADejaBouge(true);

    execute = true;
}

void CoupPriseEnPassant::annuler(Plateau& /*plateau*/) {
    if (!execute) return;
    if (!depart || !arrivee || !casePionPris || !pieceDeplacee) return;

    // Remet notre pion sur sa case de depart.
    arrivee->retirerPiece();
    depart->setPiece(pieceDeplacee);
    pieceDeplacee->setPosition(depart);
    pieceDeplacee->setADejaBouge(aDejaBougeAvant);

    // Restaure le pion ennemi sur sa case d'origine.
    if (pieceCapturee) {
        pieceCapturee->ressusciter();
        casePionPris->setPiece(pieceCapturee);
        pieceCapturee->setPosition(casePionPris);
    }

    execute = false;
}

bool CoupPriseEnPassant::estValide(const Plateau& /*plateau*/) const {
    if (!depart || !arrivee || !casePionPris) return false;
    if (!depart->estOccupee()) return false;
    if (!casePionPris->estOccupee()) return false;
    if (arrivee->estOccupee()) return false;

    auto attaquant = depart->getPiece();
    auto victime   = casePionPris->getPiece();
    if (attaquant->getType() != "Pion" || victime->getType() != "Pion") return false;
    if (attaquant->getCouleur() == victime->getCouleur()) return false;

    return true;
}
