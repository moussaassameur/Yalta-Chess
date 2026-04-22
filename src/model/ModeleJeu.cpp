#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "model/Historique.hpp"
#include "observer/Observateur.hpp"
#include "joueur/Joueur.hpp"
#include "joueur/JoueurHumain.hpp"
#include "coup/Coup.hpp"
#include <algorithm>

ModeleJeu::ModeleJeu()
    : joueurActuel(nullptr),
      plateau(std::make_shared<Plateau>()),
      etat(EtatPartie::EN_COURS),
      historique(std::make_shared<Historique>()) {
}

void ModeleJeu::demarrer() {
    // on cree les 3 joueurs avec leurs couleurs
    joueurs.clear();
    joueurs.push_back(std::make_shared<JoueurHumain>("Joueur Blanc", Couleur::BLANC));
    joueurs.push_back(std::make_shared<JoueurHumain>("Joueur Noir",  Couleur::NOIR));
    joueurs.push_back(std::make_shared<JoueurHumain>("Joueur Rouge", Couleur::ROUGE));

    // le blanc commence toujours en premier
    joueurActuel = joueurs[0];

    // on remet le plateau et l historique a zero
    plateau = std::make_shared<Plateau>();
    historique = std::make_shared<Historique>();
    etat = EtatPartie::EN_COURS;

    notifierObservateurs();
}

void ModeleJeu::jouerCoup(std::shared_ptr<Coup> coup) {
    if (!coup || !coup->estValide(*plateau)) return;

    coup->executer(*plateau);
    historique->ajouter(coup);
    etat = verifierEtat();
    notifierObservateurs();
}

void ModeleJeu::tourSuivant() {
    // on cherche l index du joueur actuel
    int idx = 0;
    for (int i = 0; i < (int)joueurs.size(); i++) {
        if (joueurs[i] == joueurActuel) {
            idx = i;
            break;
        }
    }

    // on passe au joueur suivant en sautant les joueurs elimines
    // on fait un cycle sur les 3 joueurs
    for (int i = 1; i <= (int)joueurs.size(); i++) {
        int prochain = (idx + i) % (int)joueurs.size();
        if (!joueurs[prochain]->getEstElimine()) {
            joueurActuel = joueurs[prochain];
            return;
        }
    }
}

EtatPartie ModeleJeu::verifierEtat() {
    if (!joueurActuel) return EtatPartie::EN_COURS;

    Couleur couleur = joueurActuel->getCouleur();
    bool enEchec   = plateau->estEnEchec(couleur);
    bool aDesCoups = plateau->aDesCoupsLegaux(couleur);

    if (enEchec && !aDesCoups) {
        // le joueur est mat, on l elimine
        joueurActuel->eliminer();
        return EtatPartie::ECHEC_ET_MAT;
    }
    if (!enEchec && !aDesCoups) {
        return EtatPartie::PAT;
    }
    if (enEchec) {
        return EtatPartie::ECHEC;
    }
    return EtatPartie::EN_COURS;
}

void ModeleJeu::annulerDernierCoup() {
    auto coup = historique->annulerDernier();
    if (coup) {
        coup->annuler(*plateau);
        notifierObservateurs();
    }
}

void ModeleJeu::ajouterObservateur(std::shared_ptr<Observateur> o) {
    observateurs.push_back(o);
}

void ModeleJeu::retirerObservateur(std::shared_ptr<Observateur> o) {
    observateurs.erase(
        std::remove(observateurs.begin(), observateurs.end(), o),
        observateurs.end()
    );
}

void ModeleJeu::notifierObservateurs() {
    for (auto& obs : observateurs) {
        obs->mettreAJour(*this);
    }
}

EtatPartie                  ModeleJeu::getEtat()         const { return etat; }
std::shared_ptr<Joueur>     ModeleJeu::getJoueurActuel() const { return joueurActuel; }
std::shared_ptr<Plateau>    ModeleJeu::getPlateau()      const { return plateau; }
std::shared_ptr<Historique> ModeleJeu::getHistorique()   const { return historique; }
