#pragma once

#include "Piece.hpp"

namespace Yalta {

/**
 * @brief Représente la Tour dans le jeu Yalta
 */
class Tour : public Piece {

public:
    /**
     * @brief Constructeur
     * @param couleur La couleur de la Tour
     */
    Tour(Couleur couleur);

    /**
     * @brief Destructeur
     */
    ~Tour();

    /**
     * @brief Retourne les cases disponibles pour la Tour
     * @param plateau Le plateau de jeu
     * @return Liste des cases accessibles
     */
    std::vector<Case*> getDeplacements(
        std::vector<std::vector<Case*>>& plateau) override;
};

} 