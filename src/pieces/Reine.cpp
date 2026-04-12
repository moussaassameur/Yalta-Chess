#include "../../include/pieces/Reine.hpp"
#include "../../include/model/Case.hpp"

namespace Yalta {

// Constructeur
Reine::Reine(Couleur couleur) : Piece(couleur, "Reine") {
}

// Destructeur
Reine::~Reine() {
}

// Déplacements de la Reine
std::vector<Case*> Reine::getDeplacements(
    std::vector<std::vector<Case*>>& plateau) {

    std::vector<Case*> casesDisponibles;

    int ligne = caseActuelle->getLigne();
    int colonne = caseActuelle->getColonne();

    // 8 directions : 4 droites + 4 diagonales
    int directions[8][2] = {
        {-1,  0},  // haut
        { 1,  0},  // bas
        { 0, -1},  // gauche
        { 0,  1},  // droite
        {-1, -1},  // haut gauche
        {-1,  1},  // haut droite
        { 1, -1},  // bas gauche
        { 1,  1}   // bas droite
    };

    // Pour chaque direction
    for (int i = 0; i < 8; i++) {
        int nouvLigne   = ligne   + directions[i][0];
        int nouvColonne = colonne + directions[i][1];

        // On avance tant que possible
        while (nouvLigne >= 0 && nouvLigne < 12 &&
               nouvColonne >= 0 && nouvColonne < 8) {

            Case* cible = plateau[nouvLigne][nouvColonne];

            // Case vide → on peut aller dessus et continuer
            if (!cible->estOccupee()) {
                casesDisponibles.push_back(cible);
            }
            // Case adversaire → on capture et on arrête
            else if (cible->getPiece()->getCouleur() != couleur) {
                casesDisponibles.push_back(cible);
                break;
            }
            // Case alliée → on arrête
            else {
                break;
            }

            nouvLigne   += directions[i][0];
            nouvColonne += directions[i][1];
        }
    }

    return casesDisponibles;
}

} 