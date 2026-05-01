#ifndef ETAT_PARTIE_HPP
#define ETAT_PARTIE_HPP

#include <string>

/**
 * @file EtatPartie.hpp
 * @brief Etats successifs possibles d'une partie de Yalta.
 *
 *   EN_COURS     : la partie continue, aucune condition d'arret atteinte.
 *   ECHEC        : le roi du joueur courant est mis en echec, mais peut
 *                  encore se sauver.
 *   ECHEC_ET_MAT : le roi du joueur courant est mat -- le joueur est
 *                  elimine. Si un seul joueur reste vivant, la partie
 *                  est gagnee par celui-ci.
 *   PAT          : pat (le joueur courant n'a aucun coup legal mais
 *                  n'est pas en echec). Selon les conventions, partie
 *                  nulle ou joueur elimine.
 *   NULLE        : nulle pour une autre raison (regle des 50 coups,
 *                  triple repetition...). Sera utilise plus tard.
 */
enum class EtatPartie {
    EN_COURS,
    ECHEC,
    ECHEC_ET_MAT,
    PAT,
    NULLE
};

inline std::string etatPartieToString(EtatPartie e) {
    switch (e) {
        case EtatPartie::EN_COURS:     return "EN_COURS";
        case EtatPartie::ECHEC:        return "ECHEC";
        case EtatPartie::ECHEC_ET_MAT: return "ECHEC_ET_MAT";
        case EtatPartie::PAT:          return "PAT";
        case EtatPartie::NULLE:        return "NULLE";
    }
    return "?";
}

#endif // ETAT_PARTIE_HPP
