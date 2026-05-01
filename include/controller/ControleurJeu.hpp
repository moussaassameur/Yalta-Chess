#ifndef CONTROLEUR_JEU_HPP
#define CONTROLEUR_JEU_HPP

#include <QObject>
#include <memory>

class ModeleJeu;
class VueJeu;
class Case;

/**
 * @file ControleurJeu.hpp
 * @brief Controleur du MVC -- relie les clics utilisateurs au modele.
 *
 * Ecoute le signal Qt VueJeu::caseCliquee. Maintient la "case selectionnee"
 * et applique la machine d'etat suivante :
 *   - 1er clic : si la case contient une piece du joueur courant,
 *                la selectionne et demande a la Vue de surligner ses
 *                coups possibles.
 *   - clic suivant :
 *       * meme case   -> deselection,
 *       * autre piece du joueur courant -> change de selection,
 *       * case d'un coup possible -> joue le coup via ModeleJeu.
 *
 * La gestion des coups speciaux (roque, promotion, prise en passant)
 * sera ajoutee en Phase 7. Pour l'instant, tous les coups sont des
 * CoupSimple.
 */
class ControleurJeu : public QObject {
    Q_OBJECT

public:
    ControleurJeu(std::shared_ptr<ModeleJeu> modele,
                  std::shared_ptr<VueJeu>    vue,
                  QObject* parent = nullptr);
    ~ControleurJeu() override = default;

    /// @brief Demarre la partie : initialise le modele et affiche la vue.
    void initialiser();

private slots:
    /// @brief Slot appele a chaque clic sur une case du plateau.
    void gererClic(int x, int y);

private:
    std::shared_ptr<ModeleJeu> modele;
    std::shared_ptr<VueJeu>    vue;

    /// Case actuellement selectionnee (null si aucune).
    std::shared_ptr<Case> caseSelectionnee;

    /// Affiche un dialogue modal et retourne le type de piece choisi.
    std::string demanderPromotion();
};

#endif // CONTROLEUR_JEU_HPP
