#ifndef COUP_HPP
#define COUP_HPP

#include <string>
#include <memory>

class Plateau;

// Interface du pattern Command
// Chaque type de coup sait s'exécuter et s'annuler
class Coup {
public:
    virtual ~Coup() = default;

    virtual void executer(Plateau& plateau) = 0;
    virtual void annuler(Plateau& plateau) = 0;
    virtual bool estValide(const Plateau& plateau) const = 0;
    virtual std::string getNotation() const = 0;
};

#endif // COUP_HPP
