#include "npp.h"
#include "emitter.h"
#include "qt_mainwindow.h"
#include <QPainter>
#include <QtCore/qmath.h>
#include <QGraphicsScene>
#include <QLinearGradient>
#include <QHash>
#include "tilebag.h"

qreal delay = 1.4; // delay per pixel

QPointF mulp(QPointF a, QPointF b)
{
    return QPointF(a.x() * b.x(), a.y() * b.y());
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

QRectF calculate_bbox(QPointF from, QPointF to, int margin)
{
    qreal x1 = MIN(from.x(), to.x()) - margin;
    qreal y1 = MIN(from.y(), to.y()) - margin;
    qreal x2 = MAX(from.x(), to.x()) + margin;
    qreal y2 = MAX(from.y(), to.y()) + margin;
    return QRectF(x1, y1, x2 - x1, y2 - y1);
}

BeamAnimation::BeamAnimation(QPointF from, QPointF to, int new_gf_type)
{
    gf_type = new_gf_type;
    cloud_color = defined_colors[gf_color(gf_type) % MAX_COLORS];

    from = getCenter(from.y(), from.x());
    to = getCenter(to.y(), to.x());

    QRectF bbox = calculate_bbox(from, to, 100);
    brect = QRectF(0, 0, bbox.width(), bbox.height());
    setPos(bbox.x(), bbox.y());

    p1 = from - pos();
    p2 = to - pos();

    anim = new QPropertyAnimation(this, "length");
    qreal l = QLineF(p1, p2).length();
    int dur = l * delay;
    if (dur < 250) dur = 250;
    anim->setDuration(dur);
    anim->setStartValue(0);
    anim->setEndValue(50);
    connect(anim, SIGNAL(finished()), this, SLOT(deleteLater()));
    this->setVisible(false);
    setZValue(300);
}

class BeamPoint
{
public:
    QPointF point;
    qreal length;
    BeamPoint(QPointF newPoint, QPointF origin);
    bool operator <(const BeamPoint &b) const;
};

BeamPoint::BeamPoint(QPointF newPoint, QPointF origin)
{
    point = newPoint;
    if (point == origin) {
        length = 0;
    }
    else {
        length = QLineF(origin, point).length();
    }
}

bool BeamPoint::operator <(const BeamPoint &b) const
{
    return length < b.length;
}

void make_beam_aux(QPointF from, QPointF to, QList<QPointF> *points, qreal displace, qreal detail)
{
    if (displace < detail) {
        points->append(to);
    }
    else {
        qreal mid_x = (from.x() + to.x()) / 2;
        qreal mid_y = (from.y() + to.y()) / 2;
        mid_x += (rand_int(100) / 100.0 - 0.5) * displace;
        mid_y += (rand_int(100) / 100.0 - 0.5) * displace;
        QPointF mid(mid_x, mid_y);
        make_beam_aux(from, mid, points, displace / 2, detail);
        make_beam_aux(to, mid, points, displace / 2, detail);
    }
}

QPolygonF make_beam(QPointF from, QPointF to)
{
    QList<QPointF> points;
    points.append(from);
    points.append(to);
    make_beam_aux(from, to, &points, 70, 5);
    QList<BeamPoint> bp;
    for (int i = 0; i < points.size(); i++) {
        bp.append(BeamPoint(points.at(i), from));
    }
    qSort(bp); // Sort it by distance to the source
    QPolygonF poly;
    for (int i = 0; i < bp.size(); i++) {
        poly.append(bp.at(i).point);
    }
    return poly;
}

static int BALL_TILE_SIZE = 40;

QPolygonF get_cloud_points(QPointF from, QPointF to, qreal step)
{
    QPolygonF poly;

    QLineF line(from, to);
    qreal l = line.length();
    while (l > 0) {
        line.setLength(l);
        poly.append(line.p2());
        l -= step;
    }
    return poly;
}

void BeamAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    bool do_beam = false;

    QPixmap pix = tiles_projections->get_tile("ball1.png");
    pix = colorize_pix2(pix, cloud_color);
    int bs = pix.width();

    QLineF line(p1, p2);

    if (line.length() > 50) do_beam = true;

    // Monster uses a whip
    if (gf_type == GF_ARROW) do_beam = false;

    QPolygonF beam;

    painter->setOpacity(0.5);

    if (do_beam) {
        beam = make_beam(p1, p2);
        for (int i = 1; i < beam.size(); i++) {
            QPolygonF poly = get_cloud_points(beam.at(i - 1), beam.at(i), 10);
            for (int j = 1; j < poly.size(); j++) {
                QPointF p = poly.at(j);
                p -= QPointF(bs/2, bs/2);
                painter->drawPixmap(p, pix);
            }
        }
    }
    else {
        QPolygonF poly = get_cloud_points(p1, p2, 10);
        for (int j = 1; j < poly.size(); j++) {
            QPointF p = poly.at(j);
            p -= QPointF(bs/2, bs/2);
            painter->drawPixmap(p, pix);
        }
    }

    painter->setOpacity(1);
    color = defined_colors[gf_color(gf_type) % MAX_COLORS];
    QPen pen(QBrush(color), 2, Qt::SolidLine,
             Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);

    if (do_beam) {
        painter->drawPolyline(beam);
    }
    else {
        painter->drawLine(p1, p2);
    }

    painter->restore();
}

