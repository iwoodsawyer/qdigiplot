#include "qdigiplot.h"
#include "scene.h"
#include "cross.h"
#include "view.h"

#include <stdlib.h>

using namespace std;

MyGraphicsScene::MyGraphicsScene(QObject* parent) : QGraphicsScene(parent)
{
    defXaxis = true;
    defYaxis = true;
    defCross = true;
    defX0 = false;
    defX1 = false;
    defY0 = false;
    defY1 = false;
}

void MyGraphicsScene::clearXAxis()
{
    view->xAxisButton->setEnabled(false);
    if (defX1)
    {
        removeItem(textX1);
        removeItem(crossX1);
    }
    if (defX0)
    {
        removeItem(lineX);
        removeItem(textX0);
        removeItem(crossX0);
    }
    defXaxis = true;
    defX0 = false;
    defX1 = false;
}

void MyGraphicsScene::clearYAxis()
{
    view->yAxisButton->setEnabled(false);
    if (defY1)
    {
        removeItem(textY1);
        removeItem(crossY1);
    }
    if (defY0)
    {
        removeItem(lineY);
        removeItem(textY0);
        removeItem(crossY0);
    }
    defYaxis = true;
    defY0 = false;
    defY1 = false;
}

void MyGraphicsScene::clearCross(qreal x, qreal y)
{
    QGraphicsItem *itemDel;
    QTransform transform;
    transform.reset(); // Set to default identity matrix

    itemDel = itemAt(QPointF(x - 5, y - 5), transform);
    if (itemDel != nullptr) {
        removeItem(itemDel);
        update();
    }
}

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    if (defXaxis)
    {
        if (mouseEvent->button() == Qt::LeftButton) 
        {   
            p = mouseEvent->lastScenePos();
            if (!defX0)
            {
                pX0 = p;
                lineX = new QGraphicsLineItem(QLineF(pX0,pX0));
                lineX->setPen(QPen(Qt::blue));
                crossX0 = new CrossX;
                crossX0->setPos(pX0);
                textX0 = new QGraphicsTextItem(tr("x0"));
                textX0->setPos(pX0);
                textX0->setDefaultTextColor(Qt::blue);
                addItem(lineX);
                addItem(crossX0);
                addItem(textX0);
                update();
                defX0 = true;
                return;
            }
            if (defX0 && !defX1)
            {
                pX1 = p;
                lineX->setLine(QLineF(pX0,pX1));
                crossX1 = new CrossX;
                crossX1->setPos(pX1);
                textX1 = new QGraphicsTextItem(tr("x1"));
                textX1->setPos(pX1);
                textX1->setDefaultTextColor(Qt::blue);
                addItem(crossX1);
                addItem(textX1);
                update();
                defX1 = true;
                defXaxis = false;
                view->xAxisButton->setEnabled(true);
                if (!defYaxis) {
                    window->recalculate();
                }
                return;
            }
        }
        if (mouseEvent->button() == Qt::RightButton)
        {
            if (defX0 && !defX1)
            {
                removeItem(lineX);
                removeItem(textX0);
                removeItem(crossX0);
                update();
                defX0 = false;
                return;
            }
        }
    }
    
    if (defYaxis)
    {
        if (mouseEvent->button() == Qt::LeftButton) 
        {   
            p = mouseEvent->lastScenePos();
            if (!defY0)
            {
                pY0 = p;
                lineY = new QGraphicsLineItem(QLineF(pY0,pY0));
                lineY->setPen(QPen(Qt::magenta));
                crossY0 = new CrossY;
                crossY0->setPos(pY0);
                textY0 = new QGraphicsTextItem(tr("y0"));
                textY0->setPos(pY0);
                textY0->setDefaultTextColor(Qt::magenta);
                addItem(lineY);
                addItem(crossY0);
                addItem(textY0);
                update();
                defY0 = true;
                return;
            }
            if (defY0 && !defY1)
            {
                pY1 = p;
                lineY->setLine(QLineF(pY0,pY1));
                crossY1 = new CrossY;
                crossY1->setPos(pY1);
                textY1 = new QGraphicsTextItem(tr("y1"));
                textY1->setPos(pY1);
                textY1->setDefaultTextColor(Qt::magenta);
                addItem(crossY1);
                addItem(textY1);
                update();
                defY1 = true;
                defYaxis = false;
                view->yAxisButton->setEnabled(true);
                if (!defXaxis) {
                    window->recalculate();
                }
                return;
            }
        }
        if (mouseEvent->button() == Qt::RightButton)
        {
            if (defY0 && !defY1)
            {
                removeItem(lineY);
                removeItem(textY0);
                removeItem(crossY0);
                update();
                defY0 = false;
                return;
            }
        }
    }

    if (defCross)
    {
        if (mouseEvent->button() == Qt::LeftButton) 
        {   
            QGraphicsItem *itemCross = new Cross;
            p = mouseEvent->lastScenePos();
            itemCross->setPos(p);
            addItem(itemCross);
            update();
            window->insertPixel(p);
        }
        if (mouseEvent->button() == Qt::RightButton) 
        {
            QGraphicsItem *itemDel;
            p = mouseEvent->lastScenePos();
            p.setX(p.x() - 5);
            p.setY(p.y() - 5);

            QTransform transform;
            transform.reset(); // Set to default identity matrix
            itemDel = itemAt(p,transform);
            if (itemDel == nullptr) {
                return;
            }
            
            QRectF rectDel = itemDel->boundingRect();
            if ((rectDel.height() == 5) && (rectDel.width() == 5)) {
                if (window->removePixel(itemDel->pos())) {
                    removeItem(itemDel);
                    update();
                }
            }
        }
    }
};

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{

    if (defXaxis)
    {
        p = mouseEvent->lastScenePos();
        if (defX0 && !defX1)
        {
            lineX->setLine(QLineF(pX0,p));
            update();
        }
    }
    if (defYaxis)
    {
        p = mouseEvent->lastScenePos();
        if (defY0 && !defY1)
        {
            lineY->setLine(QLineF(pY0,p));
            update();
        }
    }
}