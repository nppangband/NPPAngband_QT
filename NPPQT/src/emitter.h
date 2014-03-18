#ifndef EMITTER_H
#define EMITTER_H

#include <QObject>
#include <QPointF>
#include <QList>
#include <QGraphicsItem>
#include <QPropertyAnimation>
#include <QHash>
#include <QTimer>

class NPPAnimation: public QObject
{
    Q_OBJECT
public:
    NPPAnimation();

    QPropertyAnimation *anim;
    NPPAnimation *next;

    virtual ~NPPAnimation();

    virtual void start();
};

class BeamAnimation: public NPPAnimation, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(qreal length READ getLength WRITE setLength)
public:
    qreal length;
    int gf_type;
    QColor color;
    QColor cloud_color;
    QRectF brect;
    QPointF p1, p2;

    BeamAnimation(QPointF from, QPointF to, int new_gf_type);
    qreal getLength();
    void setLength(qreal newLength);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    virtual ~BeamAnimation();
};

class BoltAnimation: public NPPAnimation, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    int current_angle;
    int gf_type;
    QColor color;

    BoltAnimation(QPointF from, QPointF to, int new_gf_type);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    virtual ~BoltAnimation();

    void start();
};

class BallParticle
{
public:
    qreal angle;
    qreal currentLength;
    int type;
};

class BallAnimation: public NPPAnimation, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(qreal length READ getLength WRITE setLength)
public:
    QList<BallParticle *> particles;
    QPointF position;
    qreal length;
    qreal previousLength;
    qreal maxLength;
    QRectF brect;
    QHash<int, bool> valid; // Grids in los and projectable
    int gf_type;
    QColor color;

    BallAnimation(QPointF where, int newRadius, int newGFType);
    qreal getLength();
    void setLength(qreal newLength);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    virtual ~BallAnimation();
};

class ArcAnimation: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    QList<BallParticle *> particles;
    QPointF position;
    qreal length;
    qreal previousLength;
    qreal drawnLength;
    int degrees;
    QRectF brect;
    qreal centerAngle;
    qreal maxLength;
    QHash<int, bool> valid; // Grids in los and projectable
    int gf_type;
    QColor color;
    int rad;
    QTimer timer;
    QList<QPixmap> tiles;

    ArcAnimation(QPointF from, QPointF to, int newDegrees, int type, int newRad);
    void start();
    void finish();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    virtual ~ArcAnimation();
public slots:
    void do_timeout();
};

class StarAnimation: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    QTimer timer;
    qreal length;
    qreal previousLength;
    qreal maxLength;
    QPixmap pix;
    QPointF center;
    QRectF brect;
    QList<BallParticle*> particles;
    int gf_type;
    QHash<int, bool> valid;

    StarAnimation(QPointF newCenter, int radius, int newGFType, int gy[], int gx[], int grids);

    void start();
    void stop();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

public slots:
    void do_timeout();
};

#endif // EMITTER_H
