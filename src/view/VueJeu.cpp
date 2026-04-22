#include "view/VueJeu.hpp"
#include "model/ModeleJeu.hpp"
#include "model/Plateau.hpp"
#include "model/Case.hpp"
#include "model/EtatPartie.hpp"
#include "model/Couleur.hpp"
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
#include <QMessageBox>
#include <QPolygonF>
#include <QPointF>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QMouseEvent>
#include <QEvent>
#include <cmath>
#include <array>

// ─── Geometrie du hexagone Yalta ───
//
//      v1---v2          ,.b.,
//     /      \        a/     \c
//   v6   o   v3   o = origine, a..f = midpoints des aretes
//     \      /        f\     /d
//      v5---v4          `.e.´
//
// "TAILLE" = circumradius du hexagone, "HAUTEUR" = inradius.

static const double TAILLE = 350.0;          // circumradius du hexagone
static const double COTE   = TAILLE / 2.0;
static const double HAUTEUR = std::sqrt(TAILLE * TAILLE - COTE * COTE);  // ~= TAILLE * sqrt(3)/2
static const double PI = 3.14159265358979323846;
static const double COS30 = std::cos(PI / 6.0);
static const double SIN30 = std::sin(PI / 6.0);
static const double COS60 = std::cos(PI / 3.0);

// Sommets v1..v6 du hexagone (en repere ecran : y vers le bas).
//   v1 = haut-gauche, v2 = haut-droite, v3 = droite,
//   v4 = bas-droite,  v5 = bas-gauche,  v6 = gauche.
static std::array<QPointF, 6> sommetsHexagone() {
    return {{
        QPointF(-TAILLE * COS60, -HAUTEUR),  // v1
        QPointF( TAILLE * COS60, -HAUTEUR),  // v2
        QPointF( TAILLE,          0.0),       // v3
        QPointF( TAILLE * COS60,  HAUTEUR),  // v4
        QPointF(-TAILLE * COS60,  HAUTEUR),  // v5
        QPointF(-TAILLE,          0.0),       // v6
    }};
}

// Midpoints des aretes : va = mid(v6,v1), vb = mid(v1,v2), etc.
static std::array<QPointF, 6> midpointsHexagone() {
    return {{
        QPointF(-HAUTEUR * COS30, -HAUTEUR * SIN30),  // va
        QPointF( 0.0,             -HAUTEUR),          // vb
        QPointF( HAUTEUR * COS30, -HAUTEUR * SIN30),  // vc
        QPointF( HAUTEUR * COS30,  HAUTEUR * SIN30),  // vd
        QPointF( 0.0,              HAUTEUR),          // ve
        QPointF(-HAUTEUR * COS30,  HAUTEUR * SIN30),  // vf
    }};
}

// Pour une case (x, y) sur la grille 12x12 (avec sextant deja attache),
// retourne les 4 coins du quadrilatere correspondant.
//
// Calcul : chaque sextant est un rhombe, ses 4x4 cases sont obtenues par
// interpolation bilineaire entre 3 directions :
//   s1 = v[i]/2, s2 = v[(i+2)%6]/2  (deux demi-rayons depuis le centre)
//   corner = v[(i+4)%6]             (sommet exterieur du rhombe)
//   U(rY) = midpoint[(i+1)%6]*rY - s1*rY + s2
//   p(rX, rY) = s1*rY + U(rY)*rX
//   coin = corner + p
static std::array<QPointF, 4> coinsDeCase(int x, int y, int sextant) {
    auto V = sommetsHexagone();
    auto M = midpointsHexagone();

    int i = sextant;
    QPointF s1     = V[i]            / 2.0;
    QPointF s2     = V[(i + 2) % 6]  / 2.0;
    QPointF corner = V[(i + 4) % 6];   // pas de + mid : on travaille en repere centre, mid sera ajoute par le QGraphicsScene
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
        pt(U1, rY1, rX1),  // p1
        pt(U1, rY1, rX2),  // p2
        pt(U2, rY2, rX2),  // p3
        pt(U2, rY2, rX1),  // p4
    };
}

static QPointF centreDeCase(int x, int y, int sextant) {
    auto coins = coinsDeCase(x, y, sextant);
    return (coins[0] + coins[1] + coins[2] + coins[3]) / 4.0;
}

