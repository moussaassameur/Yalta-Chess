#ifndef HISTORIQUE_HPP
#define HISTORIQUE_HPP

#include <deque>
#include <memory>

class Coup;

/**
 * @file Historique.hpp
 * @brief Pile des coups joues + pile des coups annules (undo/redo).
 *
 * Cette classe est une pure structure de donnees : elle ne fait
 * qu'empiler et depiler des objets Coup. Elle n'execute PAS les coups
 * elle-meme -- c'est au composant qui s'en sert (Modele de jeu, tests)
 * d'appeler coup->executer() ou coup->annuler().
 *
 * Logique :
 *   - ajouter(c) : empile c dans coupsJoues. Vide la pile coupsAnnules
 *     (consequence de la regle : un nouveau coup invalide les redos).
 *   - annulerDernier() : retire le sommet de coupsJoues, l'empile dans
 *     coupsAnnules, le retourne (caller fait coup->annuler()).
 *   - refaire() : retire le sommet de coupsAnnules, l'empile dans
 *     coupsJoues, le retourne (caller fait coup->executer()).
 */
class Historique {
public:
    Historique() = default;
    ~Historique() = default;

    /**
     * @brief Ajoute un coup deja execute dans la pile.
     * @param coup Coup a memoriser.
     *
     * Effet de bord : la pile des coups annules est videe (on ne peut
     * pas refaire apres avoir joue un coup nouveau).
     */
    void ajouter(std::shared_ptr<Coup> coup);

    /**
     * @brief Retire le dernier coup joue et le bascule dans coupsAnnules.
     * @return Le coup retire, ou nullptr si l'historique etait vide.
     *
     * Note : cette methode ne defait pas le coup. C'est a l'appelant
     * d'invoquer coup->annuler(plateau).
     */
    std::shared_ptr<Coup> annulerDernier();

    /**
     * @brief Retire le dernier coup annule et le replace dans coupsJoues.
     * @return Le coup retire, ou nullptr si rien a refaire.
     *
     * Comme annulerDernier(), il appartient a l'appelant d'executer
     * le coup avec coup->executer(plateau).
     */
    std::shared_ptr<Coup> refaire();

    /// @return Le dernier coup joue sans le retirer, ou nullptr.
    std::shared_ptr<Coup> dernier() const;

    /// @return Nombre de coups dans la pile coupsJoues.
    int taille() const;

    /// @return true si aucun coup n'a ete joue.
    bool estVide() const;

    /// @return Nombre de coups annules disponibles pour refaire().
    int tailleRedo() const;

private:
    std::deque<std::shared_ptr<Coup>> coupsJoues;
    std::deque<std::shared_ptr<Coup>> coupsAnnules;
};

#endif // HISTORIQUE_HPP
