#include "emitter.h"
#include "qt_mainwindow.h"
#include "npp.h"
#include <QPainter>
#include <QtCore/qmath.h>
#include <QGraphicsScene>

static QPixmap *ball_pix = 0;
static QPixmap *bolt_pix = 0;

QPointF mulp(QPointF a, QPointF b)
{
    return QPointF(a.x() * b.x(), a.y() * b.y());
}

static void load_ball_pix()
{
    if (!ball_pix) {
        QString path(NPP_DIR_GRAF);
        path.append("ball1.png");
        ball_pix = new QPixmap(path);
    }
}

static void load_bolt_pix()
{
    if (!bolt_pix) {
        QString path(NPP_DIR_GRAF);
        path.append("bolt1.png");
        bolt_pix = new QPixmap(path);
    }
}

static double PI = 3.141592653589793238463;

static qreal magnitude(QPointF vec)
{
    return qSqrt(vec.x() * vec.x() + vec.y() * vec.y());
}

static qreal getAngle(QPointF vec)
{
    return qAtan2(vec.y(), vec.x());
}

static QPointF fromAngle(qreal angle, qreal magnitude)
{
    return QPointF(magnitude * qCos(angle), magnitude * qSin(angle));
}

static QPointF getCenter(int y, int x)
{
    QSize dim = ui_grid_size();
    return QPointF(x * dim.width() + dim.width() / 2,
                   y * dim.height() + dim.height() / 2);
}

NPPAnimation::NPPAnimation()
{
    anim = 0;
    next = 0;
}

NPPAnimation::~NPPAnimation()
{
    if (next) next->start();
    else if (main_window->ev_loop.isRunning()) main_window->ev_loop.quit();
    if (anim) delete anim;
}

void NPPAnimation::start()
{
    if (anim) {
        anim->start();
        if (!main_window->ev_loop.isRunning()) main_window->ev_loop.exec();
    }
}

static int BOLT_SIZE = 20;

BoltAnimation::BoltAnimation(QPointF from, QPointF to, int new_gf_type)
{
    gf_type = new_gf_type;
    byte idx = gf_color(gf_type);
    color = defined_colors[idx % MAX_COLORS];
    current_angle = 0;
    setVisible(false);
    setZValue(300);
    anim = new QPropertyAnimation(this, "pos");
    int d = magnitude(mulp(to - from, QPointF(main_window->cell_wid, main_window->cell_hgt)));
    int dur = (d * 1500 / 600); // 1 second every 600 pixels
    if (dur < 300) dur = 300;  // minimum
    anim->setDuration(dur);
    anim->setStartValue(getCenter(from.y(), from.x()) - QPointF(BOLT_SIZE / 2, BOLT_SIZE / 2));
    anim->setEndValue(getCenter(to.y(), to.x()) - QPointF(BOLT_SIZE / 2, BOLT_SIZE / 2));
    connect(anim, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void BoltAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    load_bolt_pix();

    current_angle += 5;
    current_angle %= 360;

    if (false && one_in_(5)) {
        byte idx = gf_color(gf_type);
        color = defined_colors[idx % MAX_COLORS];
    }

    QPixmap pix = rotate_pix(*bolt_pix, current_angle);
    pix = colorize_pix2(pix, color);
    painter->drawPixmap(boundingRect(), pix, boundingRect());

    painter->restore();
}

QRectF BoltAnimation::boundingRect() const
{
    return QRectF(0, 0, BOLT_SIZE, BOLT_SIZE);
}

void BoltAnimation::start()
{
    setVisible(true);
    NPPAnimation::start();
}

BoltAnimation::~BoltAnimation()
{
    if (scene()) scene()->removeItem(this);
}

static int BALL_TILE_SIZE = 40;

BallAnimation::BallAnimation(QPointF where, int newRadius)
{
    setZValue(300);
    setVisible(false);

    load_ball_pix();

    QSize size_temp = ui_grid_size();
    size = size_temp.height();
    size *= (newRadius * 2 + 1);

    length = previousLength = 0;

    int size2 = size + BALL_TILE_SIZE;

    where = getCenter(where.y(), where.x());

    setPos(where.x() - size2 / 2, where.y() - size2 / 2);

    // Collect valid grids
    QPoint p1 = to_dungeon_coord(this, pos().toPoint());
    QPoint p2 = p1 + QPoint(size2, size2);

    for (int y = p1.y(); y <= p2.y(); y++) {
        for (int x = p1.x(); x <= p2.x(); x++) {
            if (!in_bounds(y, x)) continue;
            int gr = GRID(y, x);
            bool value = false;
            if ((dungeon_info[y][x].cave_info & (CAVE_SEEN)) &&
                    generic_los(where.y(), where.x(), y, x, CAVE_PROJECT)) value = true;
            valid.insert(gr, value);
        }
    }

    position = QPointF(size2 / 2, size2 / 2);

    anim = new QPropertyAnimation(this, "length");
    anim->setDuration(1000);
    anim->setStartValue(0);
    anim->setEndValue(size / 2.0);
    connect(anim, SIGNAL(finished()), this, SLOT(deleteLater()));
}

qreal BallAnimation::getLength()
{
    return length;
}

void BallAnimation::setLength(qreal newLength)
{
    length = newLength;

    if (length < previousLength + 4) return;

    setVisible(true);

    qreal delta = length - previousLength;

    previousLength = length;

    for (int i = 0; (i < 25); i++) {
        qreal angle = rand_int(360) * 2 * PI / 360;
        BallParticle *p = new BallParticle;
        p->type = rand_int(3);
        p->angle = angle;
        p->currentLength = 0;
        particles.append(p);
    }

    for (int i = 0; i < particles.size(); i++) {
        BallParticle *p = particles.at(i);
        p->currentLength += delta;
    }

    update();
}

BallAnimation::~BallAnimation()
{
    for (int i = 0; i < particles.size(); i++) {
        delete particles.at(i);
    }
    particles.clear();

    if (scene()) scene()->removeItem(this);
}

void BallAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();    

    for (int i = 0; i < particles.size(); i++) {
        BallParticle *p = particles.at(i);
        QPointF where = position + fromAngle(p->angle, p->currentLength);

        QPoint p1 = to_dungeon_coord(this, where.toPoint());
        int gr = GRID(p1.y(), p1.x());
        if (!valid.contains(gr) || !valid.value(gr)) continue;

        QColor col("white");

        if (p->type == 0) {
            qreal opacity = 1;
            if (p->currentLength > size / 4.0) opacity = 0.5;
            painter->setOpacity(opacity);
            painter->drawPixmap(where.x() - BALL_TILE_SIZE / 2,
                                where.y() - BALL_TILE_SIZE / 2,
                                BALL_TILE_SIZE, BALL_TILE_SIZE, *ball_pix);
        }
        else {
            painter->setOpacity(1);
            painter->fillRect(QRectF(where.x(), where.y(), 1, 1), col);
        }
    }

    painter->restore();
}

