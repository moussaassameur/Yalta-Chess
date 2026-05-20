#include "controller/ControleurJeu.hpp"
#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "view/VueJeu.hpp"
#include "coup/CoupSimple.hpp"
#include "coup/CoupPromotion.hpp"
#include "coup/CoupRoque.hpp"
#include "coup/CoupPriseEnPassant.hpp"
#include "pieces/Roi.hpp"
#include "pieces/Piece.hpp"
#include "pieces/Pion.hpp"
#include "joueur/Joueur.hpp"
#include "joueur/JoueurHumain.hpp"
#include "joueur/JoueurIA.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QTimer>

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
    QObject::connect(vue.get(), &VueJeu::nouvellePartieDemandee,
                     this,      &ControleurJeu::demarrerNouvellePartie);
}

void ControleurJeu::initialiser() {
    // On affiche d'abord la page d'accueil. La partie demarre quand
    // l'utilisateur clique sur "Nouvelle partie" (signal connecte a
    // demarrerNouvellePartie).
    vue->afficher();
}

void ControleurJeu::demarrerNouvellePartie() {
    // Configuration des joueurs via une fenetre modale.
    auto joueurs = demanderConfigJoueurs();
    modele->demarrer(joueurs);
    // Bascule sur le plateau de jeu.
    vue->afficherJeu();
    // Si le premier joueur est une IA, on declenche la pulsation.
    pulserIA();
}

void ControleurJeu::pulserIA() {
    if (modele->jouerUnCoupIASiNecessaire()) {
        // Une IA a joue. On programme le prochain pulse apres un court
        // delai pour laisser Qt redessiner et donner l'impression d'une
        // partie qui se joue, pas d'un bloc instantane.
        constexpr int DELAI_MS = 1500;
        QTimer::singleShot(DELAI_MS, this, &ControleurJeu::pulserIA);
    }
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

        // Prise en passant : pion qui va sur la case "sautee" par un
        // pion adverse ayant fait un bond de 2 au coup precedent.
        } else if (pion && !caseClic->estOccupee()
                   && caseClic == plateau->getCaseEnPassantCible()
                   && plateau->getCaseEnPassantPion()) {
            coup = std::make_shared<CoupPriseEnPassant>(
                caseSelectionnee, caseClic, plateau->getCaseEnPassantPion());

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
        // Apres le coup humain, si le joueur suivant est une IA, on
        // declenche la pulsation pour qu'elle joue (et la suivante etc.).
        pulserIA();
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

std::vector<std::shared_ptr<Joueur>> ControleurJeu::demanderConfigJoueurs() {
    QDialog dialog(vue.get());
    dialog.setWindowTitle("Yalta Chess - Nouvelle partie");
    dialog.setModal(true);

    auto* layout = new QVBoxLayout(&dialog);
    layout->addWidget(new QLabel("Configuration des joueurs :"));

    // Grille : nom | type (combo) | profondeur IA (spinbox)
    auto* grille = new QGridLayout();
    grille->addWidget(new QLabel("Joueur"),     0, 0);
    grille->addWidget(new QLabel("Type"),       0, 1);
    grille->addWidget(new QLabel("Profondeur"), 0, 2);

    struct Ligne {
        QComboBox* combo;
        QSpinBox*  spin;
        QString    nom;
        Couleur    couleur;
    };
    std::vector<Ligne> lignes = {
        {nullptr, nullptr, "BLANC", Couleur::BLANC},
        {nullptr, nullptr, "ROUGE", Couleur::ROUGE},
        {nullptr, nullptr, "NOIR",  Couleur::NOIR},
    };

    for (int i = 0; i < (int)lignes.size(); ++i) {
        grille->addWidget(new QLabel(lignes[i].nom), i + 1, 0);

        auto* combo = new QComboBox(&dialog);
        combo->addItem("Humain");
        combo->addItem("IA");
        grille->addWidget(combo, i + 1, 1);

        auto* spin = new QSpinBox(&dialog);
        spin->setRange(1, 4);
        spin->setValue(3);
        spin->setEnabled(false);  // active uniquement si IA
        grille->addWidget(spin, i + 1, 2);

        // Active le spinbox uniquement quand "IA" est selectionne.
        QObject::connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [spin](int idx) { spin->setEnabled(idx == 1); });

        lignes[i].combo = combo;
        lignes[i].spin  = spin;
    }

    layout->addLayout(grille);

    auto* btnOK = new QPushButton("Commencer la partie", &dialog);
    layout->addWidget(btnOK);
    QObject::connect(btnOK, &QPushButton::clicked, &dialog, &QDialog::accept);

    std::vector<std::shared_ptr<Joueur>> joueurs;
    if (dialog.exec() == QDialog::Accepted) {
        for (const auto& l : lignes) {
            const std::string nom = ("Joueur " + l.nom).toStdString();
            if (l.combo->currentIndex() == 1) {
                joueurs.push_back(std::make_shared<JoueurIA>(
                    nom, l.couleur, l.spin->value(), 4));
            } else {
                joueurs.push_back(std::make_shared<JoueurHumain>(nom, l.couleur));
            }
        }
    } else {
        // Annulation : 3 humains par defaut.
        for (const auto& l : lignes) {
            joueurs.push_back(std::make_shared<JoueurHumain>(
                ("Joueur " + l.nom).toStdString(), l.couleur));
        }
    }
    return joueurs;
}
