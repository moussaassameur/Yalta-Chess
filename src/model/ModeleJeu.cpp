#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "model/Historique.hpp"
#include "coup/Coup.hpp"
#include "coup/CoupSimple.hpp"
#include "joueur/Joueur.hpp"
#include "joueur/JoueurHumain.hpp"
#include "observer/Observateur.hpp"
#include "pieces/Piece.hpp"

#include <algorithm>
#include <cstdlib>

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
    std::vector<std::shared_ptr<Joueur>> j;
    j.push_back(std::make_shared<JoueurHumain>("Joueur Blanc", Couleur::BLANC));
    j.push_back(std::make_shared<JoueurHumain>("Joueur Rouge", Couleur::ROUGE));
    j.push_back(std::make_shared<JoueurHumain>("Joueur Noir",  Couleur::NOIR));
    demarrer(j);
}

void ModeleJeu::demarrer(const std::vector<std::shared_ptr<Joueur>>& joueursPersos) {
    joueurs      = joueursPersos;
    joueurActuel = joueurs.empty() ? nullptr : joueurs[0];

    plateau    = std::make_shared<Plateau>();
    plateau->initialiserPiecesYalta();
    historique = std::make_shared<Historique>();
    etat       = EtatPartie::EN_COURS;
    historiqueEnPassant.clear();
    enPassantTTL = 0;

    notifier();
    // C'est le ControleurJeu qui declenchera la chaine IA via QTimer
    // pour permettre a l'UI de redessiner entre chaque coup.
}

void ModeleJeu::jouerCoup(std::shared_ptr<Coup> coup) {
    if (!coup || !coup->estValide(*plateau)) return;

    // Sauvegarde l'etat en passant avant l'execution, pour pouvoir
    // restaurer lors d'un annulerDernierCoup().
    historiqueEnPassant.push_back({plateau->getCaseEnPassantCible(),
                                   plateau->getCaseEnPassantPion(),
                                   enPassantTTL});

    coup->executer(*plateau);
    historique->ajouter(coup);

    // Met a jour la fenetre de prise en passant.
    //   - Si le coup est un bond de pion de 2 cases : on (re)ouvre le droit
    //     pour les 2 adversaires -> ttl = 2.
    //   - Sinon : on decremente le ttl ; a 0, le droit expire.
    // A 3 joueurs, ttl = 2 garantit que le joueur immediatement apres ET
    // le joueur d'apres peuvent tous deux capturer en passant.
    bool estBond = false;
    auto coupSimple = std::dynamic_pointer_cast<CoupSimple>(coup);
    if (coupSimple) {
        auto piece = coupSimple->getPiece();
        if (piece && piece->getType() == "Pion") {
            auto dep = coupSimple->getDepart();
            auto arr = coupSimple->getArrivee();
            if (dep && arr) {
                const int adx = std::abs(arr->getX() - dep->getX());
                const int ady = std::abs(arr->getY() - dep->getY());
                if (adx == 2 || ady == 2) {
                    auto sautee = plateau->getCase((dep->getX() + arr->getX()) / 2,
                                                   (dep->getY() + arr->getY()) / 2);
                    if (sautee) {
                        plateau->setEnPassant(sautee, arr);
                        enPassantTTL = 2;
                        estBond = true;
                    }
                }
            }
        }
    }
    if (!estBond && enPassantTTL > 0) {
        --enPassantTTL;
        if (enPassantTTL == 0) plateau->clearEnPassant();
    }

    tourSuivant();
    calculerEtat();
    notifier();
    // La chaine IA est declenchee par ControleurJeu via QTimer entre
    // chaque coup, pour que l'UI puisse se rafraichir.
}

void ModeleJeu::calculerEtat() {
    while (joueurActuel && !joueurActuel->getEstElimine()) {
        const Couleur c = joueurActuel->getCouleur();
        const bool mat = plateau->estMat(c);
        const bool pat = plateau->estPat(c);

        if (mat || pat) {
            etat = mat ? EtatPartie::ECHEC_ET_MAT : EtatPartie::PAT;

            if (mat) {
                // Determination du gagnant selon les regles Yalta : le
                // premier survivant (dans l'ordre du tour) qui met le roi
                // mate en echec gagne 1 point, l'autre survivant prend 1/2.
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
            etat = plateau->estEnEchec(c) ? EtatPartie::ECHEC
                                          : EtatPartie::EN_COURS;
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

    // Restaure l'etat de prise en passant qui etait actif avant ce coup.
    if (!historiqueEnPassant.empty()) {
        auto prev = historiqueEnPassant.back();
        historiqueEnPassant.pop_back();
        enPassantTTL = prev.ttl;
        if (prev.cible) plateau->setEnPassant(prev.cible, prev.pion);
        else            plateau->clearEnPassant();
    }

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

// Pattern Observer 

void ModeleJeu::attacher(std::shared_ptr<Observateur> obs) {
    if (!obs) return;
    observateurs.push_back(obs);
}

void ModeleJeu::detacher(std::shared_ptr<Observateur> obs) {
    observateurs.erase(
        std::remove(observateurs.begin(), observateurs.end(), obs),
        observateurs.end());
}

bool ModeleJeu::jouerUnCoupIASiNecessaire() {
    if (etat != EtatPartie::EN_COURS && etat != EtatPartie::ECHEC) return false;
    if (!joueurActuel || joueurActuel->getEstElimine()) return false;

    // jouerTour retourne nullptr pour un humain (il attend l'UI),
    // ou un Coup pour une IA (calcule par son MinMax).
    auto coup = joueurActuel->jouerTour(*plateau);
    if (!coup) return false;

    jouerCoup(coup);
    return true;
}

void ModeleJeu::notifier() {
    // Copie du vecteur pour eviter qu'un observateur qui se detache
    // depuis mettreAJour() invalide l'iteration.
    auto snapshot = observateurs;
    for (auto& obs : snapshot) {
        if (obs) obs->mettreAJour(*this);
    }
}

// Accesseurs 

EtatPartie                  ModeleJeu::getEtat()         const { return etat; }
std::shared_ptr<Joueur>     ModeleJeu::getJoueurActuel() const { return joueurActuel; }
std::shared_ptr<Plateau>    ModeleJeu::getPlateau()      const { return plateau; }
std::shared_ptr<Historique> ModeleJeu::getHistorique()   const { return historique; }
const std::vector<std::shared_ptr<Joueur>>& ModeleJeu::getJoueurs() const { return joueurs; }
