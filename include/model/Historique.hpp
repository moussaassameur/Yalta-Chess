#ifndef HISTORIQUE_HPP
#define HISTORIQUE_HPP

#include <deque>
#include <memory>

class Coup;

// Stocke les coups joués et annulés — utilisé par ModeleJeu pour undo/redo
class Historique {
private:
    std::deque<std::shared_ptr<Coup>> coupsJoues;   // pile LIFO des coups joués
    std::deque<std::shared_ptr<Coup>> coupsAnnules; // pile LIFO des coups annulés (redo)

public:
    Historique() = default;
    ~Historique() = default;

    void ajouter(std::shared_ptr<Coup> coup);
    std::shared_ptr<Coup> annulerDernier();  // dépile et retourne le dernier coup joué
    std::shared_ptr<Coup> refaire();         // rejoue le dernier coup annulé
    std::shared_ptr<Coup> dernier() const;   // consulte sans dépiler
    int taille() const;
    bool estVide() const;
};

#endif // HISTORIQUE_HPP
