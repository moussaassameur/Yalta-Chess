#include "controller/ControleurJeu.hpp"
#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "view/VueJeu.hpp"
#include "coup/CoupSimple.hpp"
#include "pieces/Piece.hpp"
#include "joueur/Joueur.hpp"

ControleurJeu::ControleurJeu(std::shared_ptr<ModeleJeu> modele,
                             std::shared_ptr<VueJeu>    vue,
                             QObject* parent)
    : QObject(parent), modele(modele), vue(vue),
      caseSelectionnee(nullptr) {
    // branchement du signal clic vue -> slot controleur
    QObject::connect(vue.get(), &VueJeu::caseCliquee,
                     this,      &ControleurJeu::gererClic);
}

void ControleurJeu::initialiser() {
    modele->demarrer();
    vue->afficher();
}

void ControleurJeu::gererClic(int q, int r) {
    auto plateau  = modele->getPlateau();
    auto caseClic = plateau->getCase(q, r);
    if (!caseClic) return;

    auto couleurJoueur = modele->getJoueurActuel()->getCouleur();

    // premier clic : on selectionne une piece du joueur actuel
    if (!caseSelectionnee) {
        if (caseClic->estOccupee()) {
            auto piece = caseClic->getPiece();
            if (piece->getCouleur() == couleurJoueur) {
                caseSelectionnee = caseClic;
                vue->surligner(caseClic, piece->getDeplacements(*plateau));
            }
        }
        return;
    }

    // reclic sur la meme case : on annule la selection
    if (caseClic == caseSelectionnee) {
        caseSelectionnee = nullptr;
        vue->effacerSurlignage();
        return;
    }

    // clic sur une autre piece a soi : on change de selection
    if (caseClic->estOccupee() &&
        caseClic->getPiece()->getCouleur() == couleurJoueur) {
        caseSelectionnee = caseClic;
        vue->surligner(caseClic, caseClic->getPiece()->getDeplacements(*plateau));
        return;
    }

    // sinon : tentative de coup
    auto coup = construireCoup(caseSelectionnee, caseClic);
    caseSelectionnee = nullptr;

    if (coup && coup->estValide(*plateau)) {
        modele->jouerCoup(coup);
        modele->tourSuivant();
        // mettreAJour() cote vue efface deja le surlignage
    } else {
        vue->effacerSurlignage();
    }
}

void ControleurJeu::gererTour() {
    // appele automatiquement apres chaque coup
    // si c est le tour de l IA on lance le calcul
    // TODO : detecter JoueurIA et appeler jouerTour()
}

std::shared_ptr<Coup> ControleurJeu::construireCoup(std::shared_ptr<Case> depart,
                                                     std::shared_ptr<Case> arrivee) {
    if (!depart || !arrivee) return nullptr;
    auto piece = depart->getPiece();
    if (!piece) return nullptr;

    // pour l instant on cree toujours un CoupSimple
    // les coups speciaux (roque, promotion, prise en passant) seront ajoutes plus tard
    return std::make_shared<CoupSimple>(depart, arrivee, piece);
}
