#include "view/VueJeu.hpp"
#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "model/EtatPartie.hpp"
#include "pieces/Piece.hpp"
#include "joueur/Joueur.hpp"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPolygonItem>
#include <QGraphicsSimpleTextItem>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPolygonF>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QMouseEvent>
#include <QEvent>
#include <QPainter>
#include <QFrame>

#include <array>
#include <cmath>

/**
 * @file VueJeu.cpp
 * @brief Implementation de la Vue Qt.
 *
 * La geometrie du plateau hexagonal est conservee de la version
 * precedente du projet : on travaille en repere centre, l'hexagone a
 * 6 sommets v1..v6, chaque sextant est un rhombe partant d'un sommet
 * vers le centre. Les 16 cases d'un sextant sont obtenues par
 * interpolation bilineaire entre 3 directions (s1, s2 demi-rayons et un
 * midpoint d'arete).
 */

// ─── Geometrie de l'hexagone ─────────────────────────────────────────────

static const double TAILLE   = 350.0;
static const double COTE     = TAILLE / 2.0;
static const double HAUTEUR  = std::sqrt(TAILLE * TAILLE - COTE * COTE);
static const double PI       = 3.14159265358979323846;
static const double COS30    = std::cos(PI / 6.0);
static const double SIN30    = std::sin(PI / 6.0);
static const double COS60    = std::cos(PI / 3.0);

static std::array<QPointF, 6> sommetsHexagone() {
    return {{
        QPointF(-TAILLE * COS60, -HAUTEUR),  // v1 haut-gauche
        QPointF( TAILLE * COS60, -HAUTEUR),  // v2 haut-droite
        QPointF( TAILLE,          0.0),       // v3 droite
        QPointF( TAILLE * COS60,  HAUTEUR),  // v4 bas-droite
        QPointF(-TAILLE * COS60,  HAUTEUR),  // v5 bas-gauche
        QPointF(-TAILLE,          0.0),       // v6 gauche
    }};
}

static std::array<QPointF, 6> midpointsHexagone() {
    return {{
        QPointF(-HAUTEUR * COS30, -HAUTEUR * SIN30),  // mid v6-v1
        QPointF( 0.0,             -HAUTEUR),          // mid v1-v2
        QPointF( HAUTEUR * COS30, -HAUTEUR * SIN30),  // mid v2-v3
        QPointF( HAUTEUR * COS30,  HAUTEUR * SIN30),  // mid v3-v4
        QPointF( 0.0,              HAUTEUR),          // mid v4-v5
        QPointF(-HAUTEUR * COS30,  HAUTEUR * SIN30),  // mid v5-v6
    }};
}

/// Renvoie les 4 coins du quadrilatere d'une case dans le repere centre.
static std::array<QPointF, 4> coinsDeCase(int x, int y, int sextant) {
    auto V = sommetsHexagone();
    auto M = midpointsHexagone();

    int i = sextant;
    QPointF s1     = V[i]            / 2.0;
    QPointF s2     = V[(i + 2) % 6]  / 2.0;
    QPointF corner = V[(i + 4) % 6];
    QPointF vAbc   = M[(i + 1) % 6];

    int xLocal = x % 4;
    int yLocal = y % 4;

    double rX1 = xLocal       / 4.0;
    double rX2 = (xLocal + 1) / 4.0;
    double rY1 = yLocal       / 4.0;
    double rY2 = (yLocal + 1) / 4.0;

    auto U = [&](double rY) -> QPointF {
        return QPointF(vAbc.x() * rY - s1.x() * rY + s2.x(),
                       vAbc.y() * rY - s1.y() * rY + s2.y());
    };
    QPointF U1 = U(rY1);
    QPointF U2 = U(rY2);

    auto pt = [&](QPointF U_, double rY, double rX) -> QPointF {
        return corner + QPointF(s1.x() * rY + U_.x() * rX,
                                s1.y() * rY + U_.y() * rX);
    };

    return {
        pt(U1, rY1, rX1),
        pt(U1, rY1, rX2),
        pt(U2, rY2, rX2),
        pt(U2, rY2, rX1),
    };
}

static QPointF centreDeCase(int x, int y, int sextant) {
    auto coins = coinsDeCase(x, y, sextant);
    return (coins[0] + coins[1] + coins[2] + coins[3]) / 4.0;
}

