#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "model/Coup.hpp"
#include "joueur/Joueur.hpp"
#include "pieces/Piece.hpp"
#include "pieces/Roi.hpp"
#include "pieces/Reine.hpp"
#include "pieces/Tour.hpp"
#include "pieces/Fou.hpp"
#include "pieces/Cavalier.hpp"
#include "pieces/Pion.hpp"

// Constructeur : on cree les cases et on place les pieces
Plateau::Plateau() {
    creerCases();
    initialiserPieces();
}

// Cree toutes les cases de l'hexagone
void Plateau::creerCases() {
    // On fait un hexagone de rayon 5
    // Ca donne a peu pres le bon nombre de cases
    // On ajustera plus tard pour avoir exactement 96
    const int rayon = 5;

    for (int q = -rayon; q <= rayon; q++) {
        // On calcule les limites de r pour rester dans l'hexagone
        int r1 = std::max(-rayon, -q - rayon);
        int r2 = std::min(rayon, -q + rayon);

        for (int r = r1; r <= r2; r++) {
            // On determine le secteur de la case (0, 1 ou 2)
            int secteur = 0;
            if (q > 0 && r >= 0) {
                secteur = 1;
            } else if (q <= 0 && r > 0) {
                secteur = 2;
            }

            // On determine la couleur de la case
            // Y'a 3 couleurs possibles : clair, moyen, fonce
            int c = ((q - r) % 3 + 3) % 3;
            std::string couleur;
            if (c == 0) couleur = "clair";
            else if (c == 1) couleur = "moyen";
            else couleur = "fonce";

            // On cree la case et on l'ajoute a la map
            auto nouvelleCase = std::make_shared<Case>(q, r, couleur, secteur);
            cases[std::make_pair(q, r)] = nouvelleCase;
        }
    }
}

// Recupere une case avec ses coordonnees (q, r)
std::shared_ptr<Case> Plateau::getCase(int q, int r) const {
    // On cherche dans la map
    auto it = cases.find(std::make_pair(q, r));

    // Si on trouve pas on retourne nullptr
    if (it == cases.end()) {
        return nullptr;
    }

    // Sinon on retourne la case
    return it->second;
}

// Retourne toutes les cases du plateau
std::vector<std::shared_ptr<Case>> Plateau::getToutesLesCases() const {
    std::vector<std::shared_ptr<Case>> resultat;

    // On parcourt la map et on ajoute chaque case
    for (const auto& paire : cases) {
        resultat.push_back(paire.second);
    }

    return resultat;
}

// Retourne seulement les cases libres
std::vector<std::shared_ptr<Case>> Plateau::getCasesLibres() const {
    std::vector<std::shared_ptr<Case>> resultat;

    // On parcourt et on garde que les cases vides
    for (const auto& paire : cases) {
        if (!paire.second->estOccupee()) {
            resultat.push_back(paire.second);
        }
    }

    return resultat;
}

// Place les pieces au debut de la partie
// Pour l'instant on met juste quelques pieces pour tester
void Plateau::initialiserPieces() {
    // On mettra les 48 pieces plus tard
    // Pour l'instant on teste avec 6 pieces

    // Un roi blanc
    auto caseRoi = getCase(0, 3);
    if (caseRoi != nullptr) {
        auto roi = std::make_shared<Roi>("blanc", caseRoi);
        caseRoi->setPiece(roi);
    }

    // Une tour blanche
    auto caseTour = getCase(-3, 3);
    if (caseTour != nullptr) {
        auto tour = std::make_shared<Tour>("blanc", caseTour);
        caseTour->setPiece(tour);
    }

    // Un fou blanc
    auto caseFou = getCase(3, 0);
    if (caseFou != nullptr) {
        auto fou = std::make_shared<Fou>("blanc", caseFou);
        caseFou->setPiece(fou);
    }

    // Un pion blanc
    auto casePion = getCase(0, 2);
    if (casePion != nullptr) {
        auto pion = std::make_shared<Pion>("blanc", casePion);
        casePion->setPiece(pion);
    }

    // Un roi noir
    auto caseRoiNoir = getCase(0, -3);
    if (caseRoiNoir != nullptr) {
        auto roi = std::make_shared<Roi>("noir", caseRoiNoir);
        caseRoiNoir->setPiece(roi);
    }

    // Un cavalier rouge
    auto caseCav = getCase(-3, 0);
    if (caseCav != nullptr) {
        auto cav = std::make_shared<Cavalier>("rouge", caseCav);
        caseCav->setPiece(cav);
    }
}

// Verifie si un joueur est en echec
// Pour l'instant on fait rien on retournera ca plus tard
bool Plateau::estEnEchec(const Joueur& joueur) const {
    (void)joueur;  // pour pas avoir de warning
    return false;
}

// Deplace une piece en utilisant le coup
void Plateau::deplacerPiece(Coup& coup) {
    coup.executer();
}