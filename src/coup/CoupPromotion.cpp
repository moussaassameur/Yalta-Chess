#include "coup/CoupPromotion.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "pieces/Pion.hpp"

CoupPromotion::CoupPromotion(std::shared_ptr<Pion>  pion,
                             std::shared_ptr<Case>  caseArrivee,
                             const std::string&     typePromotion,
                             std::shared_ptr<Piece> pieceCapturee)
    : pion(pion), typePromotion(typePromotion),
      caseArrivee(caseArrivee), pieceCapturee(pieceCapturee),
      nouvellePiece(nullptr) {
}

void CoupPromotion::executer(Plateau& /*plateau*/) {
    // TODO : implémenter lors de l'étape ModeleJeu
    // Créer la nouvelle pièce selon typePromotion, placer sur caseArrivee, capturer le pion
}

void CoupPromotion::annuler(Plateau& /*plateau*/) {
    // TODO : implémenter lors de l'étape ModeleJeu
}

bool CoupPromotion::estValide(const Plateau& /*plateau*/) const {
    // TODO : vérifier que le pion atteint bien la dernière rangée
    return false;
}

std::string CoupPromotion::getNotation() const {
    return "=" + typePromotion;
}