/// Symbole Unicode pour chaque type de piece.
static QString symbolePiece(const std::string& type) {
    if (type == "Roi")      return QString::fromUtf8("\xE2\x99\x9A");
    if (type == "Reine")    return QString::fromUtf8("\xE2\x99\x9B");
    if (type == "Tour")     return QString::fromUtf8("\xE2\x99\x9C");
    if (type == "Fou")      return QString::fromUtf8("\xE2\x99\x9D");
    if (type == "Cavalier") return QString::fromUtf8("\xE2\x99\x9E");
    if (type == "Pion")     return QString::fromUtf8("\xE2\x99\x9F");
    return QString("?");
}

// ─── Implementation VueJeu ───────────────────────────────────────────────

VueJeu::VueJeu(std::shared_ptr<ModeleJeu> modele, QWidget* parent)
    : QMainWindow(parent),
      modele(modele),
      scene(new QGraphicsScene(this)),
      vue(new QGraphicsView(scene, this)),
      labelJoueur(new QLabel("Joueur : -", this)),
      labelEtat(new QLabel("Etat : EN COURS", this)),
      caseSelectionnee(nullptr) {
    configurerInterface();
}

void VueJeu::configurerInterface() {
    setWindowTitle("Jeu d'echecs Yalta");
    resize(1000, 900);

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    QHBoxLayout* barreInfo = new QHBoxLayout();
    labelJoueur->setStyleSheet("font-size: 16px; font-weight: bold; color: white;");
    labelEtat->setStyleSheet("font-size: 14px; color: white;");
    barreInfo->addWidget(labelJoueur);
    barreInfo->addStretch();
    barreInfo->addWidget(labelEtat);

    layout->addLayout(barreInfo);
    layout->addWidget(vue);

    setCentralWidget(central);
    central->setStyleSheet("background-color: #303030;");
    vue->setRenderHint(QPainter::Antialiasing);
    vue->setBackgroundBrush(QBrush(QColor(48, 48, 48)));
    vue->setFrameShape(QFrame::NoFrame);

    vue->viewport()->installEventFilter(this);
}

void VueJeu::afficher() {
    redessiner();
    show();
    vue->fitInView(scene->itemsBoundingRect().adjusted(-40, -40, 40, 40),
                   Qt::KeepAspectRatio);
}

void VueJeu::mettreAJour(const ModeleJeu& m) {
    auto joueur = m.getJoueurActuel();
    if (joueur) {
        labelJoueur->setText(QString("Joueur : %1")
            .arg(QString::fromStdString(joueur->getNom())));
    }

    QString txt;
    switch (m.getEtat()) {
        case EtatPartie::EN_COURS:     txt = "EN COURS";     break;
        case EtatPartie::ECHEC:        txt = "ECHEC !";      break;
        case EtatPartie::ECHEC_ET_MAT: txt = "ECHEC ET MAT"; break;
        case EtatPartie::PAT:          txt = "PAT";          break;
        case EtatPartie::NULLE:        txt = "NULLE";        break;
    }

    // Affiche les joueurs elimines dans le label d'etat.
    QString elimines;
    for (const auto& j : m.getJoueurs()) {
        if (j->getEstElimine())
            elimines += QString("  [%1 elimine]").arg(
                QString::fromStdString(j->getNom()));
    }
    labelEtat->setText("Etat : " + txt + elimines);

    caseSelectionnee = nullptr;
    coupsPossibles.clear();
    redessiner();
}

void VueJeu::surligner(std::shared_ptr<Case> selection,
                       std::vector<std::shared_ptr<Case>> coups) {
    caseSelectionnee = selection;
    coupsPossibles   = std::move(coups);
    redessiner();
}

void VueJeu::effacerSurlignage() {
    caseSelectionnee = nullptr;
    coupsPossibles.clear();
    redessiner();
}

void VueJeu::redessiner() {
    scene->clear();
    dessinerPlateau();
    dessinerPieces();
}

