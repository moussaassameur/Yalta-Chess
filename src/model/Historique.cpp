#include "model/Historique.hpp"
#include "coup/Coup.hpp"

void Historique::ajouter(std::shared_ptr<Coup> coup) {
    // on empile le coup joue
    coupsJoues.push_back(coup);
    // quand on joue un nouveau coup on vide les coups annules
    // parce que on peut plus refaire apres un nouveau coup
    coupsAnnules.clear();
}

std::shared_ptr<Coup> Historique::annulerDernier() {
    // si ya rien a annuler on retourne nullptr
    if (coupsJoues.empty()) return nullptr;

    // on recupere le dernier coup joue
    auto coup = coupsJoues.back();
    coupsJoues.pop_back();

    // on le met dans la pile des annules pour pouvoir refaire
    coupsAnnules.push_back(coup);

    return coup;
}

std::shared_ptr<Coup> Historique::refaire() {
    // si ya rien a refaire on retourne nullptr
    if (coupsAnnules.empty()) return nullptr;

    // on recupere le dernier coup annule
    auto coup = coupsAnnules.back();
    coupsAnnules.pop_back();

    // on le remet dans les coups joues
    coupsJoues.push_back(coup);

    return coup;
}

std::shared_ptr<Coup> Historique::dernier() const {
    // on retourne le dernier coup sans le depiler
    if (coupsJoues.empty()) return nullptr;
    return coupsJoues.back();
}

int Historique::taille() const {
    return static_cast<int>(coupsJoues.size());
}

bool Historique::estVide() const {
    return coupsJoues.empty();
}
