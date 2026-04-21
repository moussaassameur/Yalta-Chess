#ifndef ETATPARTIE_HPP
#define ETATPARTIE_HPP

// Tous les états possibles d'une partie Yalta
enum class EtatPartie {
    EN_COURS,      // la partie continue normalement
    ECHEC,         // un roi est menacé mais peut encore bouger
    ECHEC_ET_MAT,  // un roi est bloqué -> ce joueur est éliminé
    PAT,           // le joueur ne peut plus bouger mais n'est pas en échec
    NULLE          // match nul (accord ou répétition)
};

#endif // ETATPARTIE_HPP
