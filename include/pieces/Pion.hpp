#ifndef PION_HPP
#define PION_HPP

#include "pieces/Piece.hpp"

/**
 * @file Pion.hpp
 * @brief Pion du jeu Yalta.
 *
 * Grace a la notation globale (a1-l12), chaque pion reste sur la meme
 * colonne (file) tout au long de son parcours. Un pion 'a' traverse
 * a1→a2→a3→a4 (sextant S0) puis a5→a6→a7→a8 (sextant S1).
 *
 * Avancement (rook direction) :
 *   - 1 case orthogonale dans le sens qui eloigne le pion de son
 *     back rank. Le bending via voisinAvecDir gere automatiquement
 *     le passage d'un sextant a l'autre.
 *   - Bond initial de 2 cases si aDejaBouge == false et les 2 cases
 *     devant sont libres.
 *
 * Captures (bishop direction) :
 *   - 1 case diagonale dans l'une des 2 directions "avant" (avant =
 *     composante forward + composante perpendiculaire).
 *   - Via Plateau::voisinDiagonal qui applique la reflexion correcte
 *     aux bords des sextants.
 *   - Center-cross : depuis l'apex (3,3) du TIERS PROPRE, si la
 *     diagonale traverse simultanement les deux bords, le pion a deux
 *     options de capture comme le fou (apexes same-color en face).
 *
 * NB : aDejaBouge est gere par Piece et mis a jour par CoupSimple.
 */
class Pion : public Piece {
public:
    /**
     * @brief Construit un pion d'une couleur donnee.
     * @param couleur Couleur du joueur proprietaire.
     */
    Pion(Couleur couleur);

    ~Pion() override = default;

    std::string getType() const override;

    std::vector<std::shared_ptr<Case>>
    getDeplacements(const Plateau& plateau) const override;

    /// @return true si la case 'dest' est un back rank ennemi pour un pion
    ///         de la couleur donnee (= case de promotion).
    static bool estCaseDePromotion(const std::shared_ptr<Case>& dest,
                                   Couleur couleur);

private:
    /**
     * @brief Calcule la direction d'avancement (dx, dy) en fonction du
     *        sextant courant et du fait qu'il est dans le tiers propre.
     * @param[out] dx Composante x (-1, 0 ou +1).
     * @param[out] dy Composante y (-1, 0 ou +1).
     */
    void calculerDirection(int& dx, int& dy) const;
};

#endif // PION_HPP
