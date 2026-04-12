#pragma once

#include <vector>

namespace Yalta {

class Case;
class Piece;

/**
 * @brief Interface du pattern Strategy pour les déplacements
 */
class StrategieDeplacement {

public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~StrategieDeplacement() {}

    /**
     * @brief Calcule les déplacements possibles
     * @param piece La pièce qui se déplace
     * @param plateau Le plateau de jeu
     * @return Liste des cases accessibles
     */
    virtual std::vector<Case*> calculerDeplacements(
        Piece* piece,
        std::vector<std::vector<Case*>>& plateau) = 0;
};

} 