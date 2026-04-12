#pragma once

#include "Piece.hpp"

namespace Yalta {

/**
 * @brief Représente le Fou dans le jeu Yalta
 */
class Fou : public Piece {

public:
    /**
     * @brief Constructeur
     * @param couleur La couleur du Fou
     */
    Fou(Couleur couleur);

    /**
     * @brief Destructeur
     */
    ~Fou();

    /**
     * @brief Retourne les cases disponibles pour le Fou
     * @param plateau Le plateau de jeu
     * @return Liste des cases accessibles
     */
    std::vector<Case*> getDeplacements(
        std::vector<std::vector<Case*>>& plateau) override;
};

} 