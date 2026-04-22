#ifndef JOUEURHUMAIN_HPP
#define JOUEURHUMAIN_HPP

#include "joueur/Joueur.hpp"

// Joueur humain : le coup est transmis par le ControleurJeu après un clic
class JoueurHumain : public Joueur {
public:
    JoueurHumain(const std::string& nom, Couleur couleur);
    ~JoueurHumain() override = default;

    std::shared_ptr<Coup> jouerTour(ModeleJeu& modele) override;
};

#endif // JOUEURHUMAIN_HPP
