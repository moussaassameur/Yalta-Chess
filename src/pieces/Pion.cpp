#include "pieces/Pion.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Pion::Pion(const std::string& couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position), aDejaBouge(false) {
}

void Pion::getDirectionAvancement(int& dq, int& dr) const {
    // Chaque couleur a sa propre direction d'avancement sur l'hexagone
    if (couleur == "blanc") {
        dq = 0; dr = -1;
    } else if (couleur == "noir") {
        dq = -1; dr = +1;
    } else { // "rouge"
        dq = +1; dr = 0;
    }
}

void Pion::getDirectionsCaptures(int& dq1, int& dr1, int& dq2, int& dr2) const {
    // Les 2 diagonales de capture, qui encadrent la direction d'avancement
    if (couleur == "blanc") {
        dq1 = +1; dr1 = -1;
        dq2 = -1; dr2 =  0;
    } else if (couleur == "noir") {
        dq1 =  0; dr1 = +1;
        dq2 = -1; dr2 =  0;
    } else { // "rouge"
        dq1 = +1; dr1 = -1;
        dq2 =  0; dr2 = +1;
    }
}

std::vector<std::shared_ptr<Case>> Pion::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (position == nullptr) return coups;

    int q = position->getQ();
    int r = position->getR();

    // ─── 1. AVANCEMENT (vers l'avant, sans capturer) ───
    int dq, dr;
    getDirectionAvancement(dq, dr);

    // 1 case en avant
    auto caseDevant = plateau.getCase(q + dq, r + dr);
    if (caseDevant != nullptr && !caseDevant->estOccupee()) {
        coups.push_back(caseDevant);

        // 2 cases en avant — uniquement si le pion n'a jamais bougé
        // ET si la case "1 devant" est aussi libre (déjà vérifié juste au-dessus)
        if (!aDejaBouge) {
            auto caseDeuxDevant = plateau.getCase(q + 2*dq, r + 2*dr);
            if (caseDeuxDevant != nullptr && !caseDeuxDevant->estOccupee()) {
                coups.push_back(caseDeuxDevant);
            }
        }
    }

    // ─── 2. CAPTURES (diagonales, uniquement si la case contient un ennemi) ───
    int dq1, dr1, dq2, dr2;
    getDirectionsCaptures(dq1, dr1, dq2, dr2);

    // Première diagonale
    auto diag1 = plateau.getCase(q + dq1, r + dr1);
    if (diag1 != nullptr && diag1->estOccupee()
        && diag1->getPiece()->getCouleur() != couleur) {
        coups.push_back(diag1);
    }

    // Deuxième diagonale
    auto diag2 = plateau.getCase(q + dq2, r + dr2);
    if (diag2 != nullptr && diag2->estOccupee()
        && diag2->getPiece()->getCouleur() != couleur) {
        coups.push_back(diag2);
    }

    // Note : la prise en passant et la promotion ne sont PAS gérées ici.
    // Elles seront ajoutées dans la classe Jeu, qui a une vue d'ensemble
    // (historique du dernier coup, condition de promotion, etc.)

    return coups;
}

std::string Pion::getType() const {
    return "Pion";
}

bool Pion::getADejaBouge() const {
    return aDejaBouge;
}

void Pion::setADejaBouge(bool valeur) {
    aDejaBouge = valeur;
}