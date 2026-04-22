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

void Plateau::creerCases() {
    const int rayon = 5;
    for (int q = -rayon; q <= rayon; q++) {
        int r1 = std::max(-rayon, -q - rayon);
        int r2 = std::min(rayon,  -q + rayon);
        for (int r = r1; r <= r2; r++) {
            // on determine le secteur de la case selon sa position
            // 0 pour blanc, 1 pour noir, 2 pour rouge
            int secteur = 0;
            if (q > 0 && r >= 0)       secteur = 1;
            else if (q <= 0 && r > 0)  secteur = 2;

            // on calcule la couleur visuelle de la case avec le modulo
            int c = ((q - r) % 3 + 3) % 3;
            std::string couleurCase;
            if (c == 0)      couleurCase = "clair";
            else if (c == 1) couleurCase = "moyen";
            else             couleurCase = "fonce";

            cases[{q, r}] = std::make_shared<Case>(q, r, couleurCase, secteur);
        }
    }
}

std::shared_ptr<Case> Plateau::getCase(int q, int r) const {
    auto it = cases.find({q, r});
    // si la case existe pas on retourne nullptr
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

void Plateau::initialiserPieces() {
    // fonction lambda pour placer une piece sur une case facilement
    auto placer = [&](int q, int r, std::shared_ptr<Piece> p) {
        auto c = getCase(q, r);
        if (c) { c->setPiece(p); p->setPosition(c); }
    };

    // pieces du joueur blanc en bas du plateau
    placer( 0,  5, std::make_shared<Roi>     (Couleur::BLANC, nullptr));
    placer(-1,  5, std::make_shared<Reine>   (Couleur::BLANC, nullptr));
    placer( 1,  5, std::make_shared<Fou>     (Couleur::BLANC, nullptr));
    placer(-2,  5, std::make_shared<Fou>     (Couleur::BLANC, nullptr));
    placer( 2,  5, std::make_shared<Cavalier>(Couleur::BLANC, nullptr));
    placer(-3,  5, std::make_shared<Cavalier>(Couleur::BLANC, nullptr));
    placer( 3,  5, std::make_shared<Tour>    (Couleur::BLANC, nullptr));
    placer(-4,  5, std::make_shared<Tour>    (Couleur::BLANC, nullptr));
    // pions blancs sur la rangee r=4
    for (int q = -4; q <= 4; q++) {
        if (getCase(q, 4))
            placer(q, 4, std::make_shared<Pion>(Couleur::BLANC, nullptr));
    }

    // pieces du joueur noir en haut a gauche
    placer(-5,  0, std::make_shared<Roi>     (Couleur::NOIR, nullptr));
    placer(-5,  1, std::make_shared<Reine>   (Couleur::NOIR, nullptr));
    placer(-5, -1, std::make_shared<Fou>     (Couleur::NOIR, nullptr));
    placer(-5,  2, std::make_shared<Fou>     (Couleur::NOIR, nullptr));
    placer(-5, -2, std::make_shared<Cavalier>(Couleur::NOIR, nullptr));
    placer(-5,  3, std::make_shared<Cavalier>(Couleur::NOIR, nullptr));
    placer(-5, -3, std::make_shared<Tour>    (Couleur::NOIR, nullptr));
    placer(-5,  4, std::make_shared<Tour>    (Couleur::NOIR, nullptr));
    // pions noirs sur la colonne q=-4
    for (int r = -4; r <= 4; r++) {
        if (getCase(-4, r))
            placer(-4, r, std::make_shared<Pion>(Couleur::NOIR, nullptr));
    }

    // pieces du joueur rouge en haut a droite
    placer( 5, -5, std::make_shared<Roi>     (Couleur::ROUGE, nullptr));
    placer( 5, -4, std::make_shared<Reine>   (Couleur::ROUGE, nullptr));
    placer( 4, -5, std::make_shared<Fou>     (Couleur::ROUGE, nullptr));
    placer( 5, -3, std::make_shared<Fou>     (Couleur::ROUGE, nullptr));
    placer( 3, -5, std::make_shared<Cavalier>(Couleur::ROUGE, nullptr));
    placer( 5, -2, std::make_shared<Cavalier>(Couleur::ROUGE, nullptr));
    placer( 2, -5, std::make_shared<Tour>    (Couleur::ROUGE, nullptr));
    placer( 5, -1, std::make_shared<Tour>    (Couleur::ROUGE, nullptr));
    // pions rouges sur la diagonale q+r=4
    for (int q = 1; q <= 5; q++) {
        int r = 4 - q;
        if (getCase(q, r))
            placer(q, r, std::make_shared<Pion>(Couleur::ROUGE, nullptr));
    }
}

void Plateau::deplacerPiece(Coup& coup) {
    coup.executer(*this);
}

void Plateau::placerRangee(Couleur /*couleur*/) {}

// retourne toutes les pieces vivantes d une couleur donnee
std::vector<std::shared_ptr<Piece>> Plateau::getPiecesDeCouleur(Couleur couleur) const {
    std::vector<std::shared_ptr<Piece>> res;
    for (const auto& p : cases) {
        auto piece = p.second->getPiece();
        if (piece && piece->estVivante() && piece->getCouleur() == couleur)
            res.push_back(piece);
    }
    return res;
}

// retourne la case ou se trouve le roi de la couleur donnee
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

// verifie si une case est menacee par n importe quelle piece d une couleur donnee
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

// verifie si le roi de la couleur donnee est en echec
// en yalta le roi peut etre menace par n importe lequel des 2 autres joueurs
bool Plateau::estEnEchec(Couleur couleurJoueur) const {
    auto caseRoi = getCaseRoi(couleurJoueur);
    if (!caseRoi) return false;

    // on verifie si les 2 autres couleurs menacent le roi
    for (Couleur adversaire : {Couleur::BLANC, Couleur::NOIR, Couleur::ROUGE}) {
        if (adversaire == couleurJoueur) continue;
        if (estMenacee(caseRoi, adversaire)) return true;
    }
    return false;
}

// verifie si le joueur a au moins un coup legal
// un coup est legal si apres l avoir joue le roi n est plus en echec
bool Plateau::aDesCoupsLegaux(Couleur couleurJoueur) {
    auto pieces = getPiecesDeCouleur(couleurJoueur);
    for (const auto& piece : pieces) {
        auto casesAccessibles = piece->getDeplacements(*this);
        for (const auto& caseArrivee : casesAccessibles) {
            // on simule le coup
            auto caseDepart = piece->getPosition();
            auto coup = std::make_shared<CoupSimple>(caseDepart, caseArrivee, piece);
            if (!coup->estValide(*this)) continue;

            coup->executer(*this);
            bool encoreEnEchec = estEnEchec(couleurJoueur);
            coup->annuler(*this);

            // si ce coup sort le roi de l echec c est un coup legal
            if (!encoreEnEchec) return true;
        }
    }
    return false;
}