// Symbole Unicode pour chaque type de piece (jeu de pieces pleines)
static QString symbolePiece(const std::string& type) {
    if (type == "Roi")      return QString::fromUtf8("♚");  // ♚
    if (type == "Reine")    return QString::fromUtf8("♛");  // ♛
    if (type == "Tour")     return QString::fromUtf8("♜");  // ♜
    if (type == "Fou")      return QString::fromUtf8("♝");  // ♝
    if (type == "Cavalier") return QString::fromUtf8("♞");  // ♞
    if (type == "Pion")     return QString::fromUtf8("♟");  // ♟
    return QString("?");
}

VueJeu::VueJeu(std::shared_ptr<ModeleJeu> modele, QWidget* parent)
    : QMainWindow(parent), modele(modele),
      scene(new QGraphicsScene(this)),
      vue(new QGraphicsView(scene, this)),
      labelJoueur(new QLabel("Joueur : -", this)),
      labelEtat(new QLabel("Etat : EN COURS", this)),
      caseSelectionnee(nullptr) {
    configurerInterface();
}

void VueJeu::configurerInterface() {
    setWindowTitle("Bienvenue sur le jeu Yalta !");
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

void VueJeu::mettreAJour(const ModeleJeu& modeleRef) {
    auto joueur = modeleRef.getJoueurActuel();
    if (joueur) {
        labelJoueur->setText(QString("Joueur : %1")
            .arg(QString::fromStdString(joueur->getNom())));
    }

    switch (modeleRef.getEtat()) {
        case EtatPartie::EN_COURS:     labelEtat->setText("Etat : EN COURS");     break;
        case EtatPartie::ECHEC:        labelEtat->setText("Etat : ECHEC !");      break;
        case EtatPartie::ECHEC_ET_MAT: labelEtat->setText("Etat : ECHEC ET MAT"); break;
        case EtatPartie::PAT:          labelEtat->setText("Etat : PAT");          break;
        case EtatPartie::NULLE:        labelEtat->setText("Etat : NULLE");        break;
    }

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

    // Couleurs damier — sombres comme le code Python (bois fonce)
    const QColor COULEUR_FONCE(54, 39, 32);
    const QColor COULEUR_CLAIR(229, 210, 170);

    for (const auto& c : cases) {
        int x  = c->getX();
        int y  = c->getY();
        int sx = c->getSextant();

        auto coins = coinsDeCase(x, y, sx);
        QPolygonF quad;
        quad << coins[0] << coins[1] << coins[2] << coins[3];

        QColor couleur = (c->getCouleur() == "clair") ? COULEUR_CLAIR : COULEUR_FONCE;
        QPen pen(QColor(20, 15, 10), 1);

        // surbrillance : selection ou coup possible
        if (caseSelectionnee
            && caseSelectionnee->getX() == x
            && caseSelectionnee->getY() == y) {
            couleur = QColor(255, 230, 80);
            pen = QPen(QColor(200, 150, 0), 3);
        } else {
            for (const auto& dest : coupsPossibles) {
                if (dest->getX() == x && dest->getY() == y) {
                    if (c->estOccupee()) {
                        couleur = QColor(220, 90, 90);    // capture
                        pen = QPen(QColor(150, 30, 30), 3);
                    } else {
                        couleur = QColor(150, 220, 140);  // deplacement
                        pen = QPen(QColor(60, 140, 60), 3);
                    }
                    break;
                }
            }
        }

        auto* item = scene->addPolygon(quad, pen, QBrush(couleur));
        item->setData(0, x);
        item->setData(1, y);
        item->setToolTip(QString("sextant=%1  (x=%2, y=%3)").arg(sx).arg(x).arg(y));
    }

    // Cadre exterieur du hexagone
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
                fond    = QColor(255, 255, 255);
                contour = QColor(0, 0, 0);
                break;
            case Couleur::NOIR:
                fond    = QColor(30, 30, 30);
                contour = QColor(220, 220, 220);
                break;
            case Couleur::ROUGE:
                fond    = QColor(214, 21, 65);
                contour = QColor(40, 0, 0);
                break;
        }

        auto* texte = scene->addSimpleText(symbolePiece(piece->getType()), policePiece);
        texte->setBrush(QBrush(fond));
        texte->setPen(QPen(contour, 1.2));

        QRectF br = texte->boundingRect();
        texte->setPos(centre.x() - br.width()  / 2.0,
                      centre.y() - br.height() / 2.0);
    }
}

void VueJeu::afficherAlerteEchec(const std::string& nomJoueur) {
    QMessageBox::warning(this, "Echec !",
        QString("Le roi de %1 est en echec !")
            .arg(QString::fromStdString(nomJoueur)));
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
