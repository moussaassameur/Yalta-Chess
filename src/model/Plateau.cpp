#include "model/Plateau.hpp"
#include "pieces/Piece.hpp"
#include "pieces/Pion.hpp"
#include "pieces/Tour.hpp"
#include "pieces/Cavalier.hpp"
#include "pieces/Fou.hpp"
#include "pieces/Reine.hpp"
#include "pieces/Roi.hpp"

// Offsets des sextants dans la grille 12x12. Doit rester coherent avec
// la table 'blocs' de Plateau::creerCases() et avec Case.cpp.
static const int OFFSET_X[6] = { 0, 0, 8, 8, 4, 4 };
static const int OFFSET_Y[6] = { 0, 4, 4, 8, 8, 0 };

/**
 * @file Plateau.cpp
 * @brief Implementation du plateau Yalta -- creation des cases.
 *
 * La grille 12x12 est decoupee en 9 blocs 4x4 dont seulement 6 sont
 * occupes par un sextant valide. Les 3 autres blocs restent vides
 * (= trous : aucune entree dans cases[]).
 *
 *   Vue de la grille (axe x horizontal, axe y vertical) :
 *
 *      x=0..3   x=4..7   x=8..11
 *     +-------+--------+--------+
 *     |  S0   |  S5    |  TROU  |   y=0..3
 *     +-------+--------+--------+
 *     |  S1   |  TROU  |  S2    |   y=4..7
 *     +-------+--------+--------+
 *     | TROU  |  S4    |  S3    |   y=8..11
 *     +-------+--------+--------+
 *
 * La couleur damier (clair/fonce) alterne selon (x + y + sextant) modulo 2.
 * Le decalage par "sextant" assure que la transition damier reste correcte
 * lorsqu'on traverse une frontiere entre deux sextants adjacents.
 */

Plateau::Plateau() {
    creerCases();
}

