#ifndef PIECE_HPP
#define PIECE_HPP

#include "model/Couleur.hpp"
#include <memory>
#include <string>
#include <vector>

class Case;
class Plateau;

/**
 * @file Piece.hpp
 * @brief Classe de base abstraite pour toutes les pieces du jeu.
 *
 * Une piece connait sa couleur, sa position courante (case) et un drapeau
 * "a deja bouge" utilise par les pieces qui en dependent (Pion pour le
 * bond initial, Tour et Roi pour le roque). Les pieces qui ne s'en
 * servent pas l'ignorent simplement.
 *
 * Les sous-classes concretisent :
 *   - getType()       : libelle texte ("Pion", "Tour", ...).
 *   - getDeplacements : ensemble des cases atteignables, regles propres
 *                       a chaque piece + topologie Yalta.
 *
 * Cycle de vie :
 *   - capturer()    : marque la piece comme non-vivante (estVivante()
 *                     renvoie alors false). Sa position est invalidee.
 *   - ressusciter() : remet la piece en vie -- utilise par Coup::annuler
 *                     pour defaire une capture.
 */
class Piece {
public:
    Piece(Couleur couleur);
    virtual ~Piece() = default;

    /// @return Couleur du joueur proprietaire.
    Couleur getCouleur() const;

    /// @return Case courante (ou nullptr si la piece a ete capturee).
    std::shared_ptr<Case> getPosition() const;

    /// @brief Met a jour la case courante (utilise par les Coup).
    void setPosition(std::shared_ptr<Case> c);

    /// @return true si la piece a deja effectue au moins un coup.
    bool getADejaBouge() const;

    /// @brief Met a jour le drapeau "a deja bouge".
    void setADejaBouge(bool v);

    /// @return true si la piece est encore en jeu.
    bool estVivante() const;

    /// @brief Marque la piece comme capturee (non vivante, pas de position).
    void capturer();

    /// @brief Restaure une piece capturee (utilise par Coup::annuler).
    void ressusciter();

    /// @return Libelle texte du type ("Pion", "Tour", "Cavalier", ...).
    virtual std::string getType() const = 0;

    /**
     * @brief Liste les cases atteignables par la piece sur le plateau.
     * @param plateau Plateau actuel (necessaire pour le voisinage).
     * @return Vecteur des cases destinations possibles.
     *
     * Cette liste applique les regles propres a la piece + la topologie
     * du plateau Yalta (bending, center-cross). Elle ne filtre PAS les
     * coups qui mettent son propre roi en echec -- ce filtrage est du
     * ressort du moteur de jeu (ModeleJeu, Phase 5).
     */
    virtual std::vector<std::shared_ptr<Case>>
    getDeplacements(const Plateau& plateau) const = 0;

protected:
    Couleur               couleur;
    std::shared_ptr<Case> position;
    bool                  aDejaBouge;
    bool                  vivante;
};

#endif // PIECE_HPP
