#ifndef CASE_HPP
#define CASE_HPP

#include <string>
#include <memory>

class Piece;

class Case {
private:
    int q;                           // coordonnée axiale q
    int r;                           // coordonnée axiale r
    std::shared_ptr<Piece> piece;    // nullptr si la case est vide
    std::string couleur;             // couleur de la case ("clair", "moyen", "fonce")
    int secteur;                     // 0, 1 ou 2 — quel tiers du plateau (joueur 1/2/3)

public:
    // Constructeur
    Case(int q, int r, const std::string& couleur, int secteur);
    ~Case() = default;

    // Getters
    int getQ() const;
    int getR() const;
    int getS() const;                // calculé : s = -q - r
    int getSecteur() const;
    std::string getCouleur() const;
    std::shared_ptr<Piece> getPiece() const;

    // Setters / modifications
    void setPiece(std::shared_ptr<Piece> nouvellePiece);
    void retirerPiece();

    // Méthodes utiles
    bool estOccupee() const;

    // Distance hexagonale entre 2 cases (utile pour l'IA et la validation)
    static int distance(const Case& a, const Case& b);

    // Égalité (utile pour comparer des cases dans des listes)
    bool operator==(const Case& autre) const;
};

#endif // CASE_HPP