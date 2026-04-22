#include "pieces/Pion.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Pion::Pion(Couleur couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position), aDejaBouge(false) {
}

void Pion::getDirectionAvancement(int sextant, int& dx, int& dy) {
    // Convention :
    //   sextants 0, 2, 4 : back rank sur une rangee (y=0/4/8) -> avance +y
    //   sextants 1, 3, 5 : back rank sur une colonne (x=0/8/4) -> avance +x
    if (sextant % 2 == 0) { dx = 0;  dy = +1; }
    else                  { dx = +1; dy = 0;  }
}

void Pion::getDirectionsCaptures(int sextant,
                                 int& dx1, int& dy1,
                                 int& dx2, int& dy2) {
    if (sextant % 2 == 0) {
        // Avance +y : captures en (-1, +1) et (+1, +1)
        dx1 = -1; dy1 = +1;
        dx2 = +1; dy2 = +1;
    } else {
        // Avance +x : captures en (+1, -1) et (+1, +1)
        dx1 = +1; dy1 = -1;
        dx2 = +1; dy2 = +1;
    }
}

std::vector<std::shared_ptr<Case>> Pion::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (position == nullptr) return coups;

    int sextant = position->getSextant();
    int x = position->getX();
    int y = position->getY();

    auto getCaseGrille = [&](int cx, int cy) -> std::shared_ptr<Case> {
        if (cx < 0 || cx >= 12 || cy < 0 || cy >= 12) return nullptr;
        return plateau.getCase(cx, cy);
    };

    // ─── 1. AVANCEMENT ───
    int dx, dy;
    getDirectionAvancement(sextant, dx, dy);

    auto devant = getCaseGrille(x + dx, y + dy);
    if (devant && !devant->estOccupee()) {
        coups.push_back(devant);

        // 2 cases en avant depuis la position de depart
        if (!aDejaBouge) {
            auto deuxDevant = getCaseGrille(x + 2*dx, y + 2*dy);
            if (deuxDevant && !deuxDevant->estOccupee()) {
                coups.push_back(deuxDevant);
            }
        }
    }

    // ─── 2. CAPTURES ───
    int dx1, dy1, dx2, dy2;
    getDirectionsCaptures(sextant, dx1, dy1, dx2, dy2);

    auto diag1 = getCaseGrille(x + dx1, y + dy1);
    if (diag1 && diag1->estOccupee()
        && diag1->getPiece()->getCouleur() != couleur) {
        coups.push_back(diag1);
    }

    auto diag2 = getCaseGrille(x + dx2, y + dy2);
    if (diag2 && diag2->estOccupee()
        && diag2->getPiece()->getCouleur() != couleur) {
        coups.push_back(diag2);
    }

    // Note : prise en passant et promotion non gerees ici.

    return coups;
}

std::string Pion::getType() const { return "Pion"; }

bool Pion::getADejaBouge() const { return aDejaBouge; }
void Pion::setADejaBouge(bool valeur) { aDejaBouge = valeur; }
