#ifndef JOUEUR_HUMAIN_HPP
#define JOUEUR_HUMAIN_HPP

#include "joueur/Joueur.hpp"

/**
 * @file JoueurHumain.hpp
 * @brief Joueur humain -- ses coups sont selectionnes via l'interface
 *        graphique (clic), pas par une logique interne.
 *
 * En Phase 5, cette classe est essentiellement un stub : un Joueur
 * concret avec son nom et sa couleur. La logique de selection des coups
 * via clic souris sera ajoutee dans le ControleurJeu en Phase 6.
 */
class JoueurHumain : public Joueur {
public:
    JoueurHumain(const std::string& nom, Couleur couleur);
    ~JoueurHumain() override = default;

    /// Toujours nullptr : le joueur humain choisit son coup via l'UI
    /// (clic souris), pas via cette methode.
    std::shared_ptr<Coup> jouerTour(Plateau& plateau) override;
};

#endif // JOUEUR_HUMAIN_HPP
