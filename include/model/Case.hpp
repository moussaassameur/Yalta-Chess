#ifndef CASE_HPP
#define CASE_HPP

#include <string>
#include <memory>

class Piece;

// Une case du plateau Yalta (modele 6 sextants).
// Coords principales : (x, y) sur une grille virtuelle 12x12 avec trous.
//   - x, y : 0..11
//   - sextant : 0..5 (le sextant geometrique auquel appartient la case)
// Seules ~96 cases sur 144 sont valides (6 sextants x 4x4 = 96).
// On expose getQ()/getR() = (x, y) pour la compatibilite avec
// l API getCase(q, r) deja utilisee dans le projet.
class Case {
private:
    int x;
    int y;
    int sextant;
    std::shared_ptr<Piece> piece;
    std::string couleur;  // "clair" ou "fonce"

public:
    Case(int x, int y, int sextant, const std::string& couleur);
    ~Case() = default;

    int getX() const;
    int getY() const;
    int getSextant() const;

    // Compat (q, r) — equivalent a (x, y)
    int getQ() const;
    int getR() const;
    int getS() const;        // toujours 0
    int getSecteur() const;  // alias getSextant()

    std::string getPosition() const;  // "x,y" en chaine

    std::string getCouleur() const;
    std::shared_ptr<Piece> getPiece() const;

    void setPiece(std::shared_ptr<Piece> nouvellePiece);
    void retirerPiece();

    bool estOccupee() const;

    static int distance(const Case& a, const Case& b);

    bool operator==(const Case& autre) const;
};

#endif // CASE_HPP
