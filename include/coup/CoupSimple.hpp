#ifndef COUP_SIMPLE_HPP
#define COUP_SIMPLE_HPP

#include "coup/Coup.hpp"
#include <memory>

class Case;
class Piece;

/**
 * @file CoupSimple.hpp
 * @brief Coup le plus courant : deplacer une piece d'une case vers une
 *        autre, avec capture eventuelle.
 *
 * Implemente l'interface Coup (Pattern Command). Memoorise tout ce qui
 * est necessaire pour annuler() :
 *   - la piece qui a ete deplacee (pour la remettre a son point de depart),
 *   - la piece capturee eventuelle (pour la ressusciter sur l'arrivee),
 *   - l'etat aDejaBouge de la piece avant le coup (drapeau utilise par
 *     les pieces sensibles : Pion pour le bond initial, Tour et Roi pour
 *     le roque).
 */
class CoupSimple : public Coup {
public:
    /**
     * @brief Construit un coup simple entre deux cases.
     * @param depart  Case de depart -- doit contenir une piece a executer.
     * @param arrivee Case d'arrivee -- peut etre vide ou contenir une
     *                piece adverse (qui sera capturee).
     */
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

    /// Drapeau interne : true si executer() a effectivement eu lieu.
    bool execute;
};

#endif // COUP_SIMPLE_HPP
