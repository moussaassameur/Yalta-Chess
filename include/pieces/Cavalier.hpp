#pragma once

#include "Piece.hpp"

namespace Yalta {

/**
 * @brief Représente le Cavalier dans le jeu Yalta
 */
class Cavalier : public Piece {

public:
    /**
     * @brief Constructeur
     * @param couleur La couleur du Cavalier
     */
    Cavalier(Couleur couleur);

    /**
     * @brief Destructeur
     */
    ~Cavalier();

    /**
     * @brief Retourne les cases disponibles pour le Cavalier
     * @param plateau Le plateau de jeu
     * @return Liste des cases accessibles
     */
    std::vector<Case*> getDeplacements(
        std::vector<std::vector<Case*>>& plateau) override;
};

} 