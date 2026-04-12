#pragma once

#include "Piece.hpp"

namespace Yalta {

/**
 * @brief Représente le Roi dans le jeu Yalta
 */
class Roi : public Piece {

public:
    /**
     * @brief Constructeur
     * @param couleur La couleur du Roi
     */
    Roi(Couleur couleur);

    /**
     * @brief Destructeur
     */
    ~Roi();

    /**
     * @brief Retourne les cases disponibles pour le Roi
     * @param plateau Le plateau de jeu
     * @return Liste des cases accessibles
     */
    std::vector<Case*> getDeplacements(
        std::vector<std::vector<Case*>>& plateau) override;
};

} 