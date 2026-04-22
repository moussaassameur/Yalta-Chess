#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "coup/Coup.hpp"
#include "coup/CoupSimple.hpp"
#include "pieces/Piece.hpp"
#include "pieces/Roi.hpp"
#include "pieces/Reine.hpp"
#include "pieces/Tour.hpp"
#include "pieces/Fou.hpp"
#include "pieces/Cavalier.hpp"
#include "pieces/Pion.hpp"
#include <algorithm>

Plateau::Plateau() {
    creerCases();
    initialiserPieces();
}

// Cree 96 cases dans une grille virtuelle 12x12 avec trous.
// Le plateau hexagonal Yalta est divise en 6 sextants (rhombes), chacun
// contenant 4x4 = 16 cases. La grille 12x12 est decoupee en 9 blocs 4x4
// dont seulement 6 sont des sextants valides ; les 3 autres restent vides.
//
// Mapping des sextants :
//   sextant 0 : x in [0,4),  y in [0,4)    -> coin v5 (bas-gauche du hexagone)
//   sextant 1 : x in [0,4),  y in [4,8)    -> coin v6 (gauche)
//   sextant 2 : x in [8,12), y in [4,8)    -> coin v1 (haut-gauche)
//   sextant 3 : x in [8,12), y in [8,12)   -> coin v2 (haut-droite)
//   sextant 4 : x in [4,8),  y in [8,12)   -> coin v3 (droite)
//   sextant 5 : x in [4,8),  y in [0,4)    -> coin v4 (bas-droite)
void Plateau::creerCases() {
    struct Bloc { int x0; int x1; int y0; int y1; int sextant; };
    const Bloc blocs[6] = {
        { 0,  4,  0,  4, 0 },
        { 0,  4,  4,  8, 1 },
        { 8, 12,  4,  8, 2 },
        { 8, 12,  8, 12, 3 },
        { 4,  8,  8, 12, 4 },
        { 4,  8,  0,  4, 5 },
    };

    for (const auto& b : blocs) {
        for (int x = b.x0; x < b.x1; x++) {
            for (int y = b.y0; y < b.y1; y++) {
                // Damier : couleur depend de (x + y + sextant) parite
                std::string couleurCase =
                    ((x + y + b.sextant) % 2 == 0) ? "clair" : "fonce";
                cases[{x, y}] = std::make_shared<Case>(x, y, b.sextant, couleurCase);
            }
        }
    }
}

