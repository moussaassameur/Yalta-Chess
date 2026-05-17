#include "pieces/Roi.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

/**
 * @file Roi.cpp
 * @brief Implementation des deplacements du Roi (1 case + regle Yalta centre).
 */

static const int OFFSET_X[6] = { 0, 0, 8, 8, 4, 4 };
static const int OFFSET_Y[6] = { 0, 4, 4, 8, 8, 0 };

Roi::Roi(Couleur couleur) : Piece(couleur) {}

std::string Roi::getType() const { return "Roi"; }

/// Indique si la case est la (3, 3) de son sextant.
static bool estCentreSextant(const std::shared_ptr<Case>& c) {
    if (!c) return false;
    const int s = c->getSextant();
    return (c->getX() - OFFSET_X[s] == 3
         && c->getY() - OFFSET_Y[s] == 3);
}

std::vector<std::shared_ptr<Case>>
Roi::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (!position) return coups;

    static const int dirs[8][2] = {
        {+1,  0}, {-1,  0}, { 0, +1}, { 0, -1},
        {+1, +1}, {+1, -1}, {-1, +1}, {-1, -1}
    };

    const bool roiSurCentre = estCentreSextant(position);

    // Helper : ajoute une case a la liste si :
    //   - elle existe,
    //   - elle n'est pas occupee par un ami,
    //   - la regle Yalta du centre n'est pas violee.
    auto ajouterSiLegal = [&](const std::shared_ptr<Case>& cible) {
        if (!cible) return;
        if (cible->estOccupee()
            && cible->getPiece()->getCouleur() == couleur) return;

        // Regle Yalta : si on part de (3, 3) et qu'on arrive sur une
        // autre (3, 3) de couleur damier opposee, c'est interdit.
        if (roiSurCentre
            && estCentreSextant(cible)
            && cible->getCouleurDamier() != position->getCouleurDamier()) {
            return;
        }

        coups.push_back(cible);
    };

    // 1 case dans les 8 directions
    for (const auto& d : dirs) {
        auto cible = plateau.voisin(position, d[0], d[1]);
        ajouterSiLegal(cible);
    }

    // Roque kingside (impair sextant, xLocal=0, yLocal=3)
    // Conditions : roi non bouge, tour non bougee, chemin libre.
    // (La verification "pas en echec" est faite dans CoupRoque::estValide,
    //  l'appeler ici provoquerait une recursion infinie via estEnEchec).
    if (!aDejaBouge) {
        const int sext = position->getSextant();
        if (sext % 2 == 1) {
            const int xL = position->getX() - OFFSET_X[sext];
            const int yL = position->getY() - OFFSET_Y[sext];
            if (xL == 0 && yL == 3) {
                auto caseTour = plateau.getCase(OFFSET_X[sext], OFFSET_Y[sext]);
                auto caseLib1 = plateau.getCase(OFFSET_X[sext], OFFSET_Y[sext] + 1);
                auto caseLib2 = plateau.getCase(OFFSET_X[sext], OFFSET_Y[sext] + 2);
                if (caseTour && caseTour->estOccupee()
                    && caseTour->getPiece()->getType() == "Tour"
                    && caseTour->getPiece()->getCouleur() == couleur
                    && !caseTour->getPiece()->getADejaBouge()
                    && caseLib1 && !caseLib1->estOccupee()
                    && caseLib2 && !caseLib2->estOccupee()) {
                    coups.push_back(caseLib1); // destination du roi (yLocal=1)
                }
            }
        }
    }

    // Center-cross same-color depuis (3, 3)
    // Les destinations (i+2)%6 et (i+4)%6 ont la meme couleur damier
    // que la source -- la regle Yalta ne s'oppose pas.
    if (roiSurCentre) {
        const int sext = position->getSextant();
        for (int delta : {2, 4}) {
            const int sDest = (sext + delta) % 6;
            auto cible = plateau.getCase(OFFSET_X[sDest] + 3,
                                         OFFSET_Y[sDest] + 3);
            ajouterSiLegal(cible);
        }
    }

    return coups;
}
