#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMouseEvent>

using namespace std;

class QGraphicsTextItem;
class QGraphicsLineItem;
class CrossX;
class CrossY;
class MainWindow;
class View;

class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    MyGraphicsScene(QObject* parent = 0);

    void clearXAxis();
    void clearYAxis();
    void clearCross(qreal x, qreal y);

    MainWindow *window;
    View *view;
    QPointF pX0;
    QPointF pX1;
    QPointF pY0;
    QPointF pY1;
    QGraphicsLineItem *lineX;
    QGraphicsLineItem *lineY;
    QGraphicsTextItem *textX0;
    QGraphicsTextItem *textX1;
    QGraphicsTextItem *textY0;
    QGraphicsTextItem *textY1;
    CrossX *crossX0;
    CrossX *crossX1;
    CrossY *crossY0;
    CrossY *crossY1;

    bool defXaxis;
    bool defYaxis;
    bool defCross;
    bool defX0;
    bool defX1;
    bool defY0;
    bool defY1;

public slots:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);

private:
    QPointF p;
};

#endif
