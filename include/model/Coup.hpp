#ifndef COUP_HPP
#define COUP_HPP

#include "commande/Commande.hpp"
#include <memory>
#include <string>

// On declare les classes qu'on va utiliser pour pas avoir de probleme d'include
class Case;
class Piece;

// La classe Coup represente un deplacement d'une piece
// Elle implemente Commande pour pouvoir etre executee et annulee
class Coup : public Commande {
private:
    std::shared_ptr<Case> depart;         // la case ou la piece est au depart
    std::shared_ptr<Case> arrivee;        // la case ou elle va
    std::shared_ptr<Piece> piece;         // la piece qui bouge
    std::string type;                      // type du coup : simple, capture, etc.

    // Ces attributs servent pour pouvoir annuler le coup plus tard
    std::shared_ptr<Piece> pieceCapturee; // la piece qu'on a capture (null si rien)
    bool aDejaBougeAvant;                  // pour savoir si la piece avait deja bouge avant
    bool dejaExecute;                      // pour eviter d'executer 2 fois le meme coup

public:
    // Constructeur : on donne les 3 infos de base du coup
    Coup(std::shared_ptr<Case> depart,
         std::shared_ptr<Case> arrivee,
         std::shared_ptr<Piece> piece);

    ~Coup() override = default;

    // Les 2 methodes du pattern Command
    void executer() override;
    void annuler() override;

    // Verifie si le coup est valide (basique)
    bool estValide() const;

    // Les getters
    std::shared_ptr<Case> getDepart() const;
    std::shared_ptr<Case> getArrivee() const;
    std::shared_ptr<Piece> getPiece() const;
    std::shared_ptr<Piece> getPieceCapturee() const;
    std::string getType() const;
};

#endif