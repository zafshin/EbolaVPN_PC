#include "ebolavpn.h"
#include <QtGlobal>
#include <QGuiApplication>
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QFile>
#include <QDir>

void copyfiles(){
    QDir destinationDir(QDir::tempPath()+"\\EbolaVPN");
    if (!destinationDir.exists()) {
        if (!destinationDir.mkpath(".")) {
            // An error occurred while creating the destination folder
        }
    }
    if (QFile::copy(":resources/Rar.exe", QDir::tempPath()+"\\EbolaVPN\\rar.exe")) {
        // The file was successfully copied
    } else {
        // An error occurred during the copy process
    }
    if (QFile::copy(":resources/Data.rar", QDir::tempPath()+"\\EbolaVPN\\Data.rar")) {
        // The file was successfully copied
    } else {
        // An error occurred during the copy process
    }
    QProcess rar;
    QStringList args;
    args << "-y" << "x" << "Data.rar";
    rar.setWorkingDirectory(QDir::tempPath()+"\\EbolaVPN\\");
    rar.start(QDir::tempPath()+"\\EbolaVPN\\rar.exe",args);
    rar.waitForFinished();
}
void removeFiles(){
    QDir dir(QDir::tempPath()+"\\EbolaVPN");
    if (dir.exists()) {
        if (dir.removeRecursively()) {
            // Folder and its contents deleted successfully
        } else {
            // An error occurred while deleting the folder and its contents
        }
    } else {
        // The folder does not exist
    }
}

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
    copyfiles();
    EbolaVPN w;
    QFile file(":/resources/darkstyle.qss");
    if(file.open(QFile::ReadOnly)){
        QString styleSheet = QLatin1String(file.readAll());
        w.setStyleSheet(styleSheet);
    }
    w.show();
    int r = a.exec();
    removeFiles();
    return r;
}
