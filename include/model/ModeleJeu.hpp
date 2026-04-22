#ifndef MODELEJEU_HPP
#define MODELEJEU_HPP

#include "model/EtatPartie.hpp"
#include <vector>
#include <memory>

class Plateau;
class Joueur;
class Coup;
class Historique;
class Observateur;

// Classe centrale du jeu — Modèle (MVC), Sujet (Observer), Invoker (Command)
class ModeleJeu {
private:
    std::vector<std::shared_ptr<Joueur>>      joueurs;
    std::shared_ptr<Joueur>                   joueurActuel;
    std::shared_ptr<Plateau>                  plateau;
    EtatPartie                                etat;
    std::shared_ptr<Historique>               historique;
    std::vector<std::shared_ptr<Observateur>> observateurs;

public:
    ModeleJeu();
    ~ModeleJeu() = default;

    // Cycle de jeu
    void demarrer();
    void jouerCoup(std::shared_ptr<Coup> coup);
    void tourSuivant();
    EtatPartie verifierEtat();
    void annulerDernierCoup();

    // Pattern Observer
    void ajouterObservateur(std::shared_ptr<Observateur> o);
    void retirerObservateur(std::shared_ptr<Observateur> o);
    void notifierObservateurs();

    // Getters
    EtatPartie                    getEtat()          const;
    std::shared_ptr<Joueur>       getJoueurActuel()  const;
    std::shared_ptr<Plateau>      getPlateau()       const;
    std::shared_ptr<Historique>   getHistorique()    const;
};

#endif // MODELEJEU_HPP
