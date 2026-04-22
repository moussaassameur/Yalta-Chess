#ifndef OBSERVATEUR_HPP
#define OBSERVATEUR_HPP

// Interface du pattern Observer
class ModeleJeu;

class Observateur {
public:
    virtual ~Observateur() = default;
    virtual void mettreAJour(const ModeleJeu& modele) = 0;
};

#endif // OBSERVATEUR_HPP
