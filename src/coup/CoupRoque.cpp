#include "coup/CoupRoque.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "pieces/Piece.hpp"

/**
 * @file CoupRoque.cpp
 * @brief Implementation de CoupRoque : le Roi et la Tour bougent ensemble.
 */

CoupRoque::CoupRoque(std::shared_ptr<Case> roiDepart,
                     std::shared_ptr<Case> roiArrivee,
                     std::shared_ptr<Case> tourDepart,
                     std::shared_ptr<Case> tourArrivee)
    : roiDepart(roiDepart),
      roiArrivee(roiArrivee),
      tourDepart(tourDepart),
      tourArrivee(tourArrivee),
      roi(nullptr), tour(nullptr),
      roiDejaBougeAvant(false), tourDejaBougeAvant(false),
      execute(false) {
}

void CoupRoque::executer(Plateau& /*plateau*/) {
    if (!roiDepart || !roiArrivee || !tourDepart || !tourArrivee) return;
    if (!roiDepart->estOccupee() || !tourDepart->estOccupee()) return;

    roi  = roiDepart->getPiece();
    tour = tourDepart->getPiece();

    roiDejaBougeAvant  = roi->getADejaBouge();
    tourDejaBougeAvant = tour->getADejaBouge();

    // Retire les deux pieces.
    roiDepart->retirerPiece();
    tourDepart->retirerPiece();

    // Pose le Roi sur sa destination.
    roiArrivee->setPiece(roi);
    roi->setPosition(roiArrivee);
    roi->setADejaBouge(true);

    // Pose la Tour sur sa destination.
    tourArrivee->setPiece(tour);
    tour->setPosition(tourArrivee);
    tour->setADejaBouge(true);

    execute = true;
}

void CoupRoque::annuler(Plateau& /*plateau*/) {
    if (!execute) return;

    // Retire les deux pieces de leurs destinations.
    roiArrivee->retirerPiece();
    tourArrivee->retirerPiece();

    // Remet le Roi a son depart.
    roiDepart->setPiece(roi);
    roi->setPosition(roiDepart);
    roi->setADejaBouge(roiDejaBougeAvant);

    // Remet la Tour a son depart.
    tourDepart->setPiece(tour);
    tour->setPosition(tourDepart);
    tour->setADejaBouge(tourDejaBougeAvant);

    execute = false;
}

bool CoupRoque::estValide(const Plateau& plateau) const {
    if (!roiDepart || !roiArrivee || !tourDepart || !tourArrivee) return false;
    if (!roiDepart->estOccupee() || !tourDepart->estOccupee()) return false;

    auto roiPiece  = roiDepart->getPiece();
    auto tourPiece = tourDepart->getPiece();

    if (!roiPiece  || roiPiece->getType()  != "Roi")  return false;
    if (!tourPiece || tourPiece->getType() != "Tour") return false;
    if (roiPiece->getCouleur() != tourPiece->getCouleur()) return false;

    // Ni le Roi ni la Tour ne doivent avoir deja bouge.
    if (roiPiece->getADejaBouge())  return false;
    if (tourPiece->getADejaBouge()) return false;

    // Les cases intermediaires doivent etre libres.
    if (roiArrivee->estOccupee())  return false;
    if (tourArrivee->estOccupee()) return false;

    // Le Roi ne doit pas etre en echec.
    if (plateau.estEnEchec(roiPiece->getCouleur())) return false;

    return true;
}
