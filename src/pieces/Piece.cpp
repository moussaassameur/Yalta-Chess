#include "pieces/Piece.hpp"
#include "model/Case.hpp"  // include complet ici
#include "model/Plateau.hpp"
Piece::Piece(const std::string& couleur, std::shared_ptr<Case> position)
    : couleur(couleur), position(position), estVivanteFlag(true) {
}

bool Piece::estVivante() const {
    return estVivanteFlag;
}

void Piece::deplacer(std::shared_ptr<Case> cible) {
    if (cible == nullptr) {
        return;  // sécurité
    }

    // Si la case d'arrivée contient une pièce ennemie, on la capture
    if (cible->estOccupee()) {
        std::shared_ptr<Piece> pieceCible = cible->getPiece();
        if (pieceCible->getCouleur() != this->couleur) {
            pieceCible->capturer();
        }
    }

    // Libérer l'ancienne case
    if (position != nullptr) {
        position->retirerPiece();
    }

    // Mettre à jour la position
    position = cible;
    // Note : c'est au Plateau ou au Jeu d'appeler cible->setPiece(...)
    // pour éviter les problèmes de shared_from_this ici
}

void Piece::capturer() {
    estVivanteFlag = false;
    position = nullptr;
}

std::string Piece::getCouleur() const {
    return couleur;
}

std::shared_ptr<Case> Piece::getPosition() const {
    return position;
}