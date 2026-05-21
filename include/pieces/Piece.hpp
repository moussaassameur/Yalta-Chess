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
 * @brief Classe de base abstraite de toutes les pieces.
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

    // Cases atteignables par la piece (ne filtre pas l'echec au roi).
    virtual std::vector<std::shared_ptr<Case>>
    getDeplacements(const Plateau& plateau) const = 0;

    // Comme getDeplacements, mais retire les coups qui laissent son roi en echec.
    std::vector<std::shared_ptr<Case>>
    getCoupsLegaux(Plateau& plateau) const;

protected:
    Couleur               couleur;
    std::shared_ptr<Case> position;
    bool                  aDejaBouge;
    bool                  vivante;
};

#endif // PIECE_HPP