QRectF BallAnimation::boundingRect() const
{
    return QRectF(0, 0, size + BALL_TILE_SIZE, size + BALL_TILE_SIZE);
}

ArcAnimation::~ArcAnimation()
{
    for (int i = 0; i < particles.size(); i++) {
        delete particles.at(i);
    }
    particles.clear();

    if (scene()) scene()->removeItem(this);
}

static int ARC_TILE_SIZE = 40;

ArcAnimation::ArcAnimation(QPointF from, QPointF to, int newDegrees, int type, int newRad)
{
    gf_type = type;
    byte idx = gf_color(gf_type);
    color = defined_colors[idx % MAX_COLORS];

    rad = newRad;

    QPointF pp(main_window->cell_wid, main_window->cell_hgt);
    QPointF p1(from.x() - rad - 5, from.y() - rad - 5); // +-5 extra
    QPointF p2(from.x() + rad + 5, from.y() + rad + 5);

    // Collect valid grids
    for (int y = p1.y(); y <= p2.y(); y++) {
        for (int x = p1.x(); x <= p2.x(); x++) {
            if (!in_bounds(y, x)) continue;
            int gr = GRID(y, x);
            bool value = false;
            if (cave_flag_bold(y, x, CAVE_SEEN) &&
                    cave_flag_bold(y, x, CAVE_PROJECT) &&
                    generic_los(from.y(), from.x(), y, x, CAVE_PROJECT)) value = true;
            valid.insert(gr, value);
        }
    }

    brect = QRectF(0, 0, (p2.x() - p1.x() + 1) * main_window->cell_wid,
                         (p2.y() - p1.y() + 1) * main_window->cell_hgt);

    setPos(mulp(p1, pp));

    position = mulp(from - p1, pp) + (pp * 0.5);

    QPointF h = mulp(to - from, pp);
    centerAngle = getAngle(h);
    maxLength = rad * MAX(main_window->cell_wid, main_window->cell_hgt);
    length = previousLength = 0;

    degrees = newDegrees;

    anim = new QPropertyAnimation(this, "length");
    anim->setStartValue(0);
    anim->setEndValue(maxLength);
    int duration = (maxLength / 600.0) * 1000; // 1 Second every 600 pixels
    if (duration < 500) duration = 500; // Minimum
    anim->setDuration(duration);
    connect(anim, SIGNAL(finished()), this, SLOT(deleteLater()));

    setZValue(300);
    setVisible(false);
}

qreal ArcAnimation::getLength()
{
    return length;
}

void ArcAnimation::setLength(qreal newLength)
{
    length = newLength;

    if (length < previousLength + 4) return;

    setVisible(true);

    qreal delta = length - previousLength;

    previousLength = length;

    int n = degrees * 10 / 30; // 10 particles every 30 degrees
    if (n < 10) n = 10;        // minimum

    for (int i = 0; i < n; i++) {
        BallParticle *p = new BallParticle;
        qreal angle = rand_int(degrees) - degrees / 2.0;
        angle = angle * PI / 180;
        p->angle = centerAngle + angle;
        p->currentLength = 4;
        p->type = rand_int(7);
        particles.append(p);
    }

    for (int i = 0; i < particles.size(); i++) {
        BallParticle *p = particles.at(i);

        p->currentLength += delta;

        // Randomness
        p->currentLength += (delta * 0.25 * (1 - rand_int(3)));
    }

    update();
}

void ArcAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    load_ball_pix();

    painter->save();

    QPixmap pix = *ball_pix;
    pix = colorize_pix2(pix, color);

    for (int i = 0; i < particles.size(); i++) {
        BallParticle *p = particles.at(i);

        QPointF pp = position + fromAngle(p->angle, p->currentLength);

        QPoint p1 = to_dungeon_coord(this, pp.toPoint());
        int gr = GRID(p1.y(), p1.x());
        if (!valid.contains(gr) || !valid.value(gr)) continue;

        qreal opacity = 1;
        opacity = 1 - p->currentLength / maxLength;
        if (opacity < 0.3) opacity = 0.3;
        painter->setOpacity(opacity);

        qreal perc = 1;
        int cl = p->currentLength / 40;   // Enlarge pix
        perc += (0.1 * cl);               // Just 10% every 40 pixels
        QPixmap pix2 = pix.scaled(pix.width() * perc, pix.height() * perc);

        pp -= QPointF(pix2.width() / 2, pix2.height() / 2);

        painter->drawPixmap(pp, pix2);
    }

    painter->restore();
}

QRectF ArcAnimation::boundingRect() const
{
    return brect;
}
