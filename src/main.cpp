#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("PhotoEditor");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("PhotoEditor");
    
    app.setStyle(QStyleFactory::create("Fusion"));
    
    MainWindow mainWindow;
    mainWindow.show();
    
    return app.exec();
}
