#include "qdigiplot.h"

#include <QPixmap>
#include <QIcon>
#include <QStyleFactory>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("windowsvista"));
    app.setWindowIcon(QIcon(":/images/chart_curve2.png"));
    QCoreApplication::setApplicationName("DigiPlot");

    MainWindow* mainWin = new MainWindow(argc, argv);
    mainWin->show();
    return app.exec();
}
