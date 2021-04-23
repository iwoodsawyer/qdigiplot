#include "qdigiplot.h"
#include "view.h"
#include "scene.h"
#include "cross.h"

#include <math.h>

using namespace std;

MainWindow::MainWindow(int argc, char *argv[])
{
    graphicsImage = new QGraphicsPixmapItem;
    graphicsScene = new MyGraphicsScene;
    graphicsScene->setSceneRect(QRect(0, 0, 200, 200));
    graphicsScene->addItem(graphicsImage);

    view = new View;
    view->view()->setScene(graphicsScene);

    setCentralWidget(view);

    createActions();
    createMenus();
    createDockWindows();
    createToolBars();
    createStatusBar();

    updateRecentImageActions();

    graphicsScene->window=this;
    graphicsScene->view=view;
    view->window=this;
    view->graphicsScene=graphicsScene;

    setCurrentFile("");
    readSettings();
}

void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open File"), QDir::currentPath(),
                                    tr("All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm);; Bitmap Files (*.bmp);; GIF Files (*.gif);; JPEG Files (*.jpg *.jpeg);; PNG Files (*.png);; All Files (*.*)"));
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Image Viewer"),
                                     tr("Cannot load %1.").arg(fileName));
            return;
        }
        clearScreen();
        setCurrentImage(fileName);
        setCurrentFile(fileName);
        graphicsScene->setSceneRect(QRect(0, 0, image.width(), image.height()));
        graphicsImage->setPixmap(QPixmap::fromImage(image));
    }
    statusBar()->showMessage(tr("Image loaded"), 2000);
}

void MainWindow::openRecentImage()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QString fileName=QString(action->data().toString());
    if (action)
    {
        QImage image(fileName);
        clearScreen();
        setCurrentImage(fileName);
        setCurrentFile(fileName);
        graphicsScene->setSceneRect(QRect(0, 0, image.width(), image.height()));
        graphicsImage->setPixmap(QPixmap::fromImage(image));
    }
    statusBar()->showMessage(tr("Image loaded"), 2000);
}

void MainWindow::setCurrentImage(const QString &fileName)
{
    QSettings settings("QSoftware", "DigiPlot");
    settings.beginGroup("RecentFiles");
    QStringList files = settings.value("recentFileList").toStringList();

    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin) {
            mainWin->updateRecentImageActions();
        }
    }
    settings.endGroup();
}

