#ifndef PION_HPP
#define PION_HPP

#include "pieces/Piece.hpp"

class Pion : public Piece {
private:
    bool aDejaBouge;  // false au départ, true après le premier coup

public:
    Pion(Couleur couleur, std::shared_ptr<Case> position);
    ~Pion() override = default;

    std::vector<std::shared_ptr<Case>> getDeplacements(const Plateau& plateau) const override;
    std::string getType() const override;

    bool getADejaBouge() const;
    void setADejaBouge(bool valeur);

private:
    // Méthodes utilitaires privées pour clarifier le code
    // Renvoie le vecteur d'avancement du pion selon sa couleur
    void getDirectionAvancement(int& dq, int& dr) const;

    // Renvoie les 2 vecteurs de diagonale de capture selon sa couleur
    void getDirectionsCaptures(int& dq1, int& dr1, int& dq2, int& dr2) const;
};

#endif