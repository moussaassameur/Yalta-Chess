#ifndef OBSERVATEUR_HPP
#define OBSERVATEUR_HPP

class ModeleJeu;

/**
 * @file Observateur.hpp
 * @brief Interface du pattern Observer : reagit aux changements du modele.
 */
class Observateur {
public:
    virtual ~Observateur() = default;

    // Appelee par ModeleJeu apres chaque changement d'etat.
    virtual void mettreAJour(const ModeleJeu& modele) = 0;
};

#endif // OBSERVATEUR_HPP
