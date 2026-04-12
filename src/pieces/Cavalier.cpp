#include "../../include/pieces/Cavalier.hpp"
#include "../../include/model/Case.hpp"

namespace Yalta {

// Constructeur
Cavalier::Cavalier(Couleur couleur) : Piece(couleur, "Cavalier") {
}

// Destructeur
Cavalier::~Cavalier() {
}

// Déplacements du Cavalier
std::vector<Case*> Cavalier::getDeplacements(
    std::vector<std::vector<Case*>>& plateau) {

    std::vector<Case*> casesDisponibles;

    int ligne = caseActuelle->getLigne();
    int colonne = caseActuelle->getColonne();

    // 8 mouvements possibles en L
    int mouvements[8][2] = {
        {-2, -1}, {-2,  1},  // 2 haut + 1 gauche/droite
        { 2, -1}, { 2,  1},  // 2 bas  + 1 gauche/droite
        {-1, -2}, {-1,  2},  // 1 haut + 2 gauche/droite
        { 1, -2}, { 1,  2}   // 1 bas  + 2 gauche/droite
    };

    // Pour chaque mouvement possible
    for (int i = 0; i < 8; i++) {
        int nouvLigne   = ligne   + mouvements[i][0];
        int nouvColonne = colonne + mouvements[i][1];

        // Vérifier que la case est dans le plateau
        if (nouvLigne >= 0 && nouvLigne < 12 &&
            nouvColonne >= 0 && nouvColonne < 8) {

            Case* cible = plateau[nouvLigne][nouvColonne];

            // Case vide → on peut aller dessus
            if (!cible->estOccupee()) {
                casesDisponibles.push_back(cible);
            }
            // Case adversaire → on peut capturer
            else if (cible->getPiece()->getCouleur() != couleur) {
                casesDisponibles.push_back(cible);
            }
            // Case alliée → interdit
        }
    }

    return casesDisponibles;
}

} 