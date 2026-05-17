#ifndef JOUEUR_HPP
#define JOUEUR_HPP

#include "model/Couleur.hpp"
#include <memory>
#include <string>

class Coup;
class Plateau;

/**
 * @file Joueur.hpp
 * @brief Classe de base abstraite pour tous les joueurs de Yalta.
 *
 * Un joueur a un nom, une couleur (BLANC, ROUGE ou NOIR) et un etat
 * "elimine" qui passe a true lorsque son roi est mat.
 *
 * Les sous-classes (JoueurHumain, JoueurIA en Phase 8) determinent la
 * maniere dont le joueur choisit son coup. Pour Phase 5, on fournit un
 * JoueurHumain stub qui n'implemente pas encore de logique de selection
 * (ce sera fait en Phase 6 avec le ControleurJeu).
 */
class Joueur {
public:
    Joueur(const std::string& nom, Couleur couleur);
    virtual ~Joueur() = default;

    /// @return Nom du joueur.
    std::string getNom() const;

    /// @return Couleur du joueur.
    Couleur getCouleur() const;

    /// @return true si le joueur a ete elimine (echec et mat).
    bool getEstElimine() const;

    /// @brief Marque le joueur comme elimine.
    void eliminer();

    /// @return Score du joueur (0, 0.5 ou 1).
    double getScore() const;

    /// @brief Ajoute des points au score du joueur.
    void ajouterScore(double s);

    /**
     * @brief Demande au joueur de choisir son coup pour le tour courant.
     * @param plateau Plateau actuel (passe en non-const car les sous-classes
     *                comme JoueurIA peuvent l'utiliser pour simuler/annuler
     *                des coups pendant l'analyse).
     * @return Le coup choisi, ou nullptr si le joueur attend une entree
     *         externe (cas du JoueurHumain qui joue via clic UI).
     */
    virtual std::shared_ptr<Coup> jouerTour(Plateau& plateau) = 0;

protected:
    std::string nom;
    Couleur     couleur;
    bool        estElimine;
    double      score;
};

#endif // JOUEUR_HPP
