#pragma once

namespace Yalta {

// On déclare Piece ici sans l'inclure
// pour éviter les inclusions circulaires
class Piece;

/**
 * @brief Représente une case du plateau Yalta
 */
class Case {

private:
    int ligne;        ///< Ligne de la case (0-11)
    int colonne;      ///< Colonne de la case (0-7)
    bool couleur;     ///< true = blanc, false = noir
    Piece* piece;     ///< Pointeur vers la pièce sur la case (nullptr si vide)

public:
    /**
     * @brief Constructeur
     * @param ligne La ligne de la case
     * @param colonne La colonne de la case
     * @param couleur La couleur de la case
     */
    Case(int ligne, int colonne, bool couleur);

    /**
     * @brief Destructeur
     */
    ~Case();

    // Getters
    int getLigne() const;
    int getColonne() const;
    bool getCouleur() const;
    Piece* getPiece() const;

    // Setters
    void setPiece(Piece* piece);

    // Methodes
    bool estOccupee() const;
    void vider();
};

} // namespace Yalta