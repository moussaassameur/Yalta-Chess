#ifndef COULEUR_HPP
#define COULEUR_HPP

#include <string>

/**
 * @file Couleur.hpp
 * @brief Couleurs des trois joueurs du jeu Yalta.
 *
 * Yalta se joue a 3 -- chaque joueur a sa propre couleur de pieces et son
 * propre tiers du plateau (deux sextants adjacents).
 */

/// Couleur d'un joueur ou d'une piece.
enum class Couleur {
    BLANC,
    ROUGE,
    NOIR
};

/**
 * @brief Convertit une couleur en chaine lisible (utile pour le debug et
 *        l'affichage).
 * @param c Couleur a convertir.
 * @return "blanc", "rouge", "noir" ou "?".
 */
inline std::string couleurToString(Couleur c) {
    switch (c) {
        case Couleur::BLANC: return "blanc";
        case Couleur::ROUGE: return "rouge";
        case Couleur::NOIR:  return "noir";
    }
    return "?";
}

#endif // COULEUR_HPP
