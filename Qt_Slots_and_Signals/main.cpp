#include <QCoreApplication>
#include "Klass.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    Klass t;
    QCoreApplication::connect(&t, SIGNAL(Exit()), &app, SLOT(quit()));
    return app.exec();  
}