std::shared_ptr<Case> Plateau::getCase(int x, int y) const {
    auto it = cases.find({x, y});
    return (it != cases.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Case>> Plateau::getToutesLesCases() const {
    std::vector<std::shared_ptr<Case>> v;
    v.reserve(cases.size());
    for (const auto& kv : cases) {
        v.push_back(kv.second);
    }
    return v;
}

int Plateau::nombreDeCases() const {
    return static_cast<int>(cases.size());
}

// ─── Placement initial des pieces (48 pieces) ──────────────────────────
//
// Convention de placement (notation Yalta) :
//   - back rank rang 1 :  Tour-Cav-Fou-Reine | Roi-Fou-Cav-Tour
//                         a1   b1  c1  d1   | e1  f1  g1  h1
//   - pions au rang 2 (a2..h2)
//
// Cote BLANC :
//   - S0 (premiere moitie a-d) : back rank yLocal=0 (xLocal=0..3)
//   - S5 (deuxieme moitie e-h) : back rank xLocal=0 (yLocal 3..0 -> e..h)
//
// Cote ROUGE / NOIR : meme structure, applique a leurs deux sextants.

namespace {

/// Pose une piece sur la case (x, y) en synchronisant Case <-> Piece.
void placer(const std::shared_ptr<Case>& cas,
            const std::shared_ptr<Piece>& p) {
    if (!cas || !p) return;
    cas->setPiece(p);
    p->setPosition(cas);
}

/**
 * @brief Pose les 16 pieces d'un joueur sur ses 2 sextants.
 *
 * @param plateau            Le plateau (lookup des cases).
 * @param couleur            Couleur du joueur a placer.
 * @param sextantA           Sextant "premiere moitie" (a-d).
 * @param sextantB           Sextant "deuxieme moitie" (e-h).
 *
 * sextantA est impair OU pair selon le joueur :
 *   - BLANC : sextantA=S0 (pair, back rank yLocal=0), sextantB=S5 (impair)
 *   - ROUGE : sextantA=S1 (impair, back rank xLocal=0), sextantB=S2 (pair)
 *   - NOIR  : sextantA=S3 (impair, back rank xLocal=0), sextantB=S4 (pair)
 */
void placerJoueur(Plateau& plateau,
                  Couleur couleur,
                  int sextantA, int sextantB,
                  bool inverserRoiReine = false) {
    static const int OFF_X[6] = { 0, 0, 8, 8, 4, 4 };
    static const int OFF_Y[6] = { 0, 4, 4, 8, 8, 0 };

    // Pour chaque sextant, on identifie l'axe du back rank et l'axe
    // d'avancement (perpendiculaire). Sextant pair : back rank yLocal=0,
    // pions yLocal=1, axe perpendiculaire = xLocal. Sextant impair :
    // back rank xLocal=0, pions xLocal=1, axe perpendiculaire = yLocal.
    auto poserSextant = [&](int sext, bool premiereMoitie) {
        const bool sextPair = (sext % 2 == 0);

        // Pour chaque "fichier" k = 0..3 dans le sextant :
        //   - premiere moitie a-d : ordre Tour, Cavalier, Fou, Reine
        //   - deuxieme moitie e-h : ordre Roi, Fou, Cavalier, Tour
        //     (decroissant pour matcher e=Roi, f=Fou, g=Cav, h=Tour quand
        //     on parcourt dans le sens du sextant)
        //
        // Note : "k" est l'index xLocal (sextant pair) ou yLocal (impair).
        for (int k = 0; k < 4; ++k) {
            // Coords du back rank et de la rangee des pions.
            int xBack, yBack, xPion, yPion;
            if (sextPair) {
                xBack = OFF_X[sext] + k;     yBack = OFF_Y[sext];
                xPion = OFF_X[sext] + k;     yPion = OFF_Y[sext] + 1;
            } else {
                xBack = OFF_X[sext];         yBack = OFF_Y[sext] + k;
                xPion = OFF_X[sext] + 1;     yPion = OFF_Y[sext] + k;
            }

            // Type de la piece du back rank selon (premiereMoitie, k).
            // Pour rouge et noir, Roi et Reine sont permutes (inverserRoiReine).
            std::shared_ptr<Piece> piece;
            if (premiereMoitie) {
                switch (k) {
                    case 0: piece = std::make_shared<Tour>(couleur);     break;
                    case 1: piece = std::make_shared<Cavalier>(couleur); break;
                    case 2: piece = std::make_shared<Fou>(couleur);      break;
                    case 3: piece = inverserRoiReine
                                    ? std::static_pointer_cast<Piece>(std::make_shared<Roi>(couleur))
                                    : std::static_pointer_cast<Piece>(std::make_shared<Reine>(couleur)); break;
                }
            } else {
                switch (k) {
                    case 0: piece = std::make_shared<Tour>(couleur);     break;
                    case 1: piece = std::make_shared<Cavalier>(couleur); break;
                    case 2: piece = std::make_shared<Fou>(couleur);      break;
                    case 3: piece = inverserRoiReine
                                    ? std::static_pointer_cast<Piece>(std::make_shared<Reine>(couleur))
                                    : std::static_pointer_cast<Piece>(std::make_shared<Roi>(couleur)); break;
                }
            }

            placer(plateau.getCase(xBack, yBack), piece);
            placer(plateau.getCase(xPion, yPion),
                   std::make_shared<Pion>(couleur));
        }
    };

    poserSextant(sextantA, /*premiereMoitie=*/true);
    poserSextant(sextantB, /*premiereMoitie=*/false);
}

}  // namespace

void Plateau::initialiserPiecesYalta() {
    // Si des pieces sont deja en place, on les enleve.
    for (auto& kv : cases) kv.second->retirerPiece();

    placerJoueur(*this, Couleur::BLANC, /*S0*/ 0, /*S5*/ 5, /*inverser=*/false);
    placerJoueur(*this, Couleur::ROUGE, /*S1*/ 1, /*S2*/ 2, /*inverser=*/true);
    placerJoueur(*this, Couleur::NOIR,  /*S3*/ 3, /*S4*/ 4, /*inverser=*/true);
}

std::vector<std::shared_ptr<Piece>>
Plateau::getPiecesDeCouleur(Couleur couleur) const {
    std::vector<std::shared_ptr<Piece>> res;
    for (const auto& kv : cases) {
        auto p = kv.second->getPiece();
        if (p && p->estVivante() && p->getCouleur() == couleur) {
            res.push_back(p);
        }
    }
    return res;
}

// ─── Topologie de bending Yalta ──────────────────────────────────────────
//
// Une piece glissante qui franchit une frontiere interne entre 2 sextants
// voit sa direction "courbee" pour suivre la geometrie hexagonale. La
// frontiere xLocal=3 du sextant i est partagee avec le sextant (i-1)%6 ;
// celle yLocal=3 avec le sextant (i+1)%6.
//
// Bending orthogonal (voisinAvecDir) : (dx, dy) -> (-dy, -dx).
// Bending diagonal (voisinDiagonal) : sortieX -> (dy,-dx), sortieY -> (-dy,dx).
// Ces rotations a 90 degres preservent l'orientation geometrique au passage
// de frontiere entre sextants adjacents.
//
// Le cas "sortie simultanee xLocal=3 ET yLocal=3" (passage exactement par
// le centre du plateau) n'est pas gere ici car il a 2 destinations
// possibles (les 2 sextants same-color). Pour les pieces qui en ont
// besoin (Fou, Reine, Roi), une logique de center-cross dediee sera
// ajoutee.

std::shared_ptr<Case>
Plateau::voisinAvecDir(std::shared_ptr<Case> depart, int& dx, int& dy) const {
    if (!depart) return nullptr;

    const int sext = depart->getSextant();
    const int xLocal = depart->getX() - OFFSET_X[sext];
    const int yLocal = depart->getY() - OFFSET_Y[sext];

    const int nxLocal = xLocal + dx;
    const int nyLocal = yLocal + dy;

    // Cas A : reste dans le meme sextant -- direction inchangee.
    if (nxLocal >= 0 && nxLocal < 4 && nyLocal >= 0 && nyLocal < 4) {
        return getCase(OFFSET_X[sext] + nxLocal,
                       OFFSET_Y[sext] + nyLocal);
    }

    // Cas B : sortie par un bord exterieur du plateau -- pas de voisin.
    if (nxLocal < 0 || nyLocal < 0) return nullptr;

    const bool sortieX = (nxLocal >= 4);
    const bool sortieY = (nyLocal >= 4);

    // Cas C : center-cross diagonal (pas gere ici, voir pieces Fou/Reine).
    if (sortieX && sortieY) return nullptr;

    // Cas D1 : franchissement xLocal=3 -> sextant (i-1)%6.
    if (sortieX) {
        const int sextSuiv = (sext + 5) % 6;  // = (i - 1 + 6) % 6
        const int newXLocal = nyLocal;        // axe parallele preserve
        const int newYLocal = 3;              // entree par le bord
        const int newDx = -dy;
        const int newDy = -dx;
        dx = newDx;
        dy = newDy;
        return getCase(OFFSET_X[sextSuiv] + newXLocal,
                       OFFSET_Y[sextSuiv] + newYLocal);
    }

    // Cas D2 : franchissement yLocal=3 -> sextant (i+1)%6.
    if (sortieY) {
        const int sextSuiv = (sext + 1) % 6;
        const int newXLocal = 3;
        const int newYLocal = nxLocal;
        const int newDx = -dy;
        const int newDy = -dx;
        dx = newDx;
        dy = newDy;
        return getCase(OFFSET_X[sextSuiv] + newXLocal,
                       OFFSET_Y[sextSuiv] + newYLocal);
    }

    return nullptr;
}

std::shared_ptr<Case>
Plateau::voisin(std::shared_ptr<Case> depart, int dx, int dy) const {
    int ldx = dx, ldy = dy;
    return voisinAvecDir(depart, ldx, ldy);
}

std::shared_ptr<Case>
Plateau::voisinDiagonal(std::shared_ptr<Case> depart, int& dx, int& dy) const {
    if (!depart) return nullptr;

    const int sext    = depart->getSextant();
    const int xLocal  = depart->getX() - OFFSET_X[sext];
    const int yLocal  = depart->getY() - OFFSET_Y[sext];

    const int nxLocal = xLocal + dx;
    const int nyLocal = yLocal + dy;

    // Reste dans le sextant.
    if (nxLocal >= 0 && nxLocal < 4 && nyLocal >= 0 && nyLocal < 4) {
        return getCase(OFFSET_X[sext] + nxLocal,
                       OFFSET_Y[sext] + nyLocal);
    }

    // Bord exterieur ou sortie negative.
    if (nxLocal < 0 || nyLocal < 0) return nullptr;

    const bool sortieX = (nxLocal >= 4);
    const bool sortieY = (nyLocal >= 4);

    // Passage par l'apex central (center-cross) -- gere dans Fou/Reine.
    if (sortieX && sortieY) return nullptr;

    // Sortie xLocal=3 -> sextant (i-1)%6. Rotation 90° : (dx,dy) -> (dy,-dx).
    if (sortieX) {
        const int sextSuiv = (sext + 5) % 6;
        const int newXLocal = nyLocal;
        const int newYLocal = 3;
        const int ndx = dy;
        const int ndy = -dx;
        dx = ndx; dy = ndy;
        return getCase(OFFSET_X[sextSuiv] + newXLocal,
                       OFFSET_Y[sextSuiv] + newYLocal);
    }

    // Sortie yLocal=3 -> sextant (i+1)%6. Rotation 90° : (dx,dy) -> (-dy,dx).
    if (sortieY) {
        const int sextSuiv = (sext + 1) % 6;
        const int newXLocal = 3;
        const int newYLocal = nxLocal;
        const int ndx = -dy;
        const int ndy = dx;
        dx = ndx; dy = ndy;
        return getCase(OFFSET_X[sextSuiv] + newXLocal,
                       OFFSET_Y[sextSuiv] + newYLocal);
    }

    return nullptr;
}

std::shared_ptr<Case> Plateau::trouverRoi(Couleur c) const {
    for (const auto& kv : cases) {
        auto p = kv.second->getPiece();
        if (p && p->estVivante() && p->getCouleur() == c && p->getType() == "Roi")
            return kv.second;
    }
    return nullptr;
}

bool Plateau::estEnEchec(Couleur c) const {
    auto caseRoi = trouverRoi(c);
    if (!caseRoi) return false;

    for (const auto& kv : cases) {
        auto p = kv.second->getPiece();
        if (!p || !p->estVivante() || p->getCouleur() == c) continue;
        for (const auto& dest : p->getDeplacements(*this)) {
            if (dest == caseRoi) return true;
        }
    }
    return false;
}

bool Plateau::estEnEchecPar(Couleur victime, Couleur attaquant) const {
    auto caseRoi = trouverRoi(victime);
    if (!caseRoi) return false;

    for (const auto& kv : cases) {
        auto p = kv.second->getPiece();
        if (!p || !p->estVivante() || p->getCouleur() != attaquant) continue;
        for (const auto& dest : p->getDeplacements(*this)) {
            if (dest == caseRoi) return true;
        }
    }
    return false;
}

void Plateau::creerCases() {
    // Description des 6 blocs valides : pour chaque sextant, son rectangle
    // dans la grille 12x12 et son numero. Doit rester synchronise avec
    // les tables OFFSET_X / OFFSET_Y de Case.cpp.
    struct Bloc {
        int x0;       ///< x minimal (inclus)
        int x1;       ///< x maximal (exclus)
        int y0;       ///< y minimal (inclus)
        int y1;       ///< y maximal (exclus)
        int sextant;  ///< numero du sextant (0..5)
    };

    const Bloc blocs[6] = {
        { 0,  4,  0,  4, 0 },  // S0 -- bas-gauche
        { 0,  4,  4,  8, 1 },  // S1 -- gauche
        { 8, 12,  4,  8, 2 },  // S2 -- haut-gauche
        { 8, 12,  8, 12, 3 },  // S3 -- haut-droite
        { 4,  8,  8, 12, 4 },  // S4 -- droite
        { 4,  8,  0,  4, 5 },  // S5 -- bas-droite
    };

    for (const Bloc& b : blocs) {
        for (int x = b.x0; x < b.x1; ++x) {
            for (int y = b.y0; y < b.y1; ++y) {
                const std::string couleurDamier =
                    ((x + y + b.sextant) % 2 == 0) ? "clair" : "fonce";

                cases[{x, y}] =
                    std::make_shared<Case>(x, y, b.sextant, couleurDamier);
            }
        }
    }
}
