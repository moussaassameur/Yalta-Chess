#include "pieces/Cavalier.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

#include <set>
#include <utility>
#include <vector>

/**
 * @file Cavalier.cpp
 * @brief Implementation du saut en L (avec bending Yalta + dedup).
 */

// Offsets des sextants dans la grille 12x12 -- coherent avec Plateau/Case.
static const int OFFSET_X[6] = { 0, 0, 8, 8, 4, 4 };
static const int OFFSET_Y[6] = { 0, 4, 4, 8, 8, 0 };

Cavalier::Cavalier(Couleur couleur) : Piece(couleur) {}

std::string Cavalier::getType() const { return "Cavalier"; }

std::vector<std::shared_ptr<Case>>
Cavalier::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (!position) return coups;

    // Un saut de cavalier se decompose en : 1 pas orthogonal + 1 pas
    // diagonal "vers l'avant" (la diagonale prolonge la direction
    // orthogonale). Par exemple le saut (2,1) = orthogonal (1,0) suivi
    // de la diagonale (1,1).
    //
    // Cette construction donne exactement les 8 sauts en L, et suit
    // correctement le bending Yalta car elle utilise voisinAvecDir pour
    // le pas orthogonal et voisinDiagonal pour le pas diagonal -- les
    // memes primitives que la Tour et le Fou.

    static const int ortho[4][2] = { {+1, 0}, {-1, 0}, {0, +1}, {0, -1} };

    std::set<std::pair<int, int>> destinations;

    for (const auto& o : ortho) {
        int dx = o[0], dy = o[1];
        auto M = plateau.voisinAvecDir(position, dx, dy);
        if (!M) continue;
        // Apres voisinAvecDir, (dx, dy) contient la direction orthogonale
        // eventuellement "courbee" par le bending. Les 2 diagonales
        // "avant" = cette direction + une composante perpendiculaire.
        const int perps[2][2] = { { dy, dx }, { -dy, -dx } };
        for (const auto& pp : perps) {
            int ddx = dx + pp[0];
            int ddy = dy + pp[1];
            auto D = plateau.voisinDiagonal(M, ddx, ddy);
            if (D) destinations.insert({D->getX(), D->getY()});
        }
    }

    // Cas special : Cavalier sur un sommet (3,3). Le sommet est la
    // singularite centrale du plateau ou 6 sextants se rejoignent. Un
    // saut de cavalier depuis ce point traverse le centre et atteint
    // aussi les 2 cases voisines (orthogonales) du sommet de chacun des
    // 2 sextants "lointains" (i+2)%6 et (i+4)%6. Ces sauts ne sont pas
    // representables par la decomposition pas-a-pas classique ci-dessus.
    {
        const int s  = position->getSextant();
        const int xL = position->getX() - OFFSET_X[s];
        const int yL = position->getY() - OFFSET_Y[s];
        if (xL == 3 && yL == 3) {
            for (int delta : {2, 4}) {
                const int sd = (s + delta) % 6;
                const int ax = OFFSET_X[sd] + 3;  // sommet du sextant lointain
                const int ay = OFFSET_Y[sd] + 3;
                for (const auto& v : { plateau.getCase(ax - 1, ay),
                                       plateau.getCase(ax, ay - 1) }) {
                    if (v) destinations.insert({v->getX(), v->getY()});
                }
            }
        }
    }

    // Securite : un saut de cavalier n'atterrit jamais sur le depart ni
    // sur une case adjacente.
    const std::pair<int, int> posKey{position->getX(), position->getY()};
    std::set<std::pair<int, int>> voisinsDepart;
    static const int dirs8[8][2] = {
        {+1,  0}, {-1,  0}, { 0, +1}, { 0, -1},
        {+1, +1}, {+1, -1}, {-1, +1}, {-1, -1}
    };
    for (const auto& d : dirs8) {
        auto v = plateau.voisin(position, d[0], d[1]);
        if (v) voisinsDepart.insert({v->getX(), v->getY()});
    }

    // Filtrage final : pas le depart, pas une case adjacente, pas de
    // capture d'une piece amie.
    for (const auto& key : destinations) {
        if (key == posKey)            continue;
        if (voisinsDepart.count(key)) continue;
        auto c = plateau.getCase(key.first, key.second);
        if (!c) continue;
        if (c->estOccupee()
            && c->getPiece()->getCouleur() == couleur) continue;
        coups.push_back(c);
    }

    return coups;
}
