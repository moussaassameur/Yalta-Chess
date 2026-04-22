#include "observer/AlerteEchec.hpp"
#include "model/ModeleJeu.hpp"
#include "model/EtatPartie.hpp"
#include <iostream>

void AlerteEchec::mettreAJour(const ModeleJeu& modele) {
    // TODO : afficher une alerte Qt quand un roi est en échec
    if (modele.getEtat() == EtatPartie::ECHEC) {
        std::cout << "[ALERTE] Le roi de "
                  << modele.getJoueurActuel()->getNom()
                  << " est en echec !" << std::endl;
    }
}
