#include "../../include/pieces/Fou.hpp"
#include "../../include/model/Case.hpp"

namespace Yalta {

// Constructeur
Fou::Fou(Couleur couleur) : Piece(couleur, "Fou") {
}

// Destructeur
Fou::~Fou() {
}

// Déplacements du Fou
std::vector<Case*> Fou::getDeplacements(
    std::vector<std::vector<Case*>>& plateau) {

    std::vector<Case*> casesDisponibles;

    int ligne = caseActuelle->getLigne();
    int colonne = caseActuelle->getColonne();

    // 4 directions diagonales
    int directions[4][2] = {
        {-1, -1},  // haut gauche
        {-1,  1},  // haut droite
        { 1, -1},  // bas gauche
        { 1,  1}   // bas droite
    };

    // Pour chaque direction diagonale
    for (int i = 0; i < 4; i++) {
        int nouvLigne   = ligne   + directions[i][0];
        int nouvColonne = colonne + directions[i][1];

        // On avance en diagonale tant que possible
        while (nouvLigne >= 0 && nouvLigne < 12 &&
               nouvColonne >= 0 && nouvColonne < 8) {

            Case* cible = plateau[nouvLigne][nouvColonne];

            // Vérifier que la case a la même couleur
            if (cible->getCouleur() != caseActuelle->getCouleur()) {
                break; // le Fou reste sur sa couleur
            }

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