#ifndef PIECE_HPP
#define PIECE_HPP

#include "model/Couleur.hpp"
#include <vector>
#include <memory>

class Case;
class Plateau;

class Piece {
protected:
    Couleur couleur;
    std::shared_ptr<Case> position;
    bool estVivanteFlag;

public:
    Piece(Couleur couleur, std::shared_ptr<Case> position);
    virtual ~Piece() = default;

    virtual std::vector<std::shared_ptr<Case>> getDeplacements(const Plateau& plateau) const = 0;
    virtual std::string getType() const = 0;

    bool estVivante() const;
    void deplacer(std::shared_ptr<Case> cible);
    void capturer();
    void ressusciter();  // remet la pièce en vie lors d'un annuler()

    Couleur getCouleur() const;
    std::shared_ptr<Case> getPosition() const;
    void setPosition(std::shared_ptr<Case> nouvelleCase);
};

#endif // PIECE_HPP
