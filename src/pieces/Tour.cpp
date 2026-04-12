#include "../../include/pieces/Tour.hpp"
#include "../../include/model/Case.hpp"

namespace Yalta {

// Constructeur
Tour::Tour(Couleur couleur) : Piece(couleur, "Tour") {
}

// Destructeur
Tour::~Tour() {
}

// Déplacements de la Tour
std::vector<Case*> Tour::getDeplacements(
    std::vector<std::vector<Case*>>& plateau) {

    std::vector<Case*> casesDisponibles;

    int ligne = caseActuelle->getLigne();
    int colonne = caseActuelle->getColonne();

    // 4 directions : haut, bas, gauche, droite
    int directions[4][2] = {
        {-1,  0},  // haut
        { 1,  0},  // bas
        { 0, -1},  // gauche
        { 0,  1}   // droite
    };

    // Pour chaque direction
    for (int i = 0; i < 4; i++) {
        int nouvLigne   = ligne   + directions[i][0];
        int nouvColonne = colonne + directions[i][1];

        // On avance dans la direction tant que c'est possible
        while (nouvLigne >= 0 && nouvLigne < 12 &&
               nouvColonne >= 0 && nouvColonne < 8) {

            Case* cible = plateau[nouvLigne][nouvColonne];

            // Case vide → on peut aller dessus et continuer
            if (!cible->estOccupee()) {
                casesDisponibles.push_back(cible);
            }
            // Case occupée par un adversaire → on capture et on arrête
            else if (cible->getPiece()->getCouleur() != couleur) {
                casesDisponibles.push_back(cible);
                break; // on s'arrête après la capture
            }
            // Case occupée par allié → on arrête
            else {
                break;
            }

            // On continue dans la même direction
            nouvLigne   += directions[i][0];
            nouvColonne += directions[i][1];
        }
    }

    return casesDisponibles;
}

} 