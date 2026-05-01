#ifndef VUE_JEU_HPP
#define VUE_JEU_HPP

#include "observer/Observateur.hpp"
#include <QMainWindow>
#include <memory>
#include <vector>

class ModeleJeu;
class Case;
class QGraphicsScene;
class QGraphicsView;
class QLabel;

/**
 * @file VueJeu.hpp
 * @brief Vue Qt du jeu Yalta -- partie V du MVC.
 *
 * Implemente Observateur pour etre notifiee par ModeleJeu de chaque
 * changement d'etat. Dessine le plateau hexagonal et les pieces dans
 * une QGraphicsScene. Capte les clics souris et les transforme en
 * signal Qt caseCliquee(x, y) consomme par ControleurJeu.
 *
 * La geometrie de dessin (hexagone -> 6 sextants -> 16 cases par
 * sextant) est calculee dans VueJeu.cpp. Cette partie est volontairement
 * isolee : la logique du jeu ne depend pas de Qt.
 */
class VueJeu : public QMainWindow, public Observateur {
    Q_OBJECT

public:
    explicit VueJeu(std::shared_ptr<ModeleJeu> modele, QWidget* parent = nullptr);
    ~VueJeu() override = default;

    /// Implementation de Observateur : redessine apres chaque notif.
    void mettreAJour(const ModeleJeu& modele) override;

    /// Affiche la fenetre et ajuste le zoom au plateau.
    void afficher();

    /// Surligne une case selectionnee + ses coups possibles.
    void surligner(std::shared_ptr<Case> selection,
                   std::vector<std::shared_ptr<Case>> coups);

    /// Efface tout surlignage.
    void effacerSurlignage();

signals:
    /// Emis par eventFilter quand le joueur clique sur une case.
    void caseCliquee(int x, int y);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    std::shared_ptr<ModeleJeu> modele;

    QGraphicsScene* scene;
    QGraphicsView*  vue;
    QLabel*         labelJoueur;
    QLabel*         labelEtat;

    // Etat de surlignage local a la vue.
    std::shared_ptr<Case>              caseSelectionnee;
    std::vector<std::shared_ptr<Case>> coupsPossibles;

    void configurerInterface();
    void redessiner();
    void dessinerPlateau();
    void dessinerPieces();
};

#endif // VUE_JEU_HPP
