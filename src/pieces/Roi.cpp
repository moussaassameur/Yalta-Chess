#include "../../include/pieces/Roi.hpp"
#include "../../include/model/Case.hpp"

namespace Yalta {

// Constructeur
Roi::Roi(Couleur couleur) : Piece(couleur, "Roi") {
}

// Destructeur
Roi::~Roi() {
}

// Déplacements du Roi
std::vector<Case*> Roi::getDeplacements(
    std::vector<std::vector<Case*>>& plateau) {

    std::vector<Case*> casesDisponibles;

    // Position actuelle du Roi
    int ligne = caseActuelle->getLigne();
    int colonne = caseActuelle->getColonne();

    // Toutes les directions possibles pour le Roi
    // {ligne, colonne}
    int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},  // haut gauche, haut, haut droite
        { 0, -1},           { 0, 1},  // gauche,           droite
        { 1, -1}, { 1, 0}, { 1, 1}   // bas gauche,  bas,  bas droite
    };

    // On vérifie chaque direction
    for (int i = 0; i < 8; i++) {
        int nouvLigne   = ligne   + directions[i][0];
        int nouvColonne = colonne + directions[i][1];

        // Vérifier que la case est dans le plateau
        if (nouvLigne >= 0 && nouvLigne < 12 &&
            nouvColonne >= 0 && nouvColonne < 8) {

            Case* cible = plateau[nouvLigne][nouvColonne];

            // Case vide → on peut aller dessus
            if (!cible->estOccupee()) {
                casesDisponibles.push_back(cible);
            }
            // Case occupée par un adversaire → on peut capturer
            else if (cible->getPiece()->getCouleur() != couleur) {
                casesDisponibles.push_back(cible);
            }
            // Case occupée par une pièce alliée → interdit
        }
    }

    return casesDisponibles;
}

} 