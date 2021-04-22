#ifndef CROSS_H
#define CROSS_H

#include <QColor>
#include <QGraphicsItem>

using namespace std;

class Cross : public QGraphicsItem
{
public:
    Cross(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    MainWindow *window;
    MyGraphicsScene *graphicsScene;
    View *view;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

private:

};


class CrossX : public QGraphicsItem
{
public:
    CrossX(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    MainWindow *window;
    MyGraphicsScene *graphicsScene;
    View *view;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

private:

};


class CrossY : public QGraphicsItem
{
public:
    CrossY(QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
    MainWindow *window;
    MyGraphicsScene *graphicsScene;
    View *view;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

private:

};


#endif
