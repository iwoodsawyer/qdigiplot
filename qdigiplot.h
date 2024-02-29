#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QMenuBAr>
#include <QStyle>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPrinter>
#include <QPrintDialog>
#include <QCursor>
#include <QToolButton>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QTableWidget>
#include <QDockWidget>

class QAction;
class QMenu;
class QScrollArea;
class QScrollBar;
class QDockWidget;
class QTableWidget;
class QTableWidgetItem;
class QGraphicsPixmapItem;
class QGraphicsLineItem;
class MyGraphicsScene;
class View;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void insertPixel(const QPointF& point);
    bool removePixel(const QPointF& point);
    void clearPoints();
    void clearScreen();
    void newXAxis();
    void newYAxis();
    void recalculate();

public:
    MainWindow(int argc, char* argv[]);
    MyGraphicsScene* graphicsScene;
    View* view;
    QTableWidget* valuesList;

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void openImage();
    void openRecentImage();
    bool saveAs();
    void help();
    void about();

private:
    void createActions();
    void createMenus();
    void createDockWindows();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool saveFile(const QString& fileName);
    void setCurrentFile(const QString& fileName);
    void setCurrentImage(const QString& fileName);
    void updateRecentImageActions();
    void realPoints(const QPointF& point, QPointF& real);
    void swap(qreal& f1, qreal& f2);

    QString strippedName(const QString& fullFileName);
    QString nosuffixName(const QString& fullFileName);
    QString curFile;
    bool isUntitled;

    QGraphicsPixmapItem* graphicsImage;
    QGraphicsView* graphicsView;
    QScrollArea* scrollArea;
    QPrinter printer;

    QToolBar* fileToolBar;
    QDockWidget* dockTable;
    QMenu* fileMenu;
    QMenu* recentFilesMenu;
    QMenu* dataMenu;
    QMenu* viewMenu;
    QMenu* windowMenu;
    QMenu* helpMenu;
    QAction* openAct;
    QAction* printImageAct;
    QAction* saveAsAct;
    QAction* exitAct;
    QAction* helpAct;
    QAction* recalcPointsAct;
    QAction* newXAxisAct;
    QAction* newYAxisAct;
    QAction* clearPointsAct;
    QAction* clearAllAct;
    QAction* resetViewAct;
    QAction* zoomInAct;
    QAction* zoomOutAct;
    QAction* zoom25Act;
    QAction* zoom50Act;
    QAction* zoom100Act;
    QAction* zoom200Act;
    QAction* zoom400Act;
    QAction* rot90Act;
    QAction* rotminus90Act;
    QAction* rotClockAct;
    QAction* rotCClockAct;
    QAction* aboutAct;
    QAction* aboutQtAct;
    QAction* separatorAct;
    enum { MaxRecentFiles = 5 };
    QAction* recentFileActs[MaxRecentFiles];
};

#endif