#include "pieces/Piece.hpp"
#include "model/Case.hpp"
#include "model/Plateau.hpp"
#include "coup/CoupSimple.hpp"

Piece::Piece(Couleur couleur)
    : couleur(couleur),
      position(nullptr),
      aDejaBouge(false),
      vivante(true) {
}

Couleur               Piece::getCouleur()   const { return couleur; }
std::shared_ptr<Case> Piece::getPosition()  const { return position; }
void                  Piece::setPosition(std::shared_ptr<Case> c) { position = c; }

bool Piece::getADejaBouge()  const         { return aDejaBouge; }
void Piece::setADejaBouge(bool v)          { aDejaBouge = v; }

bool Piece::estVivante() const             { return vivante; }

void Piece::capturer() {
    vivante = false;
    position = nullptr;
}

void Piece::ressusciter() {
    vivante = true;
    // La position sera reaffectee par Coup::annuler.
}

std::vector<std::shared_ptr<Case>>
Piece::getCoupsLegaux(Plateau& plateau) const {
    auto candidats = getDeplacements(plateau);
    std::vector<std::shared_ptr<Case>> legaux;

    for (const auto& dest : candidats) {
        CoupSimple sim(position, dest);
        sim.executer(plateau);
        bool safe = !plateau.estEnEchec(couleur);
        sim.annuler(plateau);
        if (safe) legaux.push_back(dest);
    }
    return legaux;
}
