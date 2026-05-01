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

Cavalier::Cavalier(Couleur couleur) : Piece(couleur) {}

std::string Cavalier::getType() const { return "Cavalier"; }

std::vector<std::shared_ptr<Case>>
Cavalier::getDeplacements(const Plateau& plateau) const {
    std::vector<std::shared_ptr<Case>> coups;
    if (!position) return coups;

    // Helper : suit une sequence de pas unitaires en mettant a jour la
    // direction selon le bending. Retourne nullptr si on sort du plateau.
    auto suivreSequence = [&](const std::vector<std::pair<int, int>>& pas)
        -> std::shared_ptr<Case> {
        std::shared_ptr<Case> courante = position;
        for (const auto& p : pas) {
            int dx = p.first;
            int dy = p.second;
            auto suiv = plateau.voisinAvecDir(courante, dx, dy);
            if (!suiv) return nullptr;
            courante = suiv;
        }
        return courante;
    };

    // Set des coordonnees (x, y) pour deduplication.
    std::set<std::pair<int, int>> destinations;

    const int signes[2] = { +1, -1 };
    for (int sx : signes) {
        for (int sy : signes) {
            // 4 sequences possibles pour chaque combinaison de signes.
            const std::vector<std::vector<std::pair<int, int>>> chemins = {
                { {sx, 0}, {0,  sy}, {0,  sy} },  // 1+2 axe x d'abord
                { {0,  sy}, {sx, 0}, {sx, 0} },   // 1+2 axe y d'abord
                { {sx, 0}, {sx, 0}, {0,  sy} },   // 2+1 axe x d'abord
                { {0,  sy}, {0,  sy}, {sx, 0} },  // 2+1 axe y d'abord
            };
            for (const auto& chemin : chemins) {
                auto dest = suivreSequence(chemin);
                if (!dest) continue;
                destinations.insert({dest->getX(), dest->getY()});
            }
        }
    }

    // Filtrage : pas la case de depart, pas une piece amie sur l'arrivee.
    for (const auto& key : destinations) {
        auto c = plateau.getCase(key.first, key.second);
        if (!c) continue;
        if (c == position) continue;  // securite, ne devrait pas arriver
        if (c->estOccupee()
            && c->getPiece()->getCouleur() == couleur) continue;
        coups.push_back(c);
    }

    return coups;
}
