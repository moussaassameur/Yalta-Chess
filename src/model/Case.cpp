#include "model/Case.hpp"
#include "pieces/Piece.hpp"

/**
 * @file Case.cpp
 * @brief Implementation de la classe Case et de la notation Yalta.
 *
 * Chaque case possede un identifiant unique sur tout le plateau.
 * La forme est <lettre><chiffre> (ex. a1, d4, h12, l9).
 *
 * Les 12 colonnes a..l traversent chacune deux sextants adjacents :
 *
 *   Colonne | Sextant 1 (rangs 1-4 ou 5-8) | Sextant 2 (rangs 5-8 ou 9-12)
 *   --------+------------------------------+-------------------------------
 *   a..d    | S0 (rangs 1-4)               | S1 (rangs 5-8)
 *   e..h    | S5 (rangs 1-4)               | S4 (rangs 9-12)
 *   i..l    | S2 (rangs 5-8)               | S3 (rangs 9-12)
 *
 * Formules par sextant :
 *   S0 : file = 'a'+x,      rank = y+1
 *   S1 : file = 'a'+(y-4),  rank = 8-x
 *   S2 : file = 'a'+(19-x), rank = 12-y
 *   S3 : file = 'a'+(19-y), rank = 20-x
 *   S4 : file = 'a'+(11-x), rank = 20-y
 *   S5 : file = 'a'+(7-y),  rank = x-3
 */

/// Table : couleur du proprietaire pour chaque sextant 0..5.
static const Couleur PROPRIETAIRE[6] = {
    Couleur::BLANC,  // S0
    Couleur::ROUGE,  // S1
    Couleur::ROUGE,  // S2
    Couleur::NOIR,   // S3
    Couleur::NOIR,   // S4
    Couleur::BLANC   // S5
};

// ─── Implementation ───────────────────────────────────────────────────────

Case::Case(int x, int y, int sextant, const std::string& couleur)
    : x(x), y(y), sextant(sextant),
      couleurDamier(couleur),
      piece(nullptr) {
}

int         Case::getX() const             { return x; }
int         Case::getY() const             { return y; }
int         Case::getSextant() const       { return sextant; }
std::string Case::getCouleurDamier() const { return couleurDamier; }

Couleur Case::getProprietaire() const {
    return PROPRIETAIRE[sextant];
}

char Case::getFile() const {
    switch (sextant) {
        case 0: return static_cast<char>('a' + x);
        case 1: return static_cast<char>('a' + (y - 4));
        case 2: return static_cast<char>('a' + (19 - x));
        case 3: return static_cast<char>('a' + (19 - y));
        case 4: return static_cast<char>('a' + (11 - x));
        case 5: return static_cast<char>('a' + (7  - y));
        default: return '?';
    }
}

int Case::getRank() const {
    switch (sextant) {
        case 0: return y + 1;
        case 1: return 8 - x;
        case 2: return 12 - y;
        case 3: return 20 - x;
        case 4: return 20 - y;
        case 5: return x - 3;
        default: return -1;
    }
}

std::string Case::getNotation() const {
    std::string s;
    s += getFile();
    s += std::to_string(getRank());
    return s;
}

// ─── Gestion de la piece occupante ───────────────────────────────────────

void                   Case::setPiece(std::shared_ptr<Piece> p) { piece = p; }
void                   Case::retirerPiece()                     { piece = nullptr; }
std::shared_ptr<Piece> Case::getPiece() const                   { return piece; }
bool                   Case::estOccupee() const                 { return piece != nullptr; }
