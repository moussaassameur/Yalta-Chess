#ifndef COULEUR_HPP
#define COULEUR_HPP

#include <string>

// Les 3 couleurs/joueurs du jeu Yalta
enum class Couleur { BLANC, NOIR, ROUGE };

// Conversion vers string (utile pour l'affichage et le debug)
inline std::string couleurToString(Couleur c) {
    switch (c) {
        case Couleur::BLANC: return "blanc";
        case Couleur::NOIR:  return "noir";
        case Couleur::ROUGE: return "rouge";
        default:             return "inconnu";
    }
}

#endif // COULEUR_HPP
