#include "pieces/Tour.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"

Tour::Tour(Couleur couleur, std::shared_ptr<Case> position)
    : Piece(couleur, position), aDejaBouge(false) {
}

std::vector<std::shared_ptr<Case>> Tour::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (position == nullptr) return coups;

    // Les 6 directions "Tour" hexagonales
    const int directions[6][2] = {
        {+1,  0}, {-1,  0}, { 0, +1}, { 0, -1}, {+1, -1}, {-1, +1}
    };

    int q = position->getQ();
    int r = position->getR();

    // Pour chaque direction, on glisse jusqu'à un obstacle ou le bord
    for (const auto& dir : directions) {
        int nq = q;
        int nr = r;
        while (true) {
            nq += dir[0];
            nr += dir[1];
            std::shared_ptr<Case> cible = plateau.getCase(nq, nr);
            if (cible == nullptr) break;  // hors plateau

            if (!cible->estOccupee()) {
                // Case vide → on continue
                coups.push_back(cible);
            } else {
                // Case occupée
                if (cible->getPiece()->getCouleur() != couleur) {
                    coups.push_back(cible);  // capture possible
                }
                break;  // dans tous les cas, on s'arrête
            }
        }
    }

    return coups;
}

std::string Tour::getType() const { return "Tour"; }

bool Tour::getADejaBouge() const { return aDejaBouge; }
void Tour::setADejaBouge(bool valeur) { aDejaBouge = valeur; }