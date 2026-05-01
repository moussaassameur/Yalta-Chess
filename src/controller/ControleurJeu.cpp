#include "controller/ControleurJeu.hpp"
#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "view/VueJeu.hpp"
#include "coup/CoupSimple.hpp"
#include "pieces/Piece.hpp"
#include "joueur/Joueur.hpp"

#include <algorithm>

ControleurJeu::ControleurJeu(std::shared_ptr<ModeleJeu> modele,
                             std::shared_ptr<VueJeu>    vue,
                             QObject* parent)
    : QObject(parent),
      modele(modele),
      vue(vue),
      caseSelectionnee(nullptr) {
    QObject::connect(vue.get(), &VueJeu::caseCliquee,
                     this,      &ControleurJeu::gererClic);
}

void ControleurJeu::initialiser() {
    modele->demarrer();
    vue->afficher();
}

void ControleurJeu::gererClic(int x, int y) {
    auto plateau  = modele->getPlateau();
    auto caseClic = plateau->getCase(x, y);
    if (!caseClic) return;

    auto joueur = modele->getJoueurActuel();
    if (!joueur) return;
    const Couleur couleurCourant = joueur->getCouleur();

    // ─── 1er clic : selection d'une piece du joueur courant ─────────────
    if (!caseSelectionnee) {
        if (caseClic->estOccupee()
            && caseClic->getPiece()->getCouleur() == couleurCourant) {
            caseSelectionnee = caseClic;
            auto coups = caseClic->getPiece()->getCoupsLegaux(*plateau);
            vue->surligner(caseClic, coups);
        }
        return;
    }

    // ─── Reclic sur la meme case : deselection ──────────────────────────
    if (caseClic == caseSelectionnee) {
        caseSelectionnee = nullptr;
        vue->effacerSurlignage();
        return;
    }

    // ─── Clic sur une autre piece amie : change de selection ────────────
    if (caseClic->estOccupee()
        && caseClic->getPiece()->getCouleur() == couleurCourant) {
        caseSelectionnee = caseClic;
        auto coups = caseClic->getPiece()->getCoupsLegaux(*plateau);
        vue->surligner(caseClic, coups);
        return;
    }

    // ─── Clic ailleurs : tentative de coup ──────────────────────────────
    auto piece = caseSelectionnee->getPiece();
    auto coups = piece->getCoupsLegaux(*plateau);

    bool destinationLegale = std::any_of(coups.begin(), coups.end(),
        [&](const std::shared_ptr<Case>& c) { return c == caseClic; });

    if (destinationLegale) {
        auto coup = std::make_shared<CoupSimple>(caseSelectionnee, caseClic);
        modele->jouerCoup(coup);
        // tourSuivant() et calculerEtat() sont appeles dans jouerCoup.
    } else {
        vue->effacerSurlignage();
    }

    caseSelectionnee = nullptr;
}
