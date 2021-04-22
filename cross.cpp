#include <QtGui>

#include "qdigiplot.h"
#include "view.h"
#include "scene.h"
#include "cross.h"

using namespace std;


Cross::Cross(QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsItem(parent, scene)
{
}

QRectF Cross::boundingRect() const
{
    return QRectF(-5, -5, 5, 5);
}

void Cross::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor = Qt::green;
    if (option->state & QStyle::State_MouseOver)
        fillColor = Qt::red;

    QPen pen(fillColor);
    painter->setPen(pen);

    QVector<QLine> lineVec(2);
    lineVec.append(QLine(-5, -5, 5, 5));
    lineVec.append(QLine(5, -5, -5, 5));
    painter->drawLines(lineVec);
}


CrossX::CrossX(QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsItem(parent, scene)
{
}

QRectF CrossX::boundingRect() const
{
    return QRectF(-4, -4, 4, 4);
}

void CrossX::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor = Qt::blue;
    QPen pen(fillColor);
    painter->setPen(pen);

    QVector<QLine> lineVec(2);
    lineVec.append(QLine(0, -4, 0, 4));
    lineVec.append(QLine(4, 0, -4, 0));
    painter->drawLines(lineVec);
}


CrossY::CrossY(QGraphicsItem *parent, QGraphicsScene *scene) : QGraphicsItem(parent, scene)
{
}

QRectF CrossY::boundingRect() const
{
    return QRectF(-4, -4, 4, 4);
}

void CrossY::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor = Qt::magenta;
    QPen pen(fillColor);
    painter->setPen(pen);

    QVector<QLine> lineVec(2);
    lineVec.append(QLine(0, -4, 0, 4));
    lineVec.append(QLine(4, 0, -4, 0));
    painter->drawLines(lineVec);
}