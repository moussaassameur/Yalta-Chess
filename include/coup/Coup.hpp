#ifndef COUP_HPP
#define COUP_HPP

#include <string>

class Plateau;

/**
 * @file Coup.hpp
 * @brief Interface du pattern Command -- "un coup" est une action
 *        executable / annulable.
 *
 * Pattern utilise : COMMAND. Chaque type de coup encapsule une action sur
 * le plateau (deplacement simple, capture, roque, promotion, prise en
 * passant). L'interface impose les operations symetriques executer() /
 * annuler() afin que l'historique puisse defaire un coup et le refaire.
 *
 * Avantages dans le projet :
 *   - decouplage : le moteur de jeu ne sait pas comment chaque coup
 *     s'execute, il appelle juste executer().
 *   - undo / redo gratuit via une simple pile (Historique).
 *   - l'IA MinMax pourra simuler un coup, evaluer, puis annuler() pour
 *     remonter dans l'arbre de recherche.
 */
class Coup {
public:
    virtual ~Coup() = default;

    /**
     * @brief Applique le coup sur le plateau.
     * @param plateau Plateau a modifier.
     *
     * Le coup doit memoriser tout ce qui est necessaire a annuler() :
     * pieces capturees, etat des drapeaux des pieces (roi a deja bouge,
     * pion a deja bouge, etc.).
     */
    virtual void executer(Plateau& plateau) = 0;

    /**
     * @brief Defait le coup et restaure exactement l'etat anterieur.
     * @param plateau Plateau a remettre dans son etat precedent.
     *
     * Apres annuler(), le plateau doit etre indistinguable de l'etat
     * d'avant executer().
     */
    virtual void annuler(Plateau& plateau) = 0;

    /**
     * @brief Verifie que le coup est applicable sur le plateau actuel.
     * @param plateau Plateau a tester.
     * @return true si le coup est legal au sens basique (cases existent,
     *         depart occupe...). NE verifie PAS les regles specifiques
     *         de chaque piece -- celles-ci sont du ressort de
     *         Piece::getDeplacements (Phase 3).
     */
    virtual bool estValide(const Plateau& plateau) const = 0;

    /**
     * @brief Notation lisible du coup, pour les logs et l'historique.
     * @return Chaine du type "a1-a4" (deplacement) ou "a1xa4" (capture).
     */
    virtual std::string getNotation() const = 0;
};

#endif // COUP_HPP
