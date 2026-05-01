#ifndef CASE_HPP
#define CASE_HPP

#include "model/Couleur.hpp"
#include <memory>
#include <string>

class Piece;

/**
 * @file Case.hpp
 * @brief Une case du plateau Yalta.
 *
 * Le plateau est stocke comme une grille virtuelle 12x12. Sur les 144
 * positions, seules 96 sont reellement utilisees -- les 48 autres sont
 * des trous (la grille n'est pas rectangulaire mais hexagonale).
 *
 * Chaque case porte :
 *   - ses coordonnees globales (x, y) dans la grille 12x12,
 *   - le numero de sextant a laquelle elle appartient (0..5),
 *   - la couleur damier de la case ("clair" ou "fonce"),
 *   - une notation Yalta du type "d2" (lettre du fichier + numero de
 *     rangee), unique au sein du tiers de son proprietaire,
 *   - un pointeur eventuel vers une piece occupant la case.
 */
class Case {
public:
    Case(int x, int y, int sextant, const std::string& couleur);
    ~Case() = default;

    /// @return Coordonnee x globale (0..11).
    int getX() const;
    /// @return Coordonnee y globale (0..11).
    int getY() const;
    /// @return Numero du sextant (0..5).
    int getSextant() const;
    /// @return Couleur damier ("clair" ou "fonce").
    std::string getCouleurDamier() const;

    /// @name Notation Yalta (un identifiant unique par case, ex. "a1", "h9")
    /// @{

    /// @return Couleur du joueur proprietaire du sextant de cette case.
    Couleur getProprietaire() const;
    /**
     * @return Lettre de colonne 'a'..'l'. Chaque colonne traverse deux
     *         sextants ; la formule depend du sextant (voir Case.cpp).
     */
    char getFile() const;
    /**
     * @return Numero de rangee 1..12. La formule depend du sextant.
     *         Les colonnes a..d ont les rangs 1..8, e..h ont 1..4 et 9..12,
     *         i..l ont les rangs 5..12.
     */
    int getRank() const;
    /// @return Notation complete de type "a1", "d8", "e4", "i9", "l12", etc.
    std::string getNotation() const;

    /// @}

    /// @name Gestion de la piece occupant la case
    /// @{

    /// Pose une piece sur la case (remplace l'eventuelle piece presente).
    void setPiece(std::shared_ptr<Piece> p);

    /// Retire la piece de la case (sans la capturer ni la modifier).
    void retirerPiece();

    /// @return Pointeur vers la piece presente, ou nullptr si vide.
    std::shared_ptr<Piece> getPiece() const;

    /// @return true si la case contient une piece.
    bool estOccupee() const;

    /// @}

private:
    int                    x;
    int                    y;
    int                    sextant;
    std::string            couleurDamier;
    std::shared_ptr<Piece> piece;  ///< nullptr = case libre
};

#endif // CASE_HPP
