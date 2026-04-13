#include "model/Coup.hpp"
#include "model/Case.hpp"
#include "pieces/Piece.hpp"

// Constructeur : on initialise tout
Coup::Coup(std::shared_ptr<Case> depart,
           std::shared_ptr<Case> arrivee,
           std::shared_ptr<Piece> piece)
    : depart(depart),
      arrivee(arrivee),
      piece(piece),
      type("simple"),
      pieceCapturee(nullptr),
      aDejaBougeAvant(false),
      dejaExecute(false) {
}

// Methode executer : c'est ici qu'on joue le coup
void Coup::executer() {
    // Si le coup est deja execute on fait rien (securite)
    if (dejaExecute) return;

    // On verifie que tout est ok
    if (depart == nullptr || arrivee == nullptr || piece == nullptr) return;

    // D'abord on regarde si il y a une piece sur la case d'arrivee
    // Si oui on la garde en memoire pour pouvoir la remettre si on annule
    if (arrivee->estOccupee()) {
        pieceCapturee = arrivee->getPiece();

        // Si c'est une piece ennemie on la capture
        if (pieceCapturee->getCouleur() != piece->getCouleur()) {
            type = "capture";
            pieceCapturee->capturer();
        }
    }

    // Maintenant on deplace la piece
    depart->retirerPiece();
    arrivee->setPiece(piece);

    // On marque que le coup est fait
    dejaExecute = true;
}

// Methode annuler : on revient en arriere
void Coup::annuler() {
    // Si le coup a pas ete execute on peut pas l'annuler
    if (!dejaExecute) return;

    if (depart == nullptr || arrivee == nullptr || piece == nullptr) return;

    // On enleve la piece de la case d'arrivee
    arrivee->retirerPiece();

    // On la remet sur la case de depart
    depart->setPiece(piece);

    // Si on avait capture une piece on la remet sur la case d'arrivee
    if (pieceCapturee != nullptr) {
        arrivee->setPiece(pieceCapturee);
        pieceCapturee->ressusciter();  // on la remet en vie
    }

    // Le coup est plus execute
    dejaExecute = false;
}

// Verifie si le coup est valide
bool Coup::estValide() const {
    // Si une des references est nulle le coup est pas valide
    if (depart == nullptr || arrivee == nullptr || piece == nullptr) {
        return false;
    }

    // La piece doit vraiment etre sur la case de depart
    if (depart->getPiece() != piece) {
        return false;
    }

    // On peut pas capturer une piece de sa propre couleur
    if (arrivee->estOccupee()
        && arrivee->getPiece()->getCouleur() == piece->getCouleur()) {
        return false;
    }

    return true;
}

// Les getters : juste pour recuperer les valeurs
std::shared_ptr<Case> Coup::getDepart() const {
    return depart;
}

std::shared_ptr<Case> Coup::getArrivee() const {
    return arrivee;
}

std::shared_ptr<Piece> Coup::getPiece() const {
    return piece;
}

std::shared_ptr<Piece> Coup::getPieceCapturee() const {
    return pieceCapturee;
}

std::string Coup::getType() const {
    return type;
}