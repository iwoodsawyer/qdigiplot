#ifndef VIEW_H
#define VIEW_H

#include <QFrame>
#include <QtOpenGL>
#include <QBoxLayout>
#include <QBoxLayout>
#include <QScrollBar>

class MainWindow;
class MyGraphicsScene;
class QGraphicsView;
class QLabel;
class QSlider;
class QToolButton;
class QCheckBox;
class QLineEdit;

class View : public QFrame
{
    Q_OBJECT

public:
    View(QWidget* parent = 0);
    MainWindow* window;
    MyGraphicsScene* graphicsScene;
    QGraphicsView* graphicsView;
    QGraphicsView* view() const;
    QToolButton* xAxisButton;
    QToolButton* yAxisButton;
    QCheckBox* xLogBox;
    QCheckBox* yLogBox;
    QLineEdit* xeLineEdit;
    QLineEdit* xtLineEdit;
    QLineEdit* yeLineEdit;
    QLineEdit* ytLineEdit;

public slots:
    void resetView();
    void zoom25();
    void zoom50();
    void zoom100();
    void zoom200();
    void zoom400();
    void zoomIn();
    void zoomOut();
    void rot90();
    void rotminus90();
    void rotateLeft();
    void rotateRight();
    void print();

private slots:
    void setResetButtonEnabled();
    void setupMatrix();

private:
    QLabel* label;
    QToolButton* resetButton;
    QSlider* zoomSlider;
    QSlider* rotateSlider;
};

#endif
