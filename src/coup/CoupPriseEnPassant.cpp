#include "coup/CoupPriseEnPassant.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "pieces/Pion.hpp"

CoupPriseEnPassant::CoupPriseEnPassant(std::shared_ptr<Pion> pionAttaquant,
                                       std::shared_ptr<Pion> pionCapture,
                                       std::shared_ptr<Case> caseDepart,
                                       std::shared_ptr<Case> caseArrivee)
    : pionAttaquant(pionAttaquant), pionCapture(pionCapture),
      caseDepart(caseDepart), caseArrivee(caseArrivee) {
}

void CoupPriseEnPassant::executer(Plateau& /*plateau*/) {
    // TODO : implémenter lors de l'étape ModeleJeu
}

void CoupPriseEnPassant::annuler(Plateau& /*plateau*/) {
    // TODO : implémenter lors de l'étape ModeleJeu
}

bool CoupPriseEnPassant::estValide(const Plateau& /*plateau*/) const {
    // TODO : vérifier que le dernier coup joué était un double-pas du pionCapture
    return false;
}

std::string CoupPriseEnPassant::getNotation() const {
    return "e.p.";
}
