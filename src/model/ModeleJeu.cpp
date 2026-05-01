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
    while (joueurActuel && !joueurActuel->getEstElimine()) {
        const Couleur c = joueurActuel->getCouleur();
        const bool enEchec = plateau->estEnEchec(c);

        bool aUnCoupLegal = false;
        for (const auto& p : plateau->getPiecesDeCouleur(c)) {
            if (!p->getCoupsLegaux(*plateau).empty()) { aUnCoupLegal = true; break; }
        }

        if (!aUnCoupLegal) {
            etat = enEchec ? EtatPartie::ECHEC_ET_MAT : EtatPartie::PAT;

            if (enEchec) {
                // ── Determination du gagnant selon les regles Yalta ──────────
                // On parcourt les survivants dans l'ordre apres le joueur elimine.
                // Le premier qui peut capturer le roi gagne (1 pt).
                // L'autre survivant prend 1/2 pt.
                // Si les deux checkent, le premier dans l'ordre gagne.
                int idxCourant = 0;
                for (int i = 0; i < (int)joueurs.size(); ++i)
                    if (joueurs[i]->getCouleur() == c) { idxCourant = i; break; }

                std::vector<std::shared_ptr<Joueur>> survivants;
                for (int i = 1; i <= (int)joueurs.size(); ++i) {
                    int idx = (idxCourant + i) % (int)joueurs.size();
                    if (!joueurs[idx]->getEstElimine())
                        survivants.push_back(joueurs[idx]);
                }

                std::shared_ptr<Joueur> gagnant = nullptr;
                std::shared_ptr<Joueur> tiers   = nullptr;

                // Cherche le premier checkeur dans l'ordre.
                for (const auto& surv : survivants) {
                    if (!gagnant && plateau->estEnEchecPar(c, surv->getCouleur()))
                        gagnant = surv;
                    else if (!tiers)
                        tiers = surv;
                }
                // Si gagnant non trouve (peut arriver si aucun ne checke directement),
                // le premier survivant est considere gagnant par defaut.
                if (!gagnant && !survivants.empty()) gagnant = survivants[0];
                if (!tiers && survivants.size() >= 2) tiers = survivants[1];

                if (gagnant) gagnant->ajouterScore(1.0);
                if (tiers)   tiers->ajouterScore(0.5);
            }

            joueurActuel->eliminer();
            supprimerPieces(c);

            int nActifs = 0;
            for (const auto& j : joueurs)
                if (!j->getEstElimine()) ++nActifs;

            if (nActifs <= 1) {
                etat = EtatPartie::VICTOIRE;
                return;
            }
            tourSuivant();

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

    // Revenir au joueur precedent (sens inverse du tour).
    if (!joueurs.empty() && joueurActuel) {
        int idx = 0;
        for (int i = 0; i < (int)joueurs.size(); ++i)
            if (joueurs[i] == joueurActuel) { idx = i; break; }

        for (int i = 1; i <= (int)joueurs.size(); ++i) {
            const int precedent = (idx - i + (int)joueurs.size()) % (int)joueurs.size();
            if (!joueurs[precedent]->getEstElimine()) {
                joueurActuel = joueurs[precedent];
                break;
            }
        }
    }

    etat = EtatPartie::EN_COURS;
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
