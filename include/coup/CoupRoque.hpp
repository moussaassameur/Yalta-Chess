#ifndef COUPROQUE_HPP
#define COUPROQUE_HPP

#include "coup/Coup.hpp"
#include <memory>

class Case;
class Roi;
class Tour;

// Mouvement spécial : le roi et la tour bougent simultanément
class CoupRoque : public Coup {
private:
    std::shared_ptr<Roi>  roi;
    std::shared_ptr<Tour> tour;
    std::shared_ptr<Case> caseRoiAvant;
    std::shared_ptr<Case> caseTourAvant;
    std::shared_ptr<Case> caseRoiApres;
    std::shared_ptr<Case> caseTourApres;

public:
    CoupRoque(std::shared_ptr<Roi>  roi,
              std::shared_ptr<Tour> tour,
              std::shared_ptr<Case> caseRoiApres,
              std::shared_ptr<Case> caseTourApres);

    void executer(Plateau& plateau) override;
    void annuler(Plateau& plateau) override;
    bool estValide(const Plateau& plateau) const override;
    std::string getNotation() const override;
};

#endif // COUPROQUE_HPP
