#include "pieces/Pion.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

/**
 * @file Pion.cpp
 * @brief Deplacements du Pion : avance tout droit, capture en diagonale,
 *        bond initial de 2 cases et prise en passant.
 */

// Offsets des sextants dans la grille 12x12.
static const int OFFSET_X[6] = { 0, 0, 8, 8, 4, 4 };
static const int OFFSET_Y[6] = { 0, 4, 4, 8, 8, 0 };

// Indique si le sextant appartient au tiers du joueur.
//   BLANC : S0+S5   ROUGE : S1+S2   NOIR : S3+S4
static bool tiersPropre(Couleur c, int sextant) {
    switch (c) {
        case Couleur::BLANC: return sextant == 0 || sextant == 5;
        case Couleur::ROUGE: return sextant == 1 || sextant == 2;
        case Couleur::NOIR:  return sextant == 3 || sextant == 4;
    }
    return false;
}

Pion::Pion(Couleur couleur) : Piece(couleur) {}

std::string Pion::getType() const { return "Pion"; }

void Pion::calculerDirection(int& dx, int& dy) const {
    dx = 0; dy = 0;
    if (!position) return;

    const int sext = position->getSextant();

    // Sextant pair  (S0,S2,S4) : back rank a yLocal=0, avance selon y.
    // Sextant impair (S1,S3,S5) : back rank a xLocal=0, avance selon x.
    // Tiers propre  -> signe +1 (vers le centre).
    // Tiers etranger -> signe -1 (vers le back rank ennemi).
    const int signe = tiersPropre(couleur, sext) ? +1 : -1;
    if (sext % 2 == 0) dy = signe;
    else               dx = signe;
}

std::vector<std::shared_ptr<Case>>
Pion::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (!position) return coups;

    int dx, dy;
    calculerDirection(dx, dy);
    if (dx == 0 && dy == 0) return coups;

    const int sext   = position->getSextant();
    const int xLocal = position->getX() - OFFSET_X[sext];
    const int yLocal = position->getY() - OFFSET_Y[sext];

    // ── 1. Avancement (rook direction) ──────────────────────────────────
    {
        int fdx = dx, fdy = dy;
        auto devant = plateau.voisinAvecDir(position, fdx, fdy);

        if (devant && !devant->estOccupee()) {
            coups.push_back(devant);

            // Bond initial : 2 cases si le pion n'a jamais bouge.
            if (!aDejaBouge) {
                int fdx2 = fdx, fdy2 = fdy;
                auto deux = plateau.voisinAvecDir(devant, fdx2, fdy2);
                if (deux && !deux->estOccupee())
                    coups.push_back(deux);
            }
        }
    }

    // ── 2. Captures diagonales ──
    // Les 2 diagonales "avant" via voisinDiagonal. Si la diagonale passe
    // par le centre (apex du tiers propre), on propose les 2 apexes adverses.
    {
        const int perpX = (dx != 0) ? 0 : 1;
        const int perpY = (dy != 0) ? 0 : 1;
        const bool surApexPropre =
            (xLocal == 3 && yLocal == 3) && tiersPropre(couleur, sext);

        for (int p : {-1, +1}) {
            const int diagX = dx + p * perpX;
            const int diagY = dy + p * perpY;

            int ldx = diagX, ldy = diagY;
            auto cible = plateau.voisinDiagonal(position, ldx, ldy);

            if (cible) {
                // Capture normale : case diagonale ennemi.
                if (cible->estOccupee()
                    && cible->getPiece()->getCouleur() != couleur)
                    coups.push_back(cible);
                // Prise en passant : case diagonale vide mais armee.
                else if (!cible->estOccupee()
                         && cible == plateau.getCaseEnPassantCible()) {
                    auto casePion = plateau.getCaseEnPassantPion();
                    if (casePion && casePion->estOccupee()
                        && casePion->getPiece()->getCouleur() != couleur)
                        coups.push_back(cible);
                }
            } else if (surApexPropre) {
                // La diagonale traverse le centre du plateau : 2 options
                // de capture sur les apexes des sextants same-color en face.
                for (int delta : {2, 4}) {
                    const int sDest = (sext + delta) % 6;
                    if (tiersPropre(couleur, sDest)) continue;
                    auto apex = plateau.getCase(OFFSET_X[sDest] + 3,
                                                OFFSET_Y[sDest] + 3);
                    if (apex && apex->estOccupee()
                        && apex->getPiece()->getCouleur() != couleur)
                        coups.push_back(apex);
                }
            }
        }
    }

    return coups;
}

bool Pion::estCaseDePromotion(const std::shared_ptr<Case>& dest, Couleur couleur) {
    if (!dest) return false;
    const int sext   = dest->getSextant();
    if (tiersPropre(couleur, sext)) return false;
    const int xLocal = dest->getX() - OFFSET_X[sext];
    const int yLocal = dest->getY() - OFFSET_Y[sext];
    return (sext % 2 == 1) ? (xLocal == 0) : (yLocal == 0);
}
