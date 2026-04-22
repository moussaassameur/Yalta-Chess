#ifndef VUEJEU_HPP
#define VUEJEU_HPP

#include "observer/Observateur.hpp"
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <memory>
#include <vector>

class ModeleJeu;
class Case;
class QLabel;

// Vue principale Qt — affiche le plateau hexagonal et les pieces
// Implemente Observateur pour etre notifiee quand le modele change
class VueJeu : public QMainWindow, public Observateur {
    Q_OBJECT

private:
    std::shared_ptr<ModeleJeu> modele;

    QGraphicsScene* scene;
    QGraphicsView*  vue;
    QLabel*         labelJoueur;  // affiche le nom du joueur actuel
    QLabel*         labelEtat;    // affiche l etat de la partie

    // surlignage de la case selectionnee et des coups possibles
    std::shared_ptr<Case>              caseSelectionnee;
    std::vector<std::shared_ptr<Case>> coupsPossibles;

public:
    explicit VueJeu(std::shared_ptr<ModeleJeu> modele, QWidget* parent = nullptr);
    ~VueJeu() override = default;

    // implementation de l interface Observateur
    void mettreAJour(const ModeleJeu& modele) override;

    void afficher();
    void afficherAlerteEchec(const std::string& nomJoueur);

    // met en surbrillance la case selectionnee et ses coups possibles
    void surligner(std::shared_ptr<Case> selection,
                   std::vector<std::shared_ptr<Case>> coups);
    void effacerSurlignage();

signals:
    // emis quand le joueur clique sur une case du plateau
    void caseCliquee(int q, int r);

protected:
    // intercepte les clics sur le viewport pour les convertir en (q,r)
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void dessinerPlateau();
    void dessinerPieces();
    void configurerInterface();
    void redessiner();
};

#endif // VUEJEU_HPP
