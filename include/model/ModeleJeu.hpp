#ifndef MODELE_JEU_HPP
#define MODELE_JEU_HPP

#include "model/EtatPartie.hpp"
#include "model/Couleur.hpp"
#include <memory>
#include <utility>
#include <vector>

class Plateau;
class Historique;
class Coup;
class Joueur;
class Observateur;
class Case;

/**
 * @file ModeleJeu.hpp
 * @brief Modele du jeu Yalta -- coeur du M dans MVC.
 *
 * ModeleJeu encapsule tout l'etat du jeu (plateau, joueurs, historique
 * des coups, etat de partie) et expose une API de haut niveau pour
 * jouer (jouerCoup, tourSuivant, annulerDernierCoup).
 *
 * Pattern utilise : OBSERVER. ModeleJeu maintient une liste
 * d'observateurs et les notifie a chaque changement d'etat. La methode
 * privee notifier() est appelee a la fin de jouerCoup, tourSuivant et
 * annulerDernierCoup. Les observateurs (Vue, Alerte d'echec, ...) sont
 * enregistres via attacher() / detacher().
 *
 * En Phase 5, ModeleJeu :
 *   - place 3 JoueurHumain (BLANC, ROUGE, NOIR) au demarrage,
 *   - n'initialise PAS les pieces sur le plateau (sera fait en Phase 6
 *     pour pouvoir lancer une vraie partie),
 *   - ne calcule PAS encore les etats ECHEC / ECHEC_ET_MAT / PAT (sera
 *     ajoute en Phase 6 quand les pieces seront placees).
 */
class ModeleJeu {
public:
    ModeleJeu();
    ~ModeleJeu() = default;

    /// @brief Initialise une nouvelle partie avec 3 joueurs humains.
    void demarrer();

    /**
     * @brief Initialise une nouvelle partie avec des joueurs fournis.
     * @param joueursPersos Les 3 joueurs (mix Humain/IA possible),
     *        dans l'ordre BLANC, ROUGE, NOIR (le 1er joue en premier).
     */
    void demarrer(const std::vector<std::shared_ptr<Joueur>>& joueursPersos);

    /**
     * @brief Joue un coup s'il est valide, l'ajoute a l'historique et
     *        notifie les observateurs.
     * @param coup Coup a jouer (CoupSimple, CoupRoque, CoupPromotion...).
     */
    void jouerCoup(std::shared_ptr<Coup> coup);

    /**
     * @brief Passe au joueur suivant (en sautant les joueurs elimines)
     *        et notifie les observateurs.
     */
    void tourSuivant();

    /**
     * @brief Annule le dernier coup joue (si possible) et notifie.
     */
    void annulerDernierCoup();

    /// @name Pattern Observer -- enregistrement / notification
    /// @{

    /// @brief Ajoute un observateur a la liste de diffusion.
    void attacher(std::shared_ptr<Observateur> obs);

    /// @brief Retire un observateur de la liste.
    void detacher(std::shared_ptr<Observateur> obs);

    /// @}

    /// @name Accesseurs en lecture seule (utiles aux observateurs)
    /// @{
    EtatPartie                  getEtat()         const;
    std::shared_ptr<Joueur>     getJoueurActuel() const;
    std::shared_ptr<Plateau>    getPlateau()      const;
    std::shared_ptr<Historique> getHistorique()   const;
    const std::vector<std::shared_ptr<Joueur>>& getJoueurs() const;
    /// @}

private:
    std::shared_ptr<Plateau>             plateau;
    std::shared_ptr<Historique>          historique;
    std::vector<std::shared_ptr<Joueur>> joueurs;
    std::shared_ptr<Joueur>              joueurActuel;
    EtatPartie                           etat;

    /// Liste des observateurs enregistres.
    std::vector<std::shared_ptr<Observateur>> observateurs;

    /// Pile des etats en passant avant chaque coup, pour pouvoir
    /// restaurer correctement lors d'un annulerDernierCoup().
    std::vector<std::pair<std::shared_ptr<Case>,
                          std::shared_ptr<Case>>> historiqueEnPassant;

    /// Drapeau anti-recursion : true pendant l'execution d'une chaine IA,
    /// empeche jouerCoup() de relancer jouerSiIA() recursivement.
    bool dansChaineIA = false;

    /// @brief Diffuse une notification a tous les observateurs.
    void notifier();

    /// @brief Calcule l'etat du joueur actuel (echec/mat/pat) et elimine
    ///        si necessaire. Appele apres chaque coup joue.
    void calculerEtat();

    /// @brief Retire toutes les pieces d'une couleur du plateau.
    void supprimerPieces(Couleur c);

    /**
     * @brief Si le joueur actuel est une IA (jouerTour retourne un Coup),
     *        le joue automatiquement. Sinon (humain), ne fait rien.
     *
     * Comme jouerCoup() rappelle cette methode a la fin, plusieurs IA
     * peuvent s'enchainer toutes seules jusqu'a ce qu'un humain doive
     * jouer ou que la partie se termine.
     */
    void jouerSiIA();
};

#endif // MODELE_JEU_HPP
