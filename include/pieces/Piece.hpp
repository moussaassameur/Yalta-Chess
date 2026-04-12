#ifndef PIECE_HPP
#define PIECE_HPP

#include <string>
#include <vector>
#include <memory>

// Forward declaration : Piece référence Case dans ses méthodes
class Case;

class Plateau; 

class Piece {
protected:
    std::string couleur;             // "blanc", "noir", "rouge"
    std::shared_ptr<Case> position;  // case où se trouve la pièce
    bool estVivanteFlag;             // true tant que la pièce n'est pas capturée

public:
    // Constructeur
    Piece(const std::string& couleur, std::shared_ptr<Case> position);

    //  TRÈS IMPORTANT en C++ : destructeur virtuel dans une classe de base
    // Sans ça, supprimer une Roi via un pointeur Piece* fuirait la mémoire
    virtual ~Piece() = default;

    // Méthode virtuelle pure pour obtenir les déplacements possibles
   virtual std::vector<std::shared_ptr<Case>> getDeplacements(const Plateau& plateau) const = 0;

    // Méthode virtuelle pure pour identifier le type ("Roi", "Reine", etc.)
    // Utile pour l'affichage et la sérialisation
    virtual std::string getType() const = 0;

    // Méthodes communes à toutes les pièces (non-virtuelles)
    bool estVivante() const;
    void deplacer(std::shared_ptr<Case> cible);
    void capturer();  // marque la pièce comme morte

    // Getters
    std::string getCouleur() const;
    std::shared_ptr<Case> getPosition() const;
};

#endif // PIECE_HPP