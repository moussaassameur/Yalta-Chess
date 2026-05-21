#ifndef COUP_SIMPLE_HPP
#define COUP_SIMPLE_HPP

#include "coup/Coup.hpp"
#include <memory>

class Case;
class Piece;

/**
 * @file CoupSimple.hpp
 * @brief Coup courant : deplacer une piece, avec capture eventuelle.
 */
class CoupSimple : public Coup {
public:
    // depart : case avec la piece. arrivee : destination (vide ou ennemie).
    CoupSimple(std::shared_ptr<Case> depart,
               std::shared_ptr<Case> arrivee);

    ~CoupSimple() override = default;

    void        executer(Plateau& plateau)              override;
    void        annuler(Plateau& plateau)               override;
    bool        estValide(const Plateau& plateau) const override;
    std::string getNotation() const                     override;

    /// @return Case de depart.
    std::shared_ptr<Case>  getDepart() const;
    /// @return Case d'arrivee.
    std::shared_ptr<Case>  getArrivee() const;
    /// @return Piece deplacee par ce coup (apres execution).
    std::shared_ptr<Piece> getPiece() const;
    /// @return Piece capturee, ou nullptr si pas de capture.
    std::shared_ptr<Piece> getPieceCapturee() const;
    /// @return true si le coup a captur une piece a l'execution.
    bool estCapture() const;

private:
    std::shared_ptr<Case>  depart;
    std::shared_ptr<Case>  arrivee;

    /// Piece deplacee (capturee au moment de executer pour avoir un
    /// pointeur stable, meme si la case change).
    std::shared_ptr<Piece> pieceDeplacee;

    /// Piece capturee a l'arrivee (nullptr si pas de capture).
    std::shared_ptr<Piece> pieceCapturee;

    /// Sauvegarde du drapeau aDejaBouge de la piece avant execution.
    bool aDejaBougeAvant;

    /// Drapeau interne  true si executer() a effectivement eu lieu.
    bool execute;
};

#endif // COUP_SIMPLE_HPP
