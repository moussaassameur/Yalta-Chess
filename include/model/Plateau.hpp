#ifndef PLATEAU_HPP
#define PLATEAU_HPP

#include "model/Case.hpp"
#include <map>
#include <memory>
#include <utility>
#include <vector>

/**
 * @file Plateau.hpp
 * @brief Plateau du jeu Yalta : 96 cases reparties en 6 sextants.
 *
 * Phase 1 : ce plateau ne fait QUE creer et stocker les 96 cases. Aucune
 * logique de deplacement, aucune topologie de bending, aucune piece n'y
 * vit. Ces fonctionnalites seront ajoutees dans les phases suivantes.
 *
 * Le stockage utilise une grille virtuelle 12x12 indexee par (x, y),
 * dont seules 96 positions sont valides. Chaque sextant occupe un bloc
 * 4x4 de la grille -- 6 blocs valides sur les 9 possibles.
 */
class Plateau {
public:
    /// Construit le plateau et cree les 96 cases.
    Plateau();

    ~Plateau() = default;

    /**
     * @brief Recupere la case a la position (x, y).
     * @param x Coordonnee x dans la grille 12x12.
     * @param y Coordonnee y dans la grille 12x12.
     * @return La case si elle existe, sinon nullptr (position dans un trou
     *         de la grille).
     */
    std::shared_ptr<Case> getCase(int x, int y) const;

    /// @return Vecteur de toutes les cases du plateau (96 elements).
    std::vector<std::shared_ptr<Case>> getToutesLesCases() const;

    /// @return Nombre total de cases (devrait toujours etre 96).
    int nombreDeCases() const;

    /**
     * @brief Place les 48 pieces de depart d'une partie Yalta.
     *
     * Pour chaque joueur (BLANC, ROUGE, NOIR), 16 pieces sont posees :
     *   8 pions, 2 tours, 2 cavaliers, 2 fous, 1 reine et 1 roi. Le Roi
     *   est sur la case 'e1' du joueur, la Reine sur 'd1' (notation
     *   classique des echecs).
     *
     * Si des pieces existent deja sur le plateau, elles sont ecrasees
     * (la methode peut etre appelee sur un plateau frais).
     */
    void initialiserPiecesYalta();

    /// @return Toutes les pieces vivantes d'une couleur donnee.
    std::vector<std::shared_ptr<class Piece>>
    getPiecesDeCouleur(Couleur couleur) const;

    /// @return Case occupee par le Roi de la couleur c, ou nullptr.
    std::shared_ptr<Case> trouverRoi(Couleur c) const;

    /// @return true si le roi de la couleur c est attaque par une piece adverse.
    bool estEnEchec(Couleur c) const;

    /// @return true si le roi de `victime` est attaque specifiquement par `attaquant`.
    bool estEnEchecPar(Couleur victime, Couleur attaquant) const;

    /// @return true si le joueur `c` possede au moins un coup legal.
    bool aUnCoupLegal(Couleur c);

    /// @return true si le joueur `c` est echec et mat
    ///         (son roi est en echec ET il n'a aucun coup legal).
    bool estMat(Couleur c);

    /// @return true si le joueur `c` est pat
    ///         (aucun coup legal mais son roi n'est PAS en echec).
    bool estPat(Couleur c);

    /// @name Topologie Yalta -- voisinage avec bending
    ///
    /// Les pieces glissantes (Tour, Fou, Reine) avancent d'une case a la
    /// fois dans une direction (dx, dy). Lorsqu'elles franchissent une
    /// frontiere interne entre deux sextants, leur direction se "courbe"
    /// pour suivre la geometrie hexagonale.
    ///
    /// Regle (orthogonale) :
    ///   - exit xLocal=3 du sextant i  -> entre dans (i-1+6)%6 a (yL_old, 3)
    ///                                   direction (dx, dy) -> (-dy, -dx)
    ///   - exit yLocal=3 du sextant i  -> entre dans (i+1)%6 a (3, xL_old)
    ///                                   direction (dx, dy) -> (-dy, -dx)
    /// @{

    /**
     * @brief Avance d'une case dans la direction (dx, dy), gerant le
     *        bending aux frontieres internes des sextants.
     * @param depart   Case de depart.
     * @param[in,out] dx  Composante x de la direction. Mise a jour si
     *                    bending applique.
     * @param[in,out] dy  Composante y. Mise a jour si bending.
     * @return La case voisine, ou nullptr si on sort du plateau (bord
     *         externe ou cas non gere comme la diagonale du centre).
     */
    std::shared_ptr<Case> voisinAvecDir(std::shared_ptr<Case> depart,
                                         int& dx, int& dy) const;

    /**
     * @brief Variante sans modification de direction. Pratique pour les
     *        pieces a portee 1 case (Roi).
     */
    std::shared_ptr<Case> voisin(std::shared_ptr<Case> depart,
                                  int dx, int dy) const;

    /**
     * @brief Version diagonale du bending, pour le Fou et les diagonales
     *        de la Reine.
     *
     * Meme logique que voisinAvecDir SAUF le sens de la direction mise a
     * jour au passage de frontiere :
     *   - exit xLocal=3 -> dx = -dx  (reflexion sur l'axe x)
     *   - exit yLocal=3 -> dy = -dy  (reflexion sur l'axe y)
     *
     * La formule orthogonale (-dy,-dx) est une identite pour (±1,∓1),
     * ce qui causait le bug : le Fou ne courbait jamais sa diagonale.
     */
    std::shared_ptr<Case> voisinDiagonal(std::shared_ptr<Case> depart,
                                          int& dx, int& dy) const;

    /// @}

    /// @name Etat de la prise en passant
    ///
    /// Apres qu'un pion ait avance de 2 cases d'un coup, le pion adverse
    /// situe immediatement a cote peut le capturer "en passant" comme s'il
    /// n'avait avance que d'une case. Ce droit n'existe que pour le coup
    /// suivant immediat -- ensuite il est perdu.
    ///
    ///   caseEnPassantCible : case "sautee" par le bond de 2 (destination
    ///                        du pion qui capture).
    ///   caseEnPassantPion  : case occupee par le pion qui a fait le bond
    ///                        de 2 (sera retire si la capture a lieu).
    /// @{

    std::shared_ptr<Case> getCaseEnPassantCible() const;
    std::shared_ptr<Case> getCaseEnPassantPion()  const;
    void setEnPassant(std::shared_ptr<Case> cible, std::shared_ptr<Case> pion);
    void clearEnPassant();

    /// @}

    /**
     * @brief Cree une copie profonde du plateau (cases + pieces).
     *
     * Necessaire pour le multi-threading de l'IA : chaque thread explore
     * son propre clone sans interferer avec les autres. Les pieces sont
     * recreees (meme type, couleur, drapeau aDejaBouge) et leur position
     * pointe vers les nouvelles cases du clone.
     */
    std::shared_ptr<Plateau> clone() const;

private:
    /// Cle = (x, y), valeur = Case partagee. std::map garantit l'absence
    /// de cle pour les positions invalides (trous de la grille).
    std::map<std::pair<int, int>, std::shared_ptr<Case>> cases;

    /// Etat de la prise en passant (voir setEnPassant).
    std::shared_ptr<Case> caseEnPassantCible;
    std::shared_ptr<Case> caseEnPassantPion;

    /// Cree les 96 cases dans la map cases[]. Appelle dans le constructeur.
    void creerCases();
};

#endif // PLATEAU_HPP
