#ifndef COUP_ROQUE_HPP
#define COUP_ROQUE_HPP

#include "coup/Coup.hpp"
#include <memory>
#include <string>

class Case;
class Piece;

/**
 * @file CoupRoque.hpp
 * @brief Roque : le Roi et la Tour bougent en un seul coup. Notation "O-O".
 */
class CoupRoque : public Coup {
public:
    // Cases de depart et d'arrivee du Roi et de la Tour.
    CoupRoque(std::shared_ptr<Case> roiDepart,
              std::shared_ptr<Case> roiArrivee,
              std::shared_ptr<Case> tourDepart,
              std::shared_ptr<Case> tourArrivee);

    ~CoupRoque() override = default;

    void        executer(Plateau& plateau)              override;
    void        annuler(Plateau& plateau)               override;
    bool        estValide(const Plateau& plateau) const override;

private:
    std::shared_ptr<Case>  roiDepart;
    std::shared_ptr<Case>  roiArrivee;
    std::shared_ptr<Case>  tourDepart;
    std::shared_ptr<Case>  tourArrivee;

    std::shared_ptr<Piece> roi;
    std::shared_ptr<Piece> tour;
    bool roiDejaBougeAvant;
    bool tourDejaBougeAvant;
    bool execute;
};

#endif // COUP_ROQUE_HPP