void MainWindow::updateRecentImageActions()
{
    QSettings settings("QSoftware", "DigiPlot");
    settings.beginGroup("RecentFiles");
    QStringList files = settings.value("recentFileList").toStringList();
    int numRecentFiles = qMin(files.size(),(int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
    settings.endGroup();
}



bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save As"),
                        tr("%1%2").arg(nosuffixName(curFile)).arg(tr(".cvs")),
                        tr("Comma Separated Value Files (*.cvs);; All Files (*.*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MainWindow::saveFile(const QString &fileName)
{
    int size = valuesList->rowCount();

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("DigiPlot"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    for (int i=0; i<size; i++) {
        out << valuesList->item(i,2)->text() << tr(", ") << valuesList->item(i,3)->text() << tr("\n");
    }
    
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    static int sequenceNumber = 1;

    isUntitled = fileName.isEmpty();
    if (isUntitled) {
        curFile = tr("image%1").arg(sequenceNumber++);
        setWindowTitle(tr("DigiPlot"));
    } else {
        curFile = QFileInfo(fileName).canonicalFilePath();
        setWindowTitle(tr("%1[*] - %2").arg(strippedName(curFile)).arg(tr("QDigiPlot")));
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

QString MainWindow::nosuffixName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).baseName();
}

void MainWindow::about()
{
    QMessageBox box(this);
    box.setText("DigiPlot\n"
                "written by Ivo Houtzager\n\n"
                "Credits:\n"
                "Silk icons by FAMFAMFAM \n"
                "http://www.famfamfam.com \n");
    box.setWindowTitle(tr("About DigiPlot"));
    box.exec();
}

void MainWindow::help()
{
    QMessageBox box(this);
    box.setText("DigiPlot extracts data values from graphs and plots.\n\n"
                "Features:\n"
                "- import of most common image file formats; bmp, gif, png, jpg, pbm, pgm, ppm, xbm and xpm\n"  
                "- export data to comma seperated values file\n"
                "- manual digitizing via mouse clicks\n"
                "- can handle distorted graphs, axes do not need to be orthogonal\n"
                "- linear and logarithmic axes\n"
                "- graphs can be zoomed and rotated");
    box.setWindowTitle(tr("Help"));
    box.setIcon(QMessageBox::NoIcon);
    box.exec();
}


void MainWindow::createDockWindows()
{
    dockTable=new QDockWidget(tr("Values"), this);
    dockTable->setAllowedAreas(Qt::RightDockWidgetArea);
    valuesList=new QTableWidget(0,4,dockTable);
    QStringList headers;
    headers << tr("X Pixel") << tr("Y Pixel") << tr("X Plot") << tr("Y Plot");
    valuesList->setHorizontalHeaderLabels(headers);
    dockTable->setWidget(valuesList);
    addDockWidget(Qt::RightDockWidgetArea, dockTable);
    windowMenu->addAction(dockTable->toggleViewAction());
}

void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/images/folder_image.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing image"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openImage()));

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),this, SLOT(openRecentImage()));
    }

    printImageAct = new QAction(QIcon(":/images/printer.png"), tr("&Print..."), this);
    printImageAct->setShortcut(tr("Ctrl+P"));
    printImageAct->setStatusTip(tr("Print the current image"));
    connect(printImageAct, SIGNAL(triggered()), view, SLOT(print()));

    saveAsAct = new QAction(QIcon(":/images/table_save.png"), tr("&Save As..."), this);
    saveAsAct->setShortcut(tr("Ctrl+Shift+S"));
    saveAsAct->setStatusTip(tr("Save table values to text file"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(QIcon(":/images/door_in.png"), tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    recalcPointsAct = new QAction(QIcon(":/images/table_refresh.png"), tr("Recalculate"), this);
    recalcPointsAct->setShortcut(tr("Ctrl+Shift+C"));
    recalcPointsAct->setStatusTip(tr("Recalculate points for new axis"));
    connect(recalcPointsAct, SIGNAL(triggered()), this, SLOT(recalculate()));

    newXAxisAct = new QAction(QIcon(":/images/flag_blue.png"), tr("Redefine X Axis"), this);
    newXAxisAct->setShortcut(tr("Ctrl+Shift+X"));
    newXAxisAct->setStatusTip(tr("Redefine X axis"));
    connect(newXAxisAct, SIGNAL(triggered()), this, SLOT(newXAxis()));

    newYAxisAct = new QAction(QIcon(":/images/flag_pink.png"), tr("Redefine Y Axis"), this);
    newYAxisAct->setShortcut(tr("Ctrl+Shift+Y"));
    newYAxisAct->setStatusTip(tr("Redefine Y axis"));
    connect(newYAxisAct, SIGNAL(triggered()), this, SLOT(newYAxis()));

    clearPointsAct = new QAction(QIcon(":/images/chart_line_delete.png"), tr("Clear Points"), this);
    clearPointsAct->setShortcut(tr("Ctrl+Shift+P"));
    clearPointsAct->setStatusTip(tr("Clear points"));
    connect(clearPointsAct, SIGNAL(triggered()), this, SLOT(clearPoints()));

    clearAllAct = new QAction(QIcon(":/images/chart_curve_delete.png"), tr("Clear All"), this);
    clearAllAct->setShortcut(tr("Ctrl+Shift+A"));
    clearAllAct->setStatusTip(tr("Clear points and axis"));
    connect(clearAllAct, SIGNAL(triggered()), this, SLOT(clearScreen()));

    resetViewAct = new QAction(QIcon(":/images/anchor.png"), tr("Reset View"), this);
    resetViewAct->setShortcut(tr("Ctrl+Shift+R"));
    resetViewAct->setStatusTip(tr("Reset zoom and rotation to normal"));
    connect(resetViewAct, SIGNAL(triggered()), view, SLOT(resetView()));

    zoomInAct = new QAction(QIcon(":/images/zoom_in.png"), tr("Zoom In"), this);
    zoomInAct->setShortcut(tr("Ctrl+Plus"));
    zoomInAct->setStatusTip(tr("Increase Zoom with 1%"));
    connect(zoomInAct, SIGNAL(triggered()), view, SLOT(zoomIn()));
    
    zoomOutAct = new QAction(QIcon(":/images/zoom_out.png"), tr("Zoom Out"), this);
    zoomOutAct->setShortcut(tr("Ctrl+Minus"));
    zoomOutAct->setStatusTip(tr("Decrease Zoom with 1%"));
    connect(zoomOutAct, SIGNAL(triggered()), view, SLOT(zoomOut()));

    zoom25Act = new QAction(tr("Zoom 25%"), this);
    zoom25Act->setStatusTip(tr("Zoom to 25% of the original size"));
    connect(zoom25Act, SIGNAL(triggered()), view, SLOT(zoom25()));

    zoom50Act = new QAction(tr("Zoom 50%"), this);
    zoom50Act->setStatusTip(tr("Zoom to 50% of the original size"));
    connect(zoom50Act, SIGNAL(triggered()), view, SLOT(zoom50()));

    zoom100Act = new QAction(tr("Zoom 100%"), this);
    zoom100Act->setShortcut(tr("Ctrl+Shift+Z"));
    zoom100Act->setStatusTip(tr("Zoom to 100% of the original size"));
    connect(zoom100Act, SIGNAL(triggered()), view, SLOT(zoom100()));

    zoom200Act = new QAction(tr("Zoom 200%"), this);
    zoom200Act->setStatusTip(tr("View image in 200% of the original size"));
    connect(zoom200Act, SIGNAL(triggered()), view, SLOT(zoom200()));

    zoom400Act = new QAction(tr("Zoom 400%"), this);
    zoom400Act->setStatusTip(tr("Zoom to 400% of the original size"));
    connect(zoom400Act, SIGNAL(triggered()), view, SLOT(zoom400()));

    rot90Act = new QAction(tr("Rotate 90deg CW"), this);
    rot90Act->setStatusTip(tr("Rotate image 90 degrees clockwise"));
    connect(rot90Act, SIGNAL(triggered()), view, SLOT(rot90()));

    rotminus90Act = new QAction(tr("Rotate 90deg CCW"), this);
    rotminus90Act->setStatusTip(tr("Rotate image 90 degrees counter clockwise"));
    connect(rotminus90Act, SIGNAL(triggered()), view, SLOT(rotminus90()));

    rotCClockAct = new QAction(QIcon(":/images/arrow_rotate_anticlockwise.png"), tr("Rotate CCW"), this);
    rotCClockAct->setShortcut(tr("Ctrl+G"));
    rotCClockAct->setStatusTip(tr("Rotate image one degree counter clockwise"));
    connect(rotCClockAct, SIGNAL(triggered()), view, SLOT(rotateLeft()));

    rotClockAct = new QAction(QIcon(":/images/arrow_rotate_clockwise.png"), tr("Rotate CW"), this);
    rotClockAct->setShortcut(tr("Ctrl+H"));
    rotClockAct->setStatusTip(tr("Rotate image one degree clockwise"));
    connect(rotClockAct, SIGNAL(triggered()), view, SLOT(rotateRight()));

    helpAct = new QAction(QIcon(":/images/help.png"), tr("&Contents"), this);
    helpAct->setShortcut(tr("F1"));
    helpAct->setStatusTip(tr("Show the help on the interpreter"));
    connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(view->xAxisButton, SIGNAL(clicked()), this, SLOT(newXAxis()));
    connect(view->yAxisButton, SIGNAL(clicked()), this, SLOT(newYAxis()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    separatorAct = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActs[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(printImageAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    dataMenu = menuBar()->addMenu(tr("&Data"));
    dataMenu->addAction(recalcPointsAct);
    dataMenu->addSeparator();
    dataMenu->addAction(newXAxisAct);
    dataMenu->addAction(newYAxisAct);
    dataMenu->addSeparator();
    dataMenu->addAction(clearPointsAct);
    dataMenu->addAction(clearAllAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(resetViewAct);
    viewMenu->addSeparator();
    viewMenu->addAction(zoom25Act);
    viewMenu->addAction(zoom50Act);
    viewMenu->addAction(zoom100Act);
    viewMenu->addAction(zoom200Act);
    viewMenu->addAction(zoom400Act);
    viewMenu->addSeparator();
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addSeparator();
    viewMenu->addAction(rot90Act);
    viewMenu->addAction(rotminus90Act);
    viewMenu->addSeparator();
    viewMenu->addAction(rotClockAct);
    viewMenu->addAction(rotCClockAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("Main Toolbar"));
    fileToolBar->setMovable(false);
    fileToolBar->setIconSize(QSize(16,16));
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAsAct);
    fileToolBar->addAction(printImageAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(recalcPointsAct);
    fileToolBar->addAction(newXAxisAct);
    fileToolBar->addAction(newYAxisAct);
    fileToolBar->addAction(clearPointsAct);
    fileToolBar->addAction(clearAllAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(resetViewAct);
    fileToolBar->addAction(zoomInAct);
    fileToolBar->addAction(zoomOutAct);
    fileToolBar->addAction(rotCClockAct);
    fileToolBar->addAction(rotClockAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(helpAct);
    windowMenu->addSeparator();
    windowMenu->addAction(fileToolBar->toggleViewAction());
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::readSettings()
{
    QSettings settings("QSoftware", "DigiPlot");
    settings.beginGroup("StateMainWindow");
    fileToolBar->setVisible(settings.value("tool",true).toBool());
    resize(settings.value("size",QSize(640,480)).toSize());
    move(settings.value("pos",QPoint(200,200)).toPoint());
    restoreState(settings.value("state").toByteArray());
    restoreGeometry(settings.value("geometry").toByteArray());
    dockTable->setVisible(settings.value("dockVis",true).toBool());
    dockTable->restoreGeometry(settings.value("dockGeometry").toByteArray());
    settings.endGroup();
}
void MainWindow::writeSettings()
{
    QSettings settings("QSoftware", "DigiPlot");
    settings.beginGroup("StateMainWindow");
    settings.setValue("tool", fileToolBar->isVisible());
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("state", saveState());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("dockVis", dockTable->isVisible());
    settings.setValue("dockGeometry", dockTable->saveGeometry());
    settings.endGroup();
}
void MainWindow::insertPixel(const QPointF &point)
{
    QPointF real;
    
    QTableWidgetItem *pixelX = new QTableWidgetItem(tr("%1").arg(point.x()));
    QTableWidgetItem *pixelY = new QTableWidgetItem(tr("%1").arg(point.y()));

    realPoints(point, real);

    QTableWidgetItem *plotX = new QTableWidgetItem(tr("%1").arg(real.x()));
    QTableWidgetItem *plotY = new QTableWidgetItem(tr("%1").arg(real.y()));

    int row = valuesList->rowCount();
    valuesList->insertRow(row);
    valuesList->setItem(row, 0, pixelX);
    valuesList->setItem(row, 1, pixelY);
    valuesList->setItem(row, 2, plotX);
    valuesList->setItem(row, 3, plotY);

    valuesList->scrollToItem(plotY,QAbstractItemView::EnsureVisible);
}

bool MainWindow::removePixel(const QPointF &point)
{
    QList<QTableWidgetItem*> pixelListX = valuesList->findItems(tr("%1").arg(point.x()),Qt::MatchExactly);
    QList<QTableWidgetItem*> pixelListY = valuesList->findItems(tr("%1").arg(point.y()),Qt::MatchExactly);

    int size = valuesList->rowCount();
    int sizex = pixelListX.size();
    int sizey = pixelListY.size();

    for (int i=0; i<sizex; i++) {
        for (int j=0; j<sizey; j++) {
            int rowx = valuesList->row(pixelListX[i]);
            int rowy = valuesList->row(pixelListY[j]);
        
            if (rowx == rowy) {
                valuesList->removeRow(rowx);
                valuesList->setRowCount(size-1);
                return true;
            }
        }
    }
    return false;
}

void MainWindow::clearPoints()
{
    int size = valuesList->rowCount();
    qreal x, y;
    
    for (int i=1; i<=size; i++) {
        x = valuesList->item(0,0)->text().toFloat();
        y = valuesList->item(0,1)->text().toFloat();

        valuesList->removeRow(0);
        valuesList->setRowCount(size-i);

        graphicsScene->clearCross(x,y);
    }
}

void MainWindow::clearScreen()
{
    clearPoints();
    graphicsScene->clearXAxis();
    graphicsScene->clearYAxis();
}

void MainWindow::newXAxis()
{
    graphicsScene->clearXAxis();
}

void MainWindow::newYAxis()
{
    graphicsScene->clearYAxis();
}

void MainWindow::recalculate()
{
    int size = valuesList->rowCount();
    qreal x, y;
    QPointF point, real;
    QTableWidgetItem *calcX;
    QTableWidgetItem *calcY;

    for (int i=0; i<size; i++) {
        x = valuesList->item(i,0)->text().toFloat();
        y = valuesList->item(i,1)->text().toFloat();

        point.setX(x);
        point.setY(y);
     
        realPoints(point, real);

        calcX = new QTableWidgetItem(tr("%1").arg(real.x()));
        calcY = new QTableWidgetItem(tr("%1").arg(real.y()));
        valuesList->setItem(i,2,calcX);
        valuesList->setItem(i,3,calcY);
    }
}

void MainWindow::realPoints(const QPointF &point, QPointF &real)
{
    qreal ox0, ox1, oy0, oy1;
    qreal pX0x, pX0y, pX1x, pX1y, pY0x, pY0y, pY1x, pY1y;
    qreal pXx, pXy, pYx, pYy;
    qreal scaleX, scaleY, realx, realy;
    qreal thx, thy, xCal, yCal, xPoint, yPoint;
    bool ok0x = false;
    bool ok1x = false;
    bool ok0y = false;
    bool ok1y = false;

    ox0 = QString(view->xeLineEdit->text()).toFloat(&ok0x);
    if (!ok0x) {
        QMessageBox::information(this, "X Axis", "The x0 value is not properly set.");
        ox0 = 0;
    }
    ox1 = QString(view->xtLineEdit->text()).toFloat(&ok1x);
    if (!ok1x) {
        QMessageBox::information(this, "X Axis", "The x1 value is not properly set.");
        ox1 = 1;
    }
    oy0 = QString(view->yeLineEdit->text()).toFloat(&ok0y);
    if (!ok0y) {
        QMessageBox::information(this, "Y Axis", "The y0 value is not properly set.");
        oy0 = 0;
    }
    oy1 = QString(view->ytLineEdit->text()).toFloat(&ok1y);
    if (!ok1y) {
        QMessageBox::information(this, "Y Axis", "The y1 value is not properly set.");
        oy1 = 1;
    }

    if (view->xLogBox->isChecked()) {
        if (ox0 == 0) {
            ox0 = ox1*1e-37;
        }
        scaleX = log10(ox1 / ox0);
    }
    else {
        scaleX = ox1 - ox0;
    }

    if (view->yLogBox->isChecked()) {
        if (oy0 == 0) {
            oy0 = oy1*1e-37;
        }
        scaleY = log10(oy1 / oy0);
    }
    else {
        scaleY = oy1 - oy0;
    }

    pX0x = graphicsScene->pX0.x();
    pX0y = graphicsScene->pX0.y();
    pX1x = graphicsScene->pX1.x();
    pX1y = graphicsScene->pX1.y();
    pY0x = graphicsScene->pY0.x();
    pY0y = graphicsScene->pY0.y();
    pY1x = graphicsScene->pY1.x();
    pY1y = graphicsScene->pY1.y();
    pXx = point.x();
    pXy = point.y();
    pYx = point.x();
    pYy = point.y();

    if (abs(pX1x - pX0x) < abs(pX1y - pX0y)) {
        swap(pX0x,pX0y);
        swap(pX1x,pX1y);
        swap(pXx,pXy);
    }
    
    if (abs(pY1y - pY0y) < abs(pY1x - pY0x)) {
        swap(pY0x,pY0y);
        swap(pY1x,pY1y);
        swap(pYx,pYy);
    }

    thx = atan((pX1y - pX0y)/(pX1x - pX0x));
    thy = atan((pY1x - pY0x)/(pY1y - pY0y));
    
    xCal = cos(thx)*(pX1x - pX0x) + sin(thx)*(pX1y - pX0y); 
    yCal = cos(thy)*(pY1y - pY0y) + sin(thy)*(pY1x - pY0x); 
    
    xPoint = cos(thx)*(pXx - pX0x) + sin(thx)*(pXy - pX0y); 
    yPoint = cos(thy)*(pYy - pY0y) + sin(thy)*(pYx - pY0x);

    if (view->xLogBox->isChecked())
        realx = ox0*pow(10,(xPoint*scaleX/xCal));
    else
        realx = ox0 + (xPoint*scaleX/xCal);

    if (view->yLogBox->isChecked())
        realy = oy0*pow(10,(yPoint*scaleY/yCal));
    else
        realy = oy0 + (yPoint*scaleY/yCal);

    real.setX(realx);
    real.setY(realy);
}

void MainWindow::swap(qreal &f1, qreal &f2)
{
   qreal temp;
   temp = f1;
   f1 = f2;
   f2 = temp;
}
