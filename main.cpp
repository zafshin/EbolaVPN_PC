#include "ebolavpn.h"
#include <QtGlobal>
#include <QGuiApplication>
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QFile>

int main(int argc, char *argv[])
{
    qputenv("QT_SCALE_FACTOR", QByteArray("1.2"));
    QSharedMemory sharedMemory;
    sharedMemory.setKey("EbolaVPN");

    if (!sharedMemory.create(1))
    {
        //QMessageBox::warning(nullptr, "Warning!", "An instance of this application is running!" );

        exit(0); // Exit already a process running
    }
    QApplication a(argc, argv);

    EbolaVPN w;
    QFile file("darkstyle.qss");
    if(file.open(QFile::ReadOnly)){
        QString styleSheet = QLatin1String(file.readAll());
        w.setStyleSheet(styleSheet);
    }
    w.show();
    return a.exec();
}
