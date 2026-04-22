#ifndef JOUEUR_HPP
#define JOUEUR_HPP

#include "model/Couleur.hpp"
#include <string>
#include <memory>

class Coup;
class ModeleJeu;

// Classe abstraite — joueur humain ou IA
class Joueur {
protected:
    std::string nom;
    Couleur     couleur;
    bool        estElimine;

public:
    Joueur(const std::string& nom, Couleur couleur);
    virtual ~Joueur() = default;

    virtual std::shared_ptr<Coup> jouerTour(ModeleJeu& modele) = 0;

    void        eliminer();
    std::string getNom()        const;
    Couleur     getCouleur()    const;
    bool        getEstElimine() const;
};

#endif // JOUEUR_HPP
