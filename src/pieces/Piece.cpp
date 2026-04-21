#include "pieces/Piece.hpp"
#include "model/Case.hpp"

Piece::Piece(Couleur couleur, std::shared_ptr<Case> position)
    : couleur(couleur), position(position), estVivanteFlag(true) {
}

bool Piece::estVivante() const {
    return estVivanteFlag;
}

void Piece::deplacer(std::shared_ptr<Case> cible) {
    if (cible == nullptr) return;

    if (position != nullptr) {
        position->retirerPiece();
    }
    position = cible;
}

void Piece::capturer() {
    estVivanteFlag = false;
    position = nullptr;
}

void Piece::ressusciter() {
    estVivanteFlag = true;
}

Couleur Piece::getCouleur() const {
    return couleur;
}

std::shared_ptr<Case> Piece::getPosition() const {
    return position;
}

void Piece::setPosition(std::shared_ptr<Case> nouvelleCase) {
    position = nouvelleCase;
}
