#ifndef PLATEAU_HPP
#define PLATEAU_HPP

#include <memory>
#include <vector>
#include <map>
#include <utility>

// On declare les classes qu'on va utiliser
class Case;
class Piece;
class Joueur;
class Coup;

// La classe Plateau c'est le plateau de jeu du Yalta
// Y'a 96 cases en forme d'hexagone divise en 3 secteurs
class Plateau {
private:
    // On met toutes les cases dans une map
    // La cle c'est (q,r) et la valeur c'est la case
    // Ca permet d'avoir une forme bizarre comme l'hexagone
    std::map<std::pair<int,int>, std::shared_ptr<Case>> cases;

public:
    // Constructeur : on cree tout
    Plateau();

    ~Plateau() = default;

    // Pour recuperer une case avec ses coordonnees
    // Si la case existe pas on retourne nullptr
    std::shared_ptr<Case> getCase(int q, int r) const;

    // Pour avoir toutes les cases
    std::vector<std::shared_ptr<Case>> getToutesLesCases() const;

    // Pour avoir juste les cases vides
    std::vector<std::shared_ptr<Case>> getCasesLibres() const;

    // On place les pieces au debut de la partie
    void initialiserPieces();

    // On verifie si un joueur est en echec
    bool estEnEchec(const Joueur& joueur) const;

    // Pour deplacer une piece
    void deplacerPiece(Coup& coup);

private:
    // Methode privee pour creer les cases
    void creerCases();
};

#endif