#ifndef ALERTEECHEC_HPP
#define ALERTEECHEC_HPP

#include "observer/Observateur.hpp"

// Observateur spécialisé : réagit quand un roi est en échec
class AlerteEchec : public Observateur {
public:
    AlerteEchec() = default;
    ~AlerteEchec() override = default;

    void mettreAJour(const ModeleJeu& modele) override;
};

#endif // ALERTEECHEC_HPP
