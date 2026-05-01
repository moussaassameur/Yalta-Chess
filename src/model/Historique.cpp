#include "model/Historique.hpp"
#include "coup/Coup.hpp"

void Historique::ajouter(std::shared_ptr<Coup> coup) {
    if (!coup) return;
    coupsJoues.push_back(coup);
    // Regle : un nouveau coup ferme l'arborescence des redos.
    coupsAnnules.clear();
}

std::shared_ptr<Coup> Historique::annulerDernier() {
    if (coupsJoues.empty()) return nullptr;
    auto coup = coupsJoues.back();
    coupsJoues.pop_back();
    coupsAnnules.push_back(coup);
    return coup;
}

std::shared_ptr<Coup> Historique::refaire() {
    if (coupsAnnules.empty()) return nullptr;
    auto coup = coupsAnnules.back();
    coupsAnnules.pop_back();
    coupsJoues.push_back(coup);
    return coup;
}

std::shared_ptr<Coup> Historique::dernier() const {
    if (coupsJoues.empty()) return nullptr;
    return coupsJoues.back();
}

int  Historique::taille()      const { return static_cast<int>(coupsJoues.size()); }
int  Historique::tailleRedo()  const { return static_cast<int>(coupsAnnules.size()); }
bool Historique::estVide()     const { return coupsJoues.empty(); }
