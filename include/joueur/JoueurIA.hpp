#ifndef JOUEUR_IA_HPP
#define JOUEUR_IA_HPP

#include "joueur/Joueur.hpp"
#include "ia/MinMax.hpp"

/**
 * @file JoueurIA.hpp
 * @brief Joueur controle par l'IA -- delegue le choix du coup a un MinMax.
 *
 * Contrairement a JoueurHumain qui attend l'input UI, JoueurIA calcule
 * son coup automatiquement via getMeilleurCoup() de son MinMax interne.
 */
class JoueurIA : public Joueur {
public:
    /**
     * @param nom        Libelle affiche du joueur.
     * @param couleur    Couleur Yalta (BLANC, ROUGE ou NOIR).
     * @param profondeur Profondeur de recherche du MinMax (>= 1).
     * @param nbThreads  Nombre de threads pour la parallelisation (>= 1).
     */
    JoueurIA(const std::string& nom, Couleur couleur,
             int profondeur = 3, int nbThreads = 4);

    ~JoueurIA() override = default;

    /// Retourne le meilleur coup calcule par le MinMax interne.
    std::shared_ptr<Coup> jouerTour(Plateau& plateau) override;

    /// Accesseur pour configurer l'IA (utilise par les tests / l'UI).
    const MinMax& getIA() const;

private:
    MinMax ia;
};

#endif // JOUEUR_IA_HPP
