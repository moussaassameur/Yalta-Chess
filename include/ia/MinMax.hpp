#ifndef MIN_MAX_HPP
#define MIN_MAX_HPP

#include "model/Couleur.hpp"
#include <memory>

class Coup;
class Plateau;

/**
 * @file MinMax.hpp
 * @brief Algorithme MinMax pour le jeu d'echecs Yalta (3 joueurs).
 *
 * Adaptation paranoiaque du MinMax classique a 2 joueurs :
 *   - L'IA joue le role de PLUS (noeuds MAX).
 *   - Les 2 autres joueurs sont consideres collectivement comme MOINS
 *     (noeuds MIN) : on suppose qu'ils minimisent la valeur de l'IA.
 *
 * Strategie :
 *   - On explore l'arbre des coups possibles jusqu'a une profondeur donnee.
 *   - Pour chaque feuille, on appelle la fonction d'evaluation.
 *   - On remonte les valeurs en alternant MAX et MIN selon le joueur courant.
 *
 * Pas de copie du plateau : on modifie le plateau global via Coup::executer
 * puis on annule via Coup::annuler pour restaurer l'etat initial -- c'est le
 * parcours en post-ordre decrit dans le texte du prof.
 *
 * Pour le multi-threading (Phase suivante), nbThreads sera utilise pour
 * paralleliser l'exploration au niveau de la racine.
 */
class MinMax {
public:
    /**
     * @param profondeur Nombre de demi-coups a explorer (>= 1).
     * @param nbThreads  Nombre de threads pour la parallelisation
     *                   (non utilise en mono-thread, prepare pour la suite).
     * @param couleurIA  Couleur du joueur represente par l'IA.
     */
    MinMax(int profondeur, int nbThreads, Couleur couleurIA);

    /**
     * @brief Recherche le meilleur coup pour l'IA.
     * @param plateau Plateau dans son etat courant (sera restaure apres).
     * @return Le coup juge optimal, ou nullptr si aucun coup legal.
     */
    std::shared_ptr<Coup> getMeilleurCoup(Plateau& plateau);

    int     getProfondeur() const;
    int     getNbThreads()  const;
    Couleur getCouleurIA()  const;

private:
    /**
     * @brief Procedure recursive : evalue un noeud de l'arbre.
     * @param plateau         Plateau dans son etat actuel.
     * @param profondeur      Profondeur restante a explorer.
     * @param maximisant      true si c'est un noeud MAX (tour de l'IA),
     *                        false si c'est un noeud MIN (tour adversaire).
     * @param joueurCourant   Couleur du joueur dont c'est le tour.
     * @return Valeur du noeud, du point de vue de l'IA.
     */
    int minMax(Plateau& plateau, int profondeur,
               bool maximisant, Couleur joueurCourant);

    /**
     * @brief Fonction d'evaluation d'un plateau pour un joueur.
     *
     * Score = somme des valeurs des pieces de `couleur` MOINS la somme
     * des valeurs des pieces adverses. Plus le score est eleve, meilleure
     * est la situation pour `couleur`.
     *
     * Valeurs standard : Pion=1, Cavalier=3, Fou=3, Tour=5, Reine=9, Roi=1000.
     *
     * @param plateau Plateau a evaluer.
     * @param couleur Joueur du point de vue duquel on evalue.
     * @return Score signe (positif = favorable a `couleur`).
     */
    int evaluer(const Plateau& plateau, Couleur couleur) const;

    /// Renvoie la couleur du joueur qui joue apres `c` (rotation BLANC→ROUGE→NOIR).
    static Couleur joueurSuivant(Couleur c);

    int     profondeur;
    int     nbThreads;
    Couleur couleurIA;
};

#endif // MIN_MAX_HPP
