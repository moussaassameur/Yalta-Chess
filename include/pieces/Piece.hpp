#pragma once

#include <vector>
#include <string>

namespace Yalta {

class Case;
class StrategieDeplacement;

/**
 * @brief Classe abstraite représentant une pièce du jeu Yalta
 */
class Piece {

public:
    /**
     * @brief Enumération des couleurs des joueurs
     */
    enum class Couleur {
        BLANC,  ///< Joueur blanc
        NOIR,   ///< Joueur noir
        ROUGE   ///< Joueur rouge
    };

protected:
    Couleur couleur;                    ///< Couleur de la pièce
    Case* caseActuelle;                 ///< Case actuelle de la pièce
    bool estVivante;                    ///< true si la pièce est en jeu
    std::string nom;                    ///< Nom de la pièce
    StrategieDeplacement* strategie;    ///< Stratégie de déplacement

public:
    /**
     * @brief Constructeur
     * @param couleur La couleur de la pièce
     * @param nom Le nom de la pièce
     */
    Piece(Couleur couleur, std::string nom);

    /**
     * @brief Destructeur virtuel
     */
    virtual ~Piece();

    /**
     * @brief Retourne les cases disponibles
     * @param plateau Le plateau de jeu
     * @return Liste des cases accessibles
     */
    std::vector<Case*> getDeplacements(
        std::vector<std::vector<Case*>>& plateau);

    // Getters
    Couleur getCouleur() const;
    Case* getCaseActuelle() const;
    bool getEstVivante() const;
    std::string getNom() const;
    StrategieDeplacement* getStrategie() const;

    // Setters
    void setCaseActuelle(Case* c);
    void setEstVivante(bool vivante);
    void setStrategie(StrategieDeplacement* s);
};

} 