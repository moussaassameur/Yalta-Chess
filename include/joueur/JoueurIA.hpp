#ifndef JOUEUR_IA_HPP
#define JOUEUR_IA_HPP

#include "joueur/Joueur.hpp"
#include <memory>

class MinMax;

// Joueur IA : délègue le choix du coup à l'algorithme MinMax
class JoueurIA : public Joueur {
private:
    std::shared_ptr<MinMax> ia;

public:
    JoueurIA(const std::string& nom, Couleur couleur, int profondeur = 3);
    ~JoueurIA() override = default;

    std::shared_ptr<Coup> jouerTour(ModeleJeu& modele) override;
};

#endif // JOUEUR_IA_HPP
