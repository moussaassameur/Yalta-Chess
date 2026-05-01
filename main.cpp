#include "model/ModeleJeu.hpp"
#include "view/VueJeu.hpp"
#include "controller/ControleurJeu.hpp"

#include <QApplication>
#include <memory>

/**
 * @file main.cpp
 * @brief Point d'entree de l'application Yalta Chess.
 *
 * Cable les composants du MVC :
 *   - cree le ModeleJeu (donnees + logique + Sujet de l'Observer)
 *   - cree la VueJeu Qt et l'enregistre comme observateur du modele
 *   - cree le ControleurJeu qui ecoute les clics de la vue et les
 *     traduit en operations sur le modele.
 *
 * La partie demarre quand le controleur appelle initialiser().
 */
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    auto modele = std::make_shared<ModeleJeu>();

    auto vue = std::make_shared<VueJeu>(modele);
    modele->attacher(vue);

    auto controleur = std::make_shared<ControleurJeu>(modele, vue);
    controleur->initialiser();

    return app.exec();
}
