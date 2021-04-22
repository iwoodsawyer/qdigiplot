#include <QtGui>
#include <QMessageBox>
#include <stdlib.h>

#include "qdigiplot.h"
#include "view.h"
#include "scene.h"
#include "cross.h"

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
                view->xAxisButton->setEnabled(true);
                return;
            }
        }
        defXaxis = false;
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
                view->yAxisButton->setEnabled(true);
                return;
            }
        }
        defYaxis = false;
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
            p = mouseEvent->lastScenePos();
            p.setX(p.x() - 5);
            p.setY(p.y() - 5);
            itemDel = itemAt(p);
            
            if (itemDel ==  NULL)
            {
                return;
            }
            
            if (window->removePixel(itemDel->pos()))
            {
                removeItem(itemDel);
                update();
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