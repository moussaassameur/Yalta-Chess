#ifndef CASE_HPP
#define CASE_HPP

#include <string>
#include <memory>

// Forward declaration : Case et Piece se référencent mutuellement,
// donc on déclare juste l'existence de Piece sans inclure son header.
// L'include complet sera dans Case.cpp.
class Piece;

class Case {
private:
    int ligne;
    int colonne;
    std::shared_ptr<Piece> piece;  // nullptr si la case est vide
    std::string couleur;            // couleur de la case du plateau ("clair", "fonce", etc.)

public:
    // Constructeur
    Case(int ligne, int colonne, const std::string& couleur);

    // Destructeur par défaut (les shared_ptr se nettoient tout seuls)
    ~Case() = default;

    // Getters
    int getLigne() const;
    int getColonne() const;
    std::string getCouleur() const;
    std::shared_ptr<Piece> getPiece() const;

    // Setters / modifications
    void setPiece(std::shared_ptr<Piece> nouvellePiece);
    void retirerPiece();

    // Méthode utile
    bool estOccupee() const;
};

#endif // CASE_HPP