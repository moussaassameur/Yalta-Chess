#ifndef COUPPRISEENPASSANT_HPP
#define COUPPRISEENPASSANT_HPP

#include "coup/Coup.hpp"
#include <memory>

class Case;
class Pion;

// Mouvement spécial : un pion capture un pion adjacent qui vient d'avancer de 2 cases
class CoupPriseEnPassant : public Coup {
private:
    std::shared_ptr<Pion> pionAttaquant;
    std::shared_ptr<Pion> pionCapture;
    std::shared_ptr<Case> caseDepart;
    std::shared_ptr<Case> caseArrivee;

public:
    CoupPriseEnPassant(std::shared_ptr<Pion> pionAttaquant,
                       std::shared_ptr<Pion> pionCapture,
                       std::shared_ptr<Case> caseDepart,
                       std::shared_ptr<Case> caseArrivee);

    void executer(Plateau& plateau) override;
    void annuler(Plateau& plateau) override;
    bool estValide(const Plateau& plateau) const override;
    std::string getNotation() const override;
};

#endif // COUPPRISEENPASSANT_HPP
