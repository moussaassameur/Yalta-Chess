#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "model/Historique.hpp"
#include "coup/Coup.hpp"
#include "joueur/JoueurHumain.hpp"
#include "observer/Observateur.hpp"
#include "pieces/Piece.hpp"

#include <algorithm>

/**
 * @file ModeleJeu.cpp
 * @brief Implementation du modele de jeu + pattern Observer integre.
 */

ModeleJeu::ModeleJeu()
    : plateau(std::make_shared<Plateau>()),
      historique(std::make_shared<Historique>()),
      joueurActuel(nullptr),
      etat(EtatPartie::EN_COURS) {
}

void ModeleJeu::demarrer() {
    // Cree les 3 joueurs humains avec leurs couleurs.
    joueurs.clear();
    joueurs.push_back(std::make_shared<JoueurHumain>("Joueur Blanc", Couleur::BLANC));
    joueurs.push_back(std::make_shared<JoueurHumain>("Joueur Rouge", Couleur::ROUGE));
    joueurs.push_back(std::make_shared<JoueurHumain>("Joueur Noir",  Couleur::NOIR));
    joueurActuel = joueurs[0];

    // Plateau et historique remis a zero, pieces placees au depart.
    plateau    = std::make_shared<Plateau>();
    plateau->initialiserPiecesYalta();
    historique = std::make_shared<Historique>();
    etat       = EtatPartie::EN_COURS;

    notifier();
}

void ModeleJeu::jouerCoup(std::shared_ptr<Coup> coup) {
    if (!coup || !coup->estValide(*plateau)) return;

    coup->executer(*plateau);
    historique->ajouter(coup);

    tourSuivant();
    calculerEtat();
    notifier();
}

void ModeleJeu::calculerEtat() {
    // Verifie le joueur actuel : echec, mat, pat. Si elimine, passe au suivant.
    while (joueurActuel && !joueurActuel->getEstElimine()) {
        const Couleur c = joueurActuel->getCouleur();
        const bool enEchec = plateau->estEnEchec(c);

        // Cherche au moins un coup legal.
        bool aUnCoupLegal = false;
        for (const auto& p : plateau->getPiecesDeCouleur(c)) {
            if (!p->getCoupsLegaux(*plateau).empty()) {
                aUnCoupLegal = true;
                break;
            }
        }

        if (!aUnCoupLegal) {
            etat = enEchec ? EtatPartie::ECHEC_ET_MAT : EtatPartie::PAT;
            joueurActuel->eliminer();
            supprimerPieces(c);

            // Compte les joueurs encore actifs.
            int nActifs = 0;
            for (const auto& j : joueurs)
                if (!j->getEstElimine()) ++nActifs;
            if (nActifs <= 1) return; // fin de partie

            tourSuivant();
            // Recommence la boucle pour verifier le prochain joueur.
        } else {
            etat = enEchec ? EtatPartie::ECHEC : EtatPartie::EN_COURS;
            return;
        }
    }
}

void ModeleJeu::supprimerPieces(Couleur c) {
    for (const auto& p : plateau->getPiecesDeCouleur(c)) {
        auto pos = p->getPosition();
        if (pos) pos->retirerPiece();
        p->capturer();
    }
}

void ModeleJeu::tourSuivant() {
    if (joueurs.empty() || !joueurActuel) return;

    // Cherche l'index du joueur courant.
    int idx = 0;
    for (int i = 0; i < (int)joueurs.size(); ++i) {
        if (joueurs[i] == joueurActuel) { idx = i; break; }
    }

    // Cycle sur les joueurs suivants en sautant les elimines.
    for (int i = 1; i <= (int)joueurs.size(); ++i) {
        const int prochain = (idx + i) % (int)joueurs.size();
        if (!joueurs[prochain]->getEstElimine()) {
            joueurActuel = joueurs[prochain];
            break;
        }
    }

    notifier();
}

void ModeleJeu::annulerDernierCoup() {
    auto coup = historique->annulerDernier();
    if (!coup) return;
    coup->annuler(*plateau);
    notifier();
}

// ─── Pattern Observer ────────────────────────────────────────────────────

void ModeleJeu::attacher(std::shared_ptr<Observateur> obs) {
    if (!obs) return;
    observateurs.push_back(obs);
}

void ModeleJeu::detacher(std::shared_ptr<Observateur> obs) {
    observateurs.erase(
        std::remove(observateurs.begin(), observateurs.end(), obs),
        observateurs.end());
}

void ModeleJeu::notifier() {
    // Copie du vecteur pour eviter qu'un observateur qui se detache
    // depuis mettreAJour() invalide l'iteration.
    auto snapshot = observateurs;
    for (auto& obs : snapshot) {
        if (obs) obs->mettreAJour(*this);
    }
}

// ─── Accesseurs ──────────────────────────────────────────────────────────

EtatPartie                  ModeleJeu::getEtat()         const { return etat; }
std::shared_ptr<Joueur>     ModeleJeu::getJoueurActuel() const { return joueurActuel; }
std::shared_ptr<Plateau>    ModeleJeu::getPlateau()      const { return plateau; }
std::shared_ptr<Historique> ModeleJeu::getHistorique()   const { return historique; }
const std::vector<std::shared_ptr<Joueur>>& ModeleJeu::getJoueurs() const { return joueurs; }
