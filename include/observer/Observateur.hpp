#ifndef OBSERVATEUR_HPP
#define OBSERVATEUR_HPP

class ModeleJeu;

/**
 * @file Observateur.hpp
 * @brief Interface du pattern Observer.
 *
 * Toute classe qui souhaite reagir aux changements d'etat du modele de
 * jeu (Vue, Alerte d'echec, Logger, ...) implemente cette interface et
 * s'enregistre via ModeleJeu::attacher().
 *
 * La methode mettreAJour est appelee par ModeleJeu apres chaque action
 * notable (coup joue, tour suivant, etat de partie modifie). Elle recoit
 * le modele en lecture seule, l'observateur est libre d'aller y lire les
 * informations qui l'interessent.
 */
class Observateur {
public:
    virtual ~Observateur() = default;

    /**
     * @brief Methode appelee par le modele apres un changement d'etat.
     * @param modele Reference constante au modele qui a notifie.
     */
    virtual void mettreAJour(const ModeleJeu& modele) = 0;
};

#endif // OBSERVATEUR_HPP
