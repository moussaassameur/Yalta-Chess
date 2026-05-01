#include "coup/CoupSimple.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "pieces/Piece.hpp"

/**
 * @file CoupSimple.cpp
 * @brief Implementation de CoupSimple (deplacement + capture eventuelle).
 *
 * executer() :
 *   1. Sauvegarde la piece du depart (pieceDeplacee) et son drapeau
 *      aDejaBouge avant modification.
 *   2. Sauvegarde la piece eventuellement presente sur l'arrivee
 *      (pieceCapturee) et la marque comme capturee (estVivante false).
 *   3. Vide la case de depart, place la piece sur l'arrivee, met a jour
 *      la position interne de la piece et son drapeau aDejaBouge.
 *
 * annuler() : effectue les operations symetriques pour ramener le
 *      plateau a son etat avant executer().
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
        // On ne capture que si c'est une piece adverse. Si c'est une
        // piece amie, c'est un coup illegal -- mais ici on suppose que
        // estValide a deja ete appele en amont.
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
    // Pas de capture amie : si l'arrivee a une piece de la meme couleur
    // que celle qui se deplace, le coup est invalide.
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
