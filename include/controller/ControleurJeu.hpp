#ifndef CONTROLEURJEU_HPP
#define CONTROLEURJEU_HPP

#include <memory>
#include <QObject>

class ModeleJeu;
class VueJeu;
class Case;
class Coup;

// Controleur MVC — fait le lien entre les clics Qt et le modele
class ControleurJeu : public QObject {
    Q_OBJECT

private:
    std::shared_ptr<ModeleJeu> modele;
    std::shared_ptr<VueJeu>    vue;

    std::shared_ptr<Case> caseSelectionnee; // la case cliquee en premier

public:
    ControleurJeu(std::shared_ptr<ModeleJeu> modele,
                  std::shared_ptr<VueJeu>    vue,
                  QObject* parent = nullptr);
    ~ControleurJeu() override = default;

    void initialiser();
    void gererTour();

public slots:
    // appele quand le joueur clique sur une case du plateau
    void gererClic(int q, int r);

private:
    std::shared_ptr<Coup> construireCoup(std::shared_ptr<Case> depart,
                                         std::shared_ptr<Case> arrivee);
};

#endif // CONTROLEURJEU_HPP
