#include "coup/CoupPromotion.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "pieces/Piece.hpp"
#include "pieces/Reine.hpp"
#include "pieces/Tour.hpp"
#include "pieces/Fou.hpp"
#include "pieces/Cavalier.hpp"

/**
 * @file CoupPromotion.cpp
 * @brief Implementation de CoupPromotion : un pion atteint le bord et est
 *        remplace par la piece choisie.
 */

CoupPromotion::CoupPromotion(std::shared_ptr<Case> depart,
                             std::shared_ptr<Case> arrivee,
                             const std::string&    typePiece)
    : depart(depart),
      arrivee(arrivee),
      typePiece(typePiece),
      pion(nullptr),
      piecePromotion(nullptr),
      pieceCapturee(nullptr),
      aDejaBougeAvant(false),
      execute(false) {
}

void CoupPromotion::executer(Plateau& /*plateau*/) {
    if (!depart || !arrivee || !depart->estOccupee()) return;

    pion            = depart->getPiece();
    aDejaBougeAvant = pion->getADejaBouge();

    // Capture eventuelle sur l'arrivee.
    if (arrivee->estOccupee()) {
        pieceCapturee = arrivee->getPiece();
        if (pieceCapturee->getCouleur() != pion->getCouleur())
            pieceCapturee->capturer();
        else
            pieceCapturee = nullptr; // amie : on ne capture pas
    }

    // Retire le pion du plateau.
    depart->retirerPiece();
    pion->capturer();

    // Cree et pose la piece de promotion.
    const Couleur c = pion->getCouleur();
    if      (typePiece == "Tour")     piecePromotion = std::make_shared<Tour>(c);
    else if (typePiece == "Fou")      piecePromotion = std::make_shared<Fou>(c);
    else if (typePiece == "Cavalier") piecePromotion = std::make_shared<Cavalier>(c);
    else                              piecePromotion = std::make_shared<Reine>(c);

    arrivee->setPiece(piecePromotion);
    piecePromotion->setPosition(arrivee);
    piecePromotion->setADejaBouge(true);

    execute = true;
}

void CoupPromotion::annuler(Plateau& /*plateau*/) {
    if (!execute) return;

    // Retire la piece de promotion.
    arrivee->retirerPiece();
    piecePromotion->capturer();

    // Restaure le pion au depart.
    pion->ressusciter();
    depart->setPiece(pion);
    pion->setPosition(depart);
    pion->setADejaBouge(aDejaBougeAvant);

    // Restaure la piece capturee eventuelle.
    if (pieceCapturee) {
        pieceCapturee->ressusciter();
        arrivee->setPiece(pieceCapturee);
        pieceCapturee->setPosition(arrivee);
    }

    execute = false;
}

bool CoupPromotion::estValide(const Plateau& /*plateau*/) const {
    if (!depart || !arrivee) return false;
    if (!depart->estOccupee()) return false;
    if (arrivee->estOccupee()
        && arrivee->getPiece()->getCouleur() == depart->getPiece()->getCouleur())
        return false;
    return true;
}

std::string CoupPromotion::getNotation() const {
    if (!depart || !arrivee) return "?";
    const char sep = pieceCapturee ? 'x' : '-';
    std::string s = depart->getNotation();
    s += sep;
    s += arrivee->getNotation();
    s += '=';
    s += typePiece[0]; // R, T, F, C
    return s;
}
