#ifndef MINMAX_HPP
#define MINMAX_HPP

#include <memory>

class Coup;
class ModeleJeu;
class Plateau;

// IA basée sur l'algorithme Min-Max multi-threadé
class MinMax {
private:
    int profondeur; // profondeur max d'exploration de l'arbre
    int nbThreads;  // nombre de threads pour la parallélisation

public:
    MinMax(int profondeur = 3, int nbThreads = 4);
    ~MinMax() = default;

    // Point d'entrée : retourne le meilleur coup trouvé
    std::shared_ptr<Coup> getMeilleurCoup(ModeleJeu& modele);

private:
    // Algorithme récursif — utilise executer/annuler sans cloner le plateau
    int minMax(ModeleJeu& modele, int profondeur, bool maximisant);

    // Fonction d'évaluation heuristique (valeur des pièces + bonus positionnel)
    int evaluer(const Plateau& plateau) const;
};

#endif // MINMAX_HPP
