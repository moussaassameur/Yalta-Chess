#pragma once

#include "Piece.hpp"

namespace Yalta {

/**
 * @brief Représente la Reine dans le jeu Yalta
 */
class Reine : public Piece {

public:
    /**
     * @brief Constructeur
     * @param couleur La couleur de la Reine
     */
    Reine(Couleur couleur);

    /**
     * @brief Destructeur
     */
    ~Reine();

    /**
     * @brief Retourne les cases disponibles pour la Reine
     * @param plateau Le plateau de jeu
     * @return Liste des cases accessibles
     */
    std::vector<Case*> getDeplacements(
        std::vector<std::vector<Case*>>& plateau) override;
};

} 