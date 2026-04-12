#include "pieces/Roi.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Roi::Roi(const std::string& couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position), aDejaBouge(false) {
}

std::vector<std::shared_ptr<Case>> Roi::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (position == nullptr) return coups;

    // Les 6 directions hexagonales (Tour) + 6 diagonales (Fou) = 12 directions
    // Le Roi se déplace d'1 case dans chacune
    const int directions[12][2] = {
        // Directions "Tour" (lignes droites hexagonales)
        {+1,  0}, {-1,  0}, { 0, +1}, { 0, -1}, {+1, -1}, {-1, +1},
        // Directions "Fou" (diagonales hexagonales)
        {+1, +1}, {-1, -1}, {+2, -1}, {-2, +1}, {+1, -2}, {-1, +2}
    };

    int q = position->getQ();
    int r = position->getR();

    for (const auto& dir : directions) {
        int nq = q + dir[0];
        int nr = r + dir[1];
        std::shared_ptr<Case> cible = plateau.getCase(nq, nr);
        if (cible == nullptr) continue;  // hors plateau

        // Case libre ou occupée par un ennemi → coup valide
        if (!cible->estOccupee() || cible->getPiece()->getCouleur() != couleur) {
            coups.push_back(cible);
        }
    }

    return coups;
}

std::string Roi::getType() const { return "Roi"; }

bool Roi::getADejaBouge() const { return aDejaBouge; }
void Roi::setADejaBouge(bool valeur) { aDejaBouge = valeur; }