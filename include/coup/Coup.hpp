#ifndef COUP_HPP
#define COUP_HPP

#include <string>

class Plateau;

/**
 * @file Coup.hpp
 * @brief Interface du pattern Command : un coup executable et annulable.
 */
class Coup {
public:
    virtual ~Coup() = default;

    // Applique le coup sur le plateau.
    virtual void executer(Plateau& plateau) = 0;

    // Annule le coup et restaure l'etat precedent du plateau.
    virtual void annuler(Plateau& plateau) = 0;

    // Verifie que le coup est jouable sur le plateau actuel.
    virtual bool estValide(const Plateau& plateau) const = 0;
};

#endif // COUP_HPP