std::shared_ptr<Case> Plateau::getCase(int q, int r) const {
    auto it = cases.find({q, r});
    return (it != cases.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<Case>> Plateau::getToutesLesCases() const {
    std::vector<std::shared_ptr<Case>> res;
    for (const auto& p : cases) res.push_back(p.second);
    return res;
}

std::vector<std::shared_ptr<Case>> Plateau::getCasesLibres() const {
    std::vector<std::shared_ptr<Case>> res;
    for (const auto& p : cases)
        if (!p.second->estOccupee()) res.push_back(p.second);
    return res;
}

// Placement initial Yalta.
// Chaque joueur possede 2 sextants adjacents. Sa back-rank (8 pieces) longe
// le bord exterieur du hexagone forme par ses 2 sextants reunis :
//   - Une moitie de back-rank vient d'un sextant (rangee y_local=0)
//   - L'autre moitie vient du sextant adjacent (colonne x_local=0)
// La rangee de pions est juste a l'interieur de la back-rank.
//
// Repartition des sextants par joueur :
//   BLANC  = sextants 0 + 5 (bas du hexagone)
//   ROUGE  = sextants 1 + 2 (gauche du hexagone)
//   NOIR   = sextants 3 + 4 (droite du hexagone)
void Plateau::initialiserPieces() {
    auto placer = [&](int x, int y, std::shared_ptr<Piece> p) {
        auto c = getCase(x, y);
        if (c) { c->setPiece(p); p->setPosition(c); }
    };

    // ─── BLANC (sextants 0 + 5) ───
    // Sextant 0 : back rank sur y=0, pions sur y=1
    placer(0, 0, std::make_shared<Tour>    (Couleur::BLANC, nullptr));
    placer(1, 0, std::make_shared<Cavalier>(Couleur::BLANC, nullptr));
    placer(2, 0, std::make_shared<Fou>     (Couleur::BLANC, nullptr));
    placer(3, 0, std::make_shared<Reine>   (Couleur::BLANC, nullptr));
    for (int x = 0; x < 4; x++)
        placer(x, 1, std::make_shared<Pion>(Couleur::BLANC, nullptr));

    // Sextant 5 : back rank sur x=4 (col), pions sur x=5
    placer(4, 0, std::make_shared<Tour>    (Couleur::BLANC, nullptr));
    placer(4, 1, std::make_shared<Cavalier>(Couleur::BLANC, nullptr));
    placer(4, 2, std::make_shared<Fou>     (Couleur::BLANC, nullptr));
    placer(4, 3, std::make_shared<Roi>     (Couleur::BLANC, nullptr));
    for (int y = 0; y < 4; y++)
        placer(5, y, std::make_shared<Pion>(Couleur::BLANC, nullptr));

    // ─── ROUGE (sextants 1 + 2) ───
    // Sextant 1 : back rank sur x=0 (col), pions sur x=1
    placer(0, 4, std::make_shared<Tour>    (Couleur::ROUGE, nullptr));
    placer(0, 5, std::make_shared<Cavalier>(Couleur::ROUGE, nullptr));
    placer(0, 6, std::make_shared<Fou>     (Couleur::ROUGE, nullptr));
    placer(0, 7, std::make_shared<Roi>     (Couleur::ROUGE, nullptr));
    for (int y = 4; y < 8; y++)
        placer(1, y, std::make_shared<Pion>(Couleur::ROUGE, nullptr));

    // Sextant 2 : back rank sur y=4 (row), pions sur y=5
    placer(8,  4, std::make_shared<Tour>    (Couleur::ROUGE, nullptr));
    placer(9,  4, std::make_shared<Cavalier>(Couleur::ROUGE, nullptr));
    placer(10, 4, std::make_shared<Fou>     (Couleur::ROUGE, nullptr));
    placer(11, 4, std::make_shared<Reine>   (Couleur::ROUGE, nullptr));
    for (int x = 8; x < 12; x++)
        placer(x, 5, std::make_shared<Pion>(Couleur::ROUGE, nullptr));

    // ─── NOIR (sextants 3 + 4) ───
    // Sextant 4 : back rank sur y=8 (row), pions sur y=9
    placer(4, 8, std::make_shared<Tour>    (Couleur::NOIR, nullptr));
    placer(5, 8, std::make_shared<Cavalier>(Couleur::NOIR, nullptr));
    placer(6, 8, std::make_shared<Fou>     (Couleur::NOIR, nullptr));
    placer(7, 8, std::make_shared<Reine>   (Couleur::NOIR, nullptr));
    for (int x = 4; x < 8; x++)
        placer(x, 9, std::make_shared<Pion>(Couleur::NOIR, nullptr));

    // Sextant 3 : back rank sur x=8 (col), pions sur x=9
    placer(8,  8, std::make_shared<Tour>    (Couleur::NOIR, nullptr));
    placer(8,  9, std::make_shared<Cavalier>(Couleur::NOIR, nullptr));
    placer(8, 10, std::make_shared<Fou>     (Couleur::NOIR, nullptr));
    placer(8, 11, std::make_shared<Roi>     (Couleur::NOIR, nullptr));
    for (int y = 8; y < 12; y++)
        placer(9, y, std::make_shared<Pion>(Couleur::NOIR, nullptr));
}

void Plateau::deplacerPiece(Coup& coup) {
    coup.executer(*this);
}

void Plateau::placerRangee(Couleur /*couleur*/) {}

std::vector<std::shared_ptr<Piece>> Plateau::getPiecesDeCouleur(Couleur couleur) const {
    std::vector<std::shared_ptr<Piece>> res;
    for (const auto& p : cases) {
        auto piece = p.second->getPiece();
        if (piece && piece->estVivante() && piece->getCouleur() == couleur)
            res.push_back(piece);
    }
    return res;
}

std::shared_ptr<Case> Plateau::getCaseRoi(Couleur couleur) const {
    for (const auto& p : cases) {
        auto piece = p.second->getPiece();
        if (piece && piece->estVivante()
            && piece->getCouleur() == couleur
            && piece->getType() == "Roi")
            return p.second;
    }
    return nullptr;
}

bool Plateau::estMenacee(std::shared_ptr<Case> cible, Couleur attaquant) const {
    if (!cible) return false;
    auto piecesAttaquantes = getPiecesDeCouleur(attaquant);
    for (const auto& piece : piecesAttaquantes) {
        auto casesPossibles = piece->getDeplacements(*this);
        for (const auto& c : casesPossibles) {
            if (c == cible) return true;
        }
    }
    return false;
}

bool Plateau::estEnEchec(Couleur couleurJoueur) const {
    auto caseRoi = getCaseRoi(couleurJoueur);
    if (!caseRoi) return false;

    for (Couleur adversaire : {Couleur::BLANC, Couleur::NOIR, Couleur::ROUGE}) {
        if (adversaire == couleurJoueur) continue;
        if (estMenacee(caseRoi, adversaire)) return true;
    }
    return false;
}

bool Plateau::aDesCoupsLegaux(Couleur couleurJoueur) {
    auto pieces = getPiecesDeCouleur(couleurJoueur);
    for (const auto& piece : pieces) {
        auto casesAccessibles = piece->getDeplacements(*this);
        for (const auto& caseArrivee : casesAccessibles) {
            auto caseDepart = piece->getPosition();
            auto coup = std::make_shared<CoupSimple>(caseDepart, caseArrivee, piece);
            if (!coup->estValide(*this)) continue;

            coup->executer(*this);
            bool encoreEnEchec = estEnEchec(couleurJoueur);
            coup->annuler(*this);

            if (!encoreEnEchec) return true;
        }
    }
    return false;
}
