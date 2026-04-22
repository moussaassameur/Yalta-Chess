#ifndef PLATEAU_HPP
#define PLATEAU_HPP

#include "model/Couleur.hpp"
#include <memory>
#include <vector>
#include <map>
#include <utility>

class Case;
class Piece;
class Coup;

// Plateau hexagonal Yalta — 96 cases, 3 secteurs, 48 pieces
class Plateau {
private:
    std::map<std::pair<int,int>, std::shared_ptr<Case>> cases;

public:
    Plateau();
    ~Plateau() = default;

    std::shared_ptr<Case>              getCase(int q, int r) const;
    std::vector<std::shared_ptr<Case>> getToutesLesCases()   const;
    std::vector<std::shared_ptr<Case>> getCasesLibres()      const;

    void initialiserPieces();
    void deplacerPiece(Coup& coup);

    // verification echec/mat/pat
    bool estEnEchec(Couleur couleurJoueur) const;
    bool aDesCoupsLegaux(Couleur couleurJoueur);

    // methodes utilitaires
    std::shared_ptr<Case> getCaseRoi(Couleur couleur) const;
    bool estMenacee(std::shared_ptr<Case> cible, Couleur attaquant) const;
    std::vector<std::shared_ptr<Piece>> getPiecesDeCouleur(Couleur couleur) const;

private:
    void creerCases();
    void placerRangee(Couleur couleur);
};

#endif // PLATEAU_HPP
