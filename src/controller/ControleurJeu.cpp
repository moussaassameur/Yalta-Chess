#include "controller/ControleurJeu.hpp"
#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "view/VueJeu.hpp"
#include "coup/CoupSimple.hpp"
#include "coup/CoupPromotion.hpp"
#include "coup/CoupRoque.hpp"
#include "pieces/Roi.hpp"
#include "pieces/Piece.hpp"
#include "pieces/Pion.hpp"
#include "joueur/Joueur.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

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
    QObject::connect(vue.get(), &VueJeu::annulerDemande,
                     this,      &ControleurJeu::annulerCoup);
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
        std::shared_ptr<Coup> coup;

        // Promotion : pion qui atteint le back rank ennemi.
        auto pion = std::dynamic_pointer_cast<Pion>(piece);
        if (pion && Pion::estCaseDePromotion(caseClic, pion->getCouleur())) {
            std::string type = demanderPromotion();
            coup = std::make_shared<CoupPromotion>(caseSelectionnee, caseClic, type);

        // Roque : roi se deplace de 2 cases en ligne droite dans le meme sextant.
        } else if (std::dynamic_pointer_cast<Roi>(piece)
                   && caseSelectionnee->getSextant() == caseClic->getSextant()) {
            const int dx = caseClic->getX() - caseSelectionnee->getX();
            const int dy = caseClic->getY() - caseSelectionnee->getY();
            const bool estRoque = (dx == 0 && std::abs(dy) == 2)
                                || (std::abs(dx) == 2 && dy == 0);
            if (estRoque) {
                // Direction de la tour (un pas de plus dans le meme sens).
                const int sx = (dx > 0) - (dx < 0);
                const int sy = (dy > 0) - (dy < 0);
                auto tourDep = plateau->getCase(caseClic->getX()         + sx,
                                                caseClic->getY()         + sy);
                auto tourArr = plateau->getCase(caseSelectionnee->getX() + sx,
                                                caseSelectionnee->getY() + sy);
                if (tourDep && tourArr)
                    coup = std::make_shared<CoupRoque>(
                        caseSelectionnee, caseClic, tourDep, tourArr);
            }
            if (!coup)
                coup = std::make_shared<CoupSimple>(caseSelectionnee, caseClic);

        } else {
            coup = std::make_shared<CoupSimple>(caseSelectionnee, caseClic);
        }
        modele->jouerCoup(coup);
    } else {
        vue->effacerSurlignage();
    }

    caseSelectionnee = nullptr;
}

void ControleurJeu::annulerCoup() {
    caseSelectionnee = nullptr;
    vue->effacerSurlignage();
    modele->annulerDernierCoup();
}

std::string ControleurJeu::demanderPromotion() {
    QDialog dialog(vue.get());
    dialog.setWindowTitle("Promotion");
    dialog.setModal(true);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->addWidget(new QLabel("Choisissez la piece de promotion :"));

    QHBoxLayout* boutons = new QHBoxLayout();
    std::string choix = "Reine";

    auto ajouterBouton = [&](const QString& label, const std::string& type) {
        auto* btn = new QPushButton(label, &dialog);
        boutons->addWidget(btn);
        QObject::connect(btn, &QPushButton::clicked, [&, type]() {
            choix = type;
            dialog.accept();
        });
    };

    ajouterBouton("♛ Reine",    "Reine");
    ajouterBouton("♜ Tour",     "Tour");
    ajouterBouton("♝ Fou",      "Fou");
    ajouterBouton("♞ Cavalier", "Cavalier");

    layout->addLayout(boutons);
    dialog.exec();
    return choix;
}
