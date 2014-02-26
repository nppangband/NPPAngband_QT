#ifndef EMITTER_H
#define EMITTER_H

#include <QObject>
#include <QPointF>
#include <QList>
#include <QGraphicsItem>
#include <QPropertyAnimation>

class NPPAnimation: public QObject
{
    Q_OBJECT
public:
    NPPAnimation();

    QPropertyAnimation *anim;
    NPPAnimation *next;

    virtual ~NPPAnimation();

    void start();
};

class BoltAnimation: public NPPAnimation, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
public:
    BoltAnimation(QPointF from, QPointF to);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    virtual ~BoltAnimation();
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
    qreal size;

    BallAnimation(QPointF where, int newRadius);
    qreal getLength();
    void setLength(qreal newLength);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    virtual ~BallAnimation();
};

#endif // EMITTER_H
