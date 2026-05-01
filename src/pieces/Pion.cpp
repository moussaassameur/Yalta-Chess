#include "pieces/Pion.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

/**
 * @file Pion.cpp
 * @brief Deplacements du Pion selon les regles Yalta officielles.
 *
 * Avec la notation globale, chaque pion reste sur la meme colonne (file)
 * tout au long de sa vie : un pion sur la file 'a' passe de a1→a2→a3→a4
 * (sextant S0), puis a5→a6→a7→a8 (sextant S1). Un pion sur la file 'e'
 * passe de e1→e4 (S5), franchit le centre, puis e9→e12 (S4).
 *
 * Regles implementees :
 *   1. Avancement : 1 case dans la direction du "rook" (orthogonale)
 *      qui eloigne le pion de son back rank. Le bending de Plateau::
 *      voisinAvecDir gere automatiquement le passage d'un sextant a l'autre.
 *   2. Bond initial : si le pion n'a pas encore bouge, il peut avancer
 *      de 2 cases (les 2 cases devant lui doivent etre libres).
 *   3. Capture diagonale : 1 case dans une des 2 directions diagonales
 *      "avant" (composante perpendiculaire + composante forward). On utilise
 *      Plateau::voisinDiagonal qui applique la reflexion aux frontieres.
 *   4. Center-cross capture : si la diagonale sort simultanement par les
 *      deux bords d'un sextant (cas de l'apex vers le centre), le pion a
 *      deux options de capture -- les apexes des deux sextants de meme
 *      couleur situes en face. Cela ne s'applique QUE depuis l'apex du
 *      tiers propre du pion (il faut traverser LE CENTRE, pas simplement
 *      etre a un angle interne).
 */

// Offsets des sextants dans la grille 12x12.
static const int OFFSET_X[6] = { 0, 0, 8, 8, 4, 4 };
static const int OFFSET_Y[6] = { 0, 4, 4, 8, 8, 0 };

/**
 * @brief Indique si le sextant appartient au tiers du joueur.
 *   BLANC : S0 + S5     ROUGE : S1 + S2     NOIR : S3 + S4
 */
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

    // ── 2. Captures diagonales (bishop direction) ────────────────────────
    //
    // Les deux diagonales "avant" ont une composante dans le sens d'avance
    // et une composante perpendiculaire (±1 sur l'autre axe).
    //
    // On utilise voisinDiagonal (reflexion aux frontieres) pour trouver
    // la case cible. Si la diagonale sort simultanement par les deux bords
    // du sextant (nxLocal>=4 ET nyLocal>=4), voisinDiagonal renvoie nullptr :
    // c'est le passage par le centre. Dans ce cas, et uniquement si le pion
    // est a l'apex de son PROPRE tiers, on propose les deux apexes adverses
    // (center-cross capture, comme le fou).
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
