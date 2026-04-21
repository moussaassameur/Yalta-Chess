#include "coup/CoupRoque.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "pieces/Roi.hpp"
#include "pieces/Tour.hpp"

CoupRoque::CoupRoque(std::shared_ptr<Roi>  roi,
                     std::shared_ptr<Tour> tour,
                     std::shared_ptr<Case> caseRoiApres,
                     std::shared_ptr<Case> caseTourApres)
    : roi(roi), tour(tour),
      caseRoiApres(caseRoiApres), caseTourApres(caseTourApres) {
    // On sauvegarde les positions initiales dès la construction
    caseRoiAvant  = roi  ? roi->getPosition()  : nullptr;
    caseTourAvant = tour ? tour->getPosition() : nullptr;
}

void CoupRoque::executer(Plateau& /*plateau*/) {
    // TODO : implémenter lors de l'étape ModeleJeu
}

void CoupRoque::annuler(Plateau& /*plateau*/) {
    // TODO : implémenter lors de l'étape ModeleJeu
}

bool CoupRoque::estValide(const Plateau& /*plateau*/) const {
    // TODO : vérifier aDejaBouge du roi et de la tour + cases libres entre eux
    return false;
}

std::string CoupRoque::getNotation() const {
    return "O-O";
}
