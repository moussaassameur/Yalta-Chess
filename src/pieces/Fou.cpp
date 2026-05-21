#include "pieces/Fou.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

/**
 * @file Fou.cpp
 * @brief Implementation des deplacements du Fou (diagonale + center-cross).
 */

// Offsets des sextants  doit rester coherent avec Plateau / Case.
static const int OFFSET_X[6] = { 0, 0, 8, 8, 4, 4 };
static const int OFFSET_Y[6] = { 0, 4, 4, 8, 8, 0 };

Fou::Fou(Couleur couleur) : Piece(couleur) {}

std::string Fou::getType() const { return "Fou"; }

/// Indique si la case donnee est la case centrale (xLocal=3, yLocal=3)
/// de son sextant.
static bool estCentreSextant(const std::shared_ptr<Case>& c) {
    if (!c) return false;
    const int s = c->getSextant();
    return (c->getX() - OFFSET_X[s] == 3
         && c->getY() - OFFSET_Y[s] == 3);
}

std::vector<std::shared_ptr<Case>>
Fou::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (!position) return coups;

    static const int dirs[4][2] = {
        {+1, +1}, {+1, -1},
        {-1, +1}, {-1, -1}
    };

    // Helper : tente d'ajouter une case a la liste des destinations.
    // Retourne true si la glissade peut continuer (case vide), false
    // sinon (case occupee par ennemi capturable, ou amie bloquante,
    // ou nullptr).
    auto ajouterSiPossible = [&](const std::shared_ptr<Case>& cible) -> bool {
        if (!cible) return false;
        if (!cible->estOccupee()) {
            coups.push_back(cible);
            return true;
        }
        if (cible->getPiece()->getCouleur() != couleur) {
            coups.push_back(cible);  // capture
        }
        return false;  // case occupee = on s'arrete
    };

    // Depuis l'apex (3,3) du sextant i, le fou peut traverser le centre
    // vers les apexes (3,3) des sextants same-color (i+2)%6 et (i+4)%6,
    // puis continuer a glisser depuis chaque apex en direction (-1,-1)
    // jusqu'a etre bloque. C'est la regle "choose which path to follow".
    auto proposerCenterCross = [&](const std::shared_ptr<Case>& centre) {
        if (!centre) return;
        const int sext = centre->getSextant();
        for (int delta : {2, 4}) {
            const int sDest = (sext + delta) % 6;
            auto apex = plateau.getCase(OFFSET_X[sDest] + 3,
                                        OFFSET_Y[sDest] + 3);
            if (!ajouterSiPossible(apex)) continue;
            // Continue depuis l'apex en (-1,-1) : c'est le sens qui
            // s'eloigne du centre dans le sextant de destination.
            auto courant = apex;
            int sdx = -1, sdy = -1;
            while (true) {
                auto suiv = plateau.voisinDiagonal(courant, sdx, sdy);
                if (!ajouterSiPossible(suiv)) break;
                courant = suiv;
            }
        }
    };

    //  Glissade diagonale dans les 4 directions 
    for (const auto& d : dirs) {
        int dx = d[0];
        int dy = d[1];
        std::shared_ptr<Case> courante = position;

        while (true) {
            // Le Fou utilise voisinDiagonal (reflexion aux frontieres)
            // et non voisinAvecDir (rotation) -- voir Plateau.hpp.
            auto suivante = plateau.voisinDiagonal(courante, dx, dy);
            if (!ajouterSiPossible(suivante)) break;
            courante = suivante;

            // La glissade arrive sur un sommet (3, 3). Deux cas :
            //   - la direction locale pointe vers le centre mort (+1, +1) :
            //     elle ne peut pas continuer -> on offre le center-cross
            //     (les 2 diagonales same-color offertes au centre).
            //   - sinon : la glissade ne fait que TRAVERSER le sommet ;
            //     le bending de voisinDiagonal la prolonge normalement,
            //     donc on ne declenche surtout pas de center-cross ici.
            if (estCentreSextant(courante) && dx == 1 && dy == 1) {
                proposerCenterCross(courante);
                break;
            }
        }
    }

    //  Center-cross direct si le Fou est deja sur (3, 3) 
    if (estCentreSextant(position)) {
        proposerCenterCross(position);
    }

    return coups;
}
