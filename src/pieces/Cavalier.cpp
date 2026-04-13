#include "pieces/Cavalier.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Cavalier::Cavalier(const std::string& couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position) {
}

std::vector<std::shared_ptr<Case>> Cavalier::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (position == nullptr) return coups;

    // Les 12 sauts du Cavalier hexagonal
    // Sur un hexagone, un cavalier fait "2 cases dans une direction + 1 dans une direction
    // adjacente". Avec 6 directions principales, ça donne 12 combinaisons possibles.
    const int sauts[12][2] = {
        {+1, +2}, {+2, +1},
        {-1, -2}, {-2, -1},
        {+1, -3}, {+3, -1},   // Note : -3 et +3 viennent de la combinaison
        {-1, +3}, {-3, +1},   // de 2 directions hexagonales (pas une faute)
        {+2, -3}, {+3, -2},
        {-2, +3}, {-3, +2}
    };

    int q = position->getQ();
    int r = position->getR();

    // Le Cavalier SAUTE : il ne tient pas compte des pièces qu'il survole
    for (const auto& saut : sauts) {
        int nq = q + saut[0];
        int nr = r + saut[1];
        std::shared_ptr<Case> cible = plateau.getCase(nq, nr);
        if (cible == nullptr) continue;  // hors plateau

        // Case libre OU occupée par un ennemi → coup valide
        if (!cible->estOccupee() || cible->getPiece()->getCouleur() != couleur) {
            coups.push_back(cible);
        }
    }

    return coups;
}

std::string Cavalier::getType() const {
    return "Cavalier";
}