void VueJeu::dessinerPlateau() {
    if (!modele) return;
    auto cases = modele->getPlateau()->getToutesLesCases();

    const QColor COULEUR_FONCE(54, 39, 32);
    const QColor COULEUR_CLAIR(229, 210, 170);

    for (const auto& c : cases) {
        int x  = c->getX();
        int y  = c->getY();
        int sx = c->getSextant();

        auto coins = coinsDeCase(x, y, sx);
        QPolygonF quad;
        quad << coins[0] << coins[1] << coins[2] << coins[3];

        QColor couleur = (c->getCouleurDamier() == "clair") ? COULEUR_CLAIR : COULEUR_FONCE;
        QPen pen(QColor(20, 15, 10), 1);

        // Surbrillance.
        if (caseSelectionnee
            && caseSelectionnee->getX() == x
            && caseSelectionnee->getY() == y) {
            couleur = QColor(255, 230, 80);
            pen = QPen(QColor(200, 150, 0), 3);
        } else {
            for (const auto& dest : coupsPossibles) {
                if (dest->getX() == x && dest->getY() == y) {
                    if (c->estOccupee()) {
                        couleur = QColor(220, 90, 90);
                        pen = QPen(QColor(150, 30, 30), 3);
                    } else {
                        couleur = QColor(150, 220, 140);
                        pen = QPen(QColor(60, 140, 60), 3);
                    }
                    break;
                }
            }
        }

        auto* item = scene->addPolygon(quad, pen, QBrush(couleur));
        item->setData(0, x);
        item->setData(1, y);
        item->setToolTip(QString("%1  (sext=%2, x=%3, y=%4)")
            .arg(QString::fromStdString(c->getNotation()))
            .arg(sx).arg(x).arg(y));

        // Etiquette de notation dans la case.
        QPointF centre = centreDeCase(x, y, sx);
        auto* label = scene->addSimpleText(
            QString::fromStdString(c->getNotation()));
        QFont fLabel;
        fLabel.setPixelSize(9);
        label->setFont(fLabel);
        label->setBrush(QBrush(QColor(50, 180, 50)));
        label->setZValue(1);
        QRectF lr = label->boundingRect();
        label->setPos(centre.x() - lr.width()  / 2.0,
                      centre.y() - lr.height() / 2.0);
    }

    // Cadre exterieur.
    auto V = sommetsHexagone();
    QPolygonF hex;
    for (int i = 0; i < 6; i++) hex << V[i];
    auto* cadre = scene->addPolygon(hex, QPen(Qt::white, 2), Qt::NoBrush);
    cadre->setZValue(-1);
}

void VueJeu::dessinerPieces() {
    if (!modele) return;
    auto cases = modele->getPlateau()->getToutesLesCases();

    QFont policePiece;
    policePiece.setPixelSize(28);

    for (const auto& c : cases) {
        auto piece = c->getPiece();
        if (!piece || !piece->estVivante()) continue;

        QPointF centre = centreDeCase(c->getX(), c->getY(), c->getSextant());

        QColor fond, contour;
        switch (piece->getCouleur()) {
            case Couleur::BLANC:
                fond    = QColor(255, 255, 255); contour = QColor(0, 0, 0); break;
            case Couleur::NOIR:
                fond    = QColor(30, 30, 30);    contour = QColor(220, 220, 220); break;
            case Couleur::ROUGE:
                fond    = QColor(214, 21, 65);   contour = QColor(40, 0, 0); break;
        }

        auto* texte = scene->addSimpleText(symbolePiece(piece->getType()), policePiece);
        texte->setBrush(QBrush(fond));
        texte->setPen(QPen(contour, 1.2));

        QRectF br = texte->boundingRect();
        texte->setPos(centre.x() - br.width()  / 2.0,
                      centre.y() - br.height() / 2.0);
    }
}

bool VueJeu::eventFilter(QObject* watched, QEvent* event) {
    if (watched == vue->viewport() && event->type() == QEvent::MouseButtonPress) {
        auto* me = static_cast<QMouseEvent*>(event);
        if (me->button() == Qt::LeftButton) {
            QPointF scenePos = vue->mapToScene(me->pos());
            const auto items = scene->items(scenePos);
            for (auto* item : items) {
                QVariant qData = item->data(0);
                QVariant rData = item->data(1);
                if (qData.isValid() && rData.isValid()) {
                    emit caseCliquee(qData.toInt(), rData.toInt());
                    return true;
                }
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