QRectF BeamAnimation::boundingRect() const
{
    return brect;
}

BeamAnimation::~BeamAnimation()
{
    if (scene()) scene()->removeItem(this);
}

qreal BeamAnimation::getLength()
{
    return length;
}

void BeamAnimation::setLength(qreal newLength)
{
    length = newLength;

    this->setVisible(true);

    this->update();
}

NPPAnimation::NPPAnimation()
{
    anim = 0;
    next = 0;
}

NPPAnimation::~NPPAnimation()
{
    if (next) next->start();
    main_window->stop_loop();
    if (anim) delete anim;
}

void NPPAnimation::start()
{
    if (anim) {
        anim->start();
        main_window->start_loop();
    }
}

BoltAnimation::BoltAnimation(QPointF from, QPointF to, int new_gf_type, u32b new_flg, object_type *o_ptr)
{
    flg = new_flg;
    gf_type = new_gf_type;
    if (gf_type > 0) color = defined_colors[gf_color(gf_type) % MAX_COLORS];

    from = getCenter(from.y(), from.x());
    to = getCenter(to.y(), to.x());
    current_angle = QLineF(from, to).angle();

    bool do_default = true;

    if (o_ptr != 0) {
        do_default = false;

        object_kind *k_ptr = k_info + o_ptr->k_idx;
        QChar chr = k_ptr->d_char;
        QColor col = k_ptr->d_color;
        QString key = k_ptr->tile_id;
        if (k_ptr->flavor > 0) {
            flavor_type *fl_ptr = flavor_info + k_ptr->flavor;
            chr = fl_ptr->d_char;
            col = fl_ptr->d_color;
            key = fl_ptr->tile_id;
        }

        if (use_graphics) {
            main_window->rebuild_tile(key);
            pix = main_window->tiles.value(key);
        }
        else {
            pix = pseudo_ascii(chr, col, main_window->cur_font,
                               QSizeF(main_window->cell_wid + 10,
                                      main_window->cell_hgt + 10));
        }
    }
    else if (gf_type == GF_ARROW) {
        if (flg & PROJECT_ROCK) {
            pix = tiles_projections->get_tile("boulder1.png");
            do_default = false;
        }
        else if (flg & PROJECT_SHOT) {
            pix = tiles_projections->get_tile("shot1.png");
            do_default = false;
        }
        else if (flg & PROJECT_AMMO) {
            pix = tiles_projections->get_tile("arrow1.png");
            pix = rotate_pix(pix, current_angle);
            do_default = false;
        }
    }

    if (do_default) {
        pix = rotate_pix(tiles_projections->get_tile("bolt1.png"), current_angle);
        pix = colorize_pix3(pix, color);
    }

    setVisible(false);
    setZValue(300);

    anim = new QPropertyAnimation(this, "pos");   

    int d = QLineF(from, to).length();
    int dur = d * delay * 1.2; // +20%
    if (dur < 250) dur = 250;  // minimum    
    anim->setDuration(dur);    

    QPointF adj = QPointF(pix.width() / 2, pix.height() / 2);
    anim->setStartValue(from - adj);
    anim->setEndValue(to - adj);

    connect(anim, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void BoltAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    painter->drawPixmap(0, 0, pix);

    painter->restore();
}

QRectF BoltAnimation::boundingRect() const
{
    return QRectF(0, 0, pix.width(), pix.height());
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

BallAnimation::BallAnimation(QPointF where, int newRadius, int newGFType, u32b flg)
{
    gf_type = newGFType;
    color = defined_colors[gf_color(gf_type) % MAX_COLORS];

    setZValue(1000);
    setVisible(false);

    int size = (newRadius * 2 + 1 + 5); // 5 extra

    maxLength = (size * main_window->cell_hgt) * 0.5;

    length = previousLength = 0;

    QPointF center = getCenter(where.y(), where.x());

    QPointF p3 = center - QPointF(maxLength, maxLength);
    setPos(p3);

    brect = QRectF(0, 0, maxLength * 2, maxLength * 2);

    // Collect valid grids
    QPoint p1 = to_dungeon_coord(this, QPoint(0, 0));
    QPoint p2 = to_dungeon_coord(this, QPoint(maxLength * 2, maxLength * 2));

    for (int y = p1.y(); y <= p2.y(); y++) {
        for (int x = p1.x(); x <= p2.x(); x++) {
            if (!in_bounds(y, x)) continue;
            int gr = GRID(y, x);
            bool value = false;
            if (flg & PROJECT_PASS) value = true;
            else if (cave_project_bold(y, x) &&
                    generic_los(where.y(), where.x(), y, x, CAVE_PROJECT)) value = true;
            valid.insert(gr, value);
        }
    }

    position = center - pos();

    anim = new QPropertyAnimation(this, "length");
    anim->setDuration(500);
    anim->setStartValue(0);
    maxLength = ((newRadius * 2 + 1) * main_window->cell_hgt) * 0.5;
    anim->setEndValue(maxLength);
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

    QPixmap pix = colorize_pix2(tiles_projections->get_tile("ball1.png"), color);

    for (int i = 0; i < particles.size(); i++) {
        BallParticle *p = particles.at(i);
        QPointF where = position + fromAngle(p->angle, p->currentLength);

        QPoint p1 = to_dungeon_coord(this, where.toPoint());
        int gr = GRID(p1.y(), p1.x());
        if (!valid.contains(gr) || !valid.value(gr)) continue;

        if (p->type == 0) {
            qreal opacity = 1;
            if (p->currentLength > maxLength / 2.0) opacity = 0.5;
            painter->setOpacity(opacity);
            painter->drawPixmap(where.x() - pix.width() / 2,
                                where.y() - pix.height() / 2,
                                pix);
        }
        else {
            painter->setOpacity(1);
            painter->fillRect(QRectF(where.x(), where.y(), 1, 1), color);
        }
    }

    painter->restore();
}

QRectF BallAnimation::boundingRect() const
{
    return brect;
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

ArcAnimation::ArcAnimation(QPointF from, QPointF to, int newDegrees, int type, int newRad, u32b flg)
{
    gf_type = type;
    byte idx = gf_color(gf_type);
    color = defined_colors[idx % MAX_COLORS];

    QPixmap pix = tiles_projections->get_tile("ball1.png");
    pix = colorize_pix2(pix, color);

    tiles.append(pix);
    for (qreal adj = 1.5; adj < 3; adj += 0.5) {
        tiles.append(pix.scaled(pix.width() * adj, pix.height() * adj));
    }

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
            if (flg & PROJECT_PASS) value = true;
            else if (cave_flag_bold(y, x, CAVE_PROJECT) &&
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
    drawnLength = length = previousLength = 0;

    degrees = newDegrees;

    setZValue(300);
    setVisible(false);

    timer.setInterval(delay * 20);
    connect(&timer, SIGNAL(timeout()), this, SLOT(do_timeout()));
}

void ArcAnimation::start()
{
    timer.start();
    main_window->start_loop();
}

void ArcAnimation::finish()
{
    this->setVisible(false);
    timer.stop();
    main_window->stop_loop();
    this->deleteLater();
}

void ArcAnimation::do_timeout()
{
    qreal delta = 40;
    if (length == 0) delta = 5;

    length += delta;

    if (length > maxLength) {
        finish();
        return;
    }

    setVisible(true);

    previousLength = length;

    int n = degrees * 10 / 30; // 10 particles every 30 degrees
    if (n < 10) n = 10;        // minimum

    for (int i = 0; i < n; i++) {
        BallParticle *p = new BallParticle;
        qreal angle = rand_int(degrees) - degrees / 2.0;
        angle = angle * PI / 180;
        p->angle = centerAngle + angle;
        p->currentLength = 0;
        p->type = rand_int(7);
        particles.append(p);
    }

    for (int i = 0; i < particles.size(); i++) {
        BallParticle *p = particles.at(i);

        if ((p->currentLength > 0) || (delta < 40)) {
            p->currentLength += delta;
        }
        else {
            p->currentLength = 20 + rand_int(30);
        }
    }

    update();
}

int get_pix_index(int size, qreal percent)
{
    qreal upper = 1.0 / size;
    qreal current = upper;

    for (int i = 0; i < size; i++) {
        if (percent < current) return i;
        current += upper;
    }

    return size - 1;
}

void ArcAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    qreal max = 0;

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

        int idx = get_pix_index(tiles.size(), p->currentLength / maxLength);
        QPixmap pix2 = tiles.at(idx);

        pp -= QPointF(pix2.width() / 2, pix2.height() / 2);

        painter->drawPixmap(pp, pix2);

        if (p->currentLength > max) max = p->currentLength;
    }

    painter->restore();

    // Stop animation if we can't draw particles anymore
    if (max <= drawnLength) {
        finish();
    }
    else {
        drawnLength = max;
    }
}

QRectF ArcAnimation::boundingRect() const
{
    return brect;
}

StarAnimation::StarAnimation(QPointF newCenter, int radius, int newGFType, int gy[], int gx[], int grids)
{
    gf_type = newGFType;

    while (--grids >= 0) {
        int gr = GRID(gy[grids], gx[grids]);
        valid.insert(gr, true);
    }

    QColor color = defined_colors[gf_color(gf_type) % MAX_COLORS];
    pix = colorize_pix3(tiles_projections->get_tile("star1.png"), color);

    this->setVisible(false);
    this->setZValue(1000);

    QPointF adj(radius + 5, radius + 5); // 5 extra
    QPointF p1 = newCenter - adj;
    QPointF p2 = newCenter + adj;

    p1 = getCenter(p1.y(), p1.x());
    p2 = getCenter(p2.y(), p2.x());
    brect = QRectF(0, 0, p2.x() - p1.x(), p2.y() - p1.y());
    this->setPos(p1);

    center = getCenter(newCenter.y(), newCenter.x()) - p1;

    length = previousLength = 0;

    maxLength = main_window->cell_hgt * (radius + 0.5);

    timer.setInterval(40);
    connect(&timer, SIGNAL(timeout()), this, SLOT(do_timeout()));
}

void StarAnimation::start()
{
    timer.start();
    main_window->start_loop();
}

void StarAnimation::stop()
{
    timer.stop();
    main_window->stop_loop();
    this->setVisible(false);
    this->scene()->removeItem(this);
    for (int i = 0; i < particles.size(); i++) {
        delete particles.at(i);
    }
    particles.clear();
    this->deleteLater();
}

void StarAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    for (int i = 0; i < particles.size(); i++) {
        BallParticle *p = particles.at(i);
        QLineF line = QLineF::fromPolar(p->currentLength, p->angle);
        QPointF point = center + line.p2();

        QPoint coord = to_dungeon_coord(this, point.toPoint());
        int gr = GRID(coord.y(), coord.x());
        if (!valid.contains(gr)) continue;

        QPointF adj(pix.width() / 2, pix.height() / 2);
        point -= adj;

        painter->drawPixmap(point, pix);
    }

    painter->restore();
}

QRectF StarAnimation::boundingRect() const
{
    return brect;
}

void StarAnimation::do_timeout()
{
    length += 10;

    if (length > maxLength) {
        this->stop();
        return;
    }

    this->setVisible(true);

    for (int i = 0; i < 15; i++) {
        BallParticle *p = new BallParticle;
        p->currentLength = 0;
        p->type = 0;
        p->angle = rand_int(360);
        particles.append(p);
    }

    for (int i = 0; i < particles.size(); i++) {
        BallParticle *p = particles.at(i);
        if (p->currentLength > 0) {
            p->currentLength += 10;
        }
        else {
            p->currentLength = 4 + rand_int(8);
        }
    }

    this->update();
}

HaloAnimation::HaloAnimation(int y, int x)
{
    QPointF center = getCenter(y, x);

    curLength = 20;

    haloPix = tiles_projections->get_tile("big_halo.png");

    maxLength = haloPix.width();

    QPointF adj(maxLength / 2, maxLength / 2);

    setPos(center - adj);

    c_y = adj.y();
    c_x = adj.x();

    timer.setInterval(70);

    connect(&timer, SIGNAL(timeout()), this, SLOT(do_timeout()));

    this->setVisible(false);
}

void HaloAnimation::start()
{
    timer.start();
    main_window->start_loop();
}

void HaloAnimation::stop()
{
    timer.stop();
    main_window->stop_loop();
    if (scene()) scene()->removeItem(this);
    this->deleteLater();
}

void HaloAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPointF adj(curPix.width() / 2, curPix.height() / 2);
    QPointF center(c_x, c_y);
    center -= adj;
    painter->drawPixmap(center, curPix);
}

QRectF HaloAnimation::boundingRect() const
{
    return QRectF(0, 0, maxLength, maxLength);
}

void HaloAnimation::do_timeout()
{
    curLength += 30;

    if (curLength > maxLength) {
        stop();
        return;
    }

    this->setVisible(true);

    curPix = haloPix.scaled(curLength, curLength);

    update();
}
