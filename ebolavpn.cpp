#include "ebolavpn.h"
#include "ui_ebolavpn.h"
#include <tlsserver.h>
#include <openvpn.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QThread>
#include <QDir>
#include <QRegularExpression>
#include <QSettings>
tlsServer* server;
QThread *serverTh;
openVPN* ovpn;
EbolaVPN::EbolaVPN(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EbolaVPN)
{
    //qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();

    ui->setupUi(this);
    this->setWindowTitle(this->windowTitle() + " Version 1.1");
    workingPath = QDir::tempPath().replace("/","\\") + "\\EbolaVPN\\";
    if(!QDir(workingPath).exists()){
        QDir().mkdir(workingPath);
    }
    upPath=workingPath + "up";
    configPath = workingPath + "config.ovpn";
    serverTh = new QThread;
    server = new tlsServer();
    ovpn = new openVPN(QDir::currentPath().replace("/","\\") + "\\config","openvpn.exe");
    connect(ovpn,&openVPN::readOutput,this,&EbolaVPN::writeLog);
    connect(ovpn,&openVPN::isConnected,this,&EbolaVPN::isConnected);
    connect(ovpn,&openVPN::isClosed,this,&EbolaVPN::isFinished);
    connect(ovpn,&openVPN::isSettingRoute,this,&EbolaVPN::isSetRoute);
    connect(ovpn,&openVPN::isReconnecting,this,&EbolaVPN::isReconnecting);
    connect(ovpn,&openVPN::isAthFail,this,&EbolaVPN::isAthFail);

    server->moveToThread(serverTh);
    connect(this,&EbolaVPN::disconnect,ovpn,&openVPN::stop);
    connect(this,&EbolaVPN::stopServer,server,&tlsServer::disconnectAll);
    connect(this,&EbolaVPN::startServer,server,&tlsServer::startServer);
    connect(this,&EbolaVPN::setThost,server,&tlsServer::setThost);
    connect(this,&EbolaVPN::setTport,server,&tlsServer::setTport);
    connect(this,&EbolaVPN::setfakeSNI,server,&tlsServer::setfakeSNI);
    connect(this,&EbolaVPN::setlport,server,&tlsServer::setlport);
    connect(this,&EbolaVPN::setNamePass,server,&tlsServer::setName);
    connect(qApp,&QApplication::aboutToQuit,this,&EbolaVPN::on_disconnectBtn_clicked);

    connect(serverTh,&QThread::started,server,&tlsServer::start);
    connect(serverTh,&QThread::finished,server,&tlsServer::stop);
    serverTh->start();
    killAll();
    loadSettings();

}

EbolaVPN::~EbolaVPN()
{
    delete ui;
}
QString EbolaVPN::getDefaultGateway(){
    QString gateway = "";
    QProcess route;
    route.start("ipconfig");
    route.waitForFinished();
    QString output = route.readAllStandardOutput();
    QTextStream stream(&output);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        if(line.indexOf("Default Gateway") > -1){
            QStringList sep = line.split(":");
            gateway = sep.last().replace(" ","");
            if(gateway.length() >0)
                break;
        }
    }
    //log(output);
    log("Default Gateway: " + gateway);
    return gateway;
}


void EbolaVPN::on_connectBtn_clicked()
{
    ui->logBro->clear();
    checkDriver();
    addRoute();
    ui->connectBtn->setEnabled(false);
    ui->disconnectBtn->setEnabled(true);
    ui->addressEdit->setEnabled(false);
    ui->idEdit->setEnabled(false);
    ui->sniEdit->setEnabled(false);
    QStringList add = ui->addressEdit->text().split(":");
    if(add.length() >0){
        emit setNamePass(ui->idEdit->text());
        emit setTport(add[1]);
        emit setThost(add[0]);
        emit setlport("9099");
        emit setfakeSNI(ui->sniEdit->text());
        log("Connecting.");
        emit startServer();
        ovpn->start();
    }
}
void EbolaVPN::checkDriver(){
    QProcess tapctl;
    QStringList args;
    args << "list";
    tapctl.start("tapctl",args);
    tapctl.waitForFinished();
    QString output = tapctl.readAllStandardOutput() + tapctl.readAllStandardError();
    log("Checking adapter.");
    if(output.indexOf("EbolaVPN") == -1){
        log("Adapter not find, Installing driver.");

        QProcess driver;
        QStringList dargs;
        dargs << "/F" << "/S";
        driver.setWorkingDirectory(QDir::currentPath().replace("/","\\")+"\\Driver");
        driver.start(QDir::currentPath().replace("/","\\")+"\\Driver\\dpinst.exe",dargs);
        driver.waitForFinished();
        log("Driver is installed, Creating adapter.");
        QStringList args2;
        QProcess tapctl2;
        args2 << "create" << "--name" << "EbolaVPN" << "--hwid" << "wintun";
        tapctl2.start("tapctl",args2);
        tapctl2.waitForFinished();
        log("Adapter is ready.");
    }
}

void EbolaVPN::addRoute(){
    if(ui->addressEdit->text().split(":")[0] != "127.0.0.1"){
    QString gateway = "";
    QProcess route;
    dgw = getDefaultGateway();
    QStringList args={"add",ui->addressEdit->text().split(":")[0],"mask","255.255.255.255",dgw};
    route.start("route",args);
    route.waitForFinished();
    QString output = route.readAllStandardOutput();
    output += route.readAllStandardError();
    if(output.indexOf("OK")>-1)
        log("Route is set.");
    }

}
void EbolaVPN::removeRoute(){
    if(ui->addressEdit->text().split(":")[0] != "127.0.0.1"){
    QString gateway = "";
    QProcess route;
    QStringList args={"delete",ui->addressEdit->text().split(":")[0],"mask","255.255.255.255",dgw};
    route.start("route",args);
    route.waitForFinished();
    QString output = route.readAllStandardOutput();
    output += route.readAllStandardError();
    if(output.indexOf("OK")>-1)
        log("Route is unset.");
    }

}


void EbolaVPN::on_disconnectBtn_clicked()
{
    ui->disconnectBtn->setEnabled(false);
    ui->addressEdit->setEnabled(true);
    ui->idEdit->setEnabled(true);
    ui->sniEdit->setEnabled(true);
    emit disconnect();
    emit stopServer();
    delFiles();
    removeRoute();
}

void EbolaVPN::isConnected(){
    log("OpenVPN: Connected.");
    saveSettings();
}
void EbolaVPN::isSetRoute(){
    log("OpenVPN: Setting up route.");
}
void EbolaVPN::isReconnecting(){
    log("OpenVPN: Reconnecting.");
}
void EbolaVPN::isAthFail(){
    log("OpenVPN: Authentication failed.");
}
void EbolaVPN::isFinished(){
    log("OpenVPN: Closed.");
    emit disconnect();
    ui->connectBtn->setEnabled(true);
}
void EbolaVPN::writeLog(QString input){
    //log(input.replace("\n","").replace("\r",""));
}


void EbolaVPN::log(QString input){
    ui->logBro->append(input);
}

void EbolaVPN::writeOnFile(QString path,QString input){
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(input.toLatin1());
        file.close();
    } else {
        // handle error
    }

}
void EbolaVPN::loadSettings(){
    QSettings settings("HKEY_CURRENT_USER\\Software\\EbolaVPN", QSettings::NativeFormat);
    ui->addressEdit->setText(settings.value("address").toString());
    ui->idEdit->setText(settings.value("id").toString());
    ui->sniEdit->setText(settings.value("sni").toString());
}
void EbolaVPN::saveSettings(){
    QSettings settings("HKEY_CURRENT_USER\\Software\\EbolaVPN", QSettings::NativeFormat);
    settings.setValue("address", ui->addressEdit->text());
    settings.setValue("id", ui->idEdit->text());
    settings.setValue("sni", ui->sniEdit->text());
}
void EbolaVPN::delFiles(){
    if(QFile::exists(configPath))
        QFile::remove(configPath);
    if(QFile::exists(upPath))
        QFile::remove(upPath);
}

void EbolaVPN::killAll(){
    QProcess process;
    process.start("tasklist.exe");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    QRegularExpression regExp("openvpn.exe\\s+(\\d+)");
    QRegularExpressionMatch match = regExp.match(output);
    if (match.hasMatch()) {
        QString processId = match.captured(1);
        QStringList arguments;
        arguments << "/PID" << processId << "/F";
        process.start("taskkill.exe",arguments);
        process.waitForFinished();
    }
}
void EbolaVPN::decodeURL(QString input){
    input = input.replace("ebola://","");
    QString decoded = QByteArray::fromBase64(input.toLatin1());
    QJsonDocument jsonDoc = QJsonDocument::fromJson(decoded.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    //    log(jsonObj["server"].toString());
    //    log(jsonObj["port"].toString());
    //    log(jsonObj["user"].toString());
    //    log(jsonObj["pass"].toString());
    //    log(jsonObj["porotocol"].toString());
    //    log(jsonObj["sni"].toString());
    //log(jsonObj["cert"].toString());
    emit setThost(jsonObj["server"].toString());
    emit setTport(jsonObj["port"].toString());
    emit setlport("9099");
    emit setfakeSNI(jsonObj["sni"].toString());
    writeOnFile(upPath, jsonObj["user"].toString() + "\r\n" + jsonObj["pass"].toString());
    QString config = "client\r\ndev tun\r\nproto " + jsonObj["porotocol"].toString() \
            + "\r\n" + "auth-user-pass " + upPath.replace("\\","\\\\") + "\r\nremote " + "127.0.0.1" + " " + "9099" + "\r\ncomp-lzo no"\
            + "\r\nresolv-retry infinite\r\nnobind\r\n"\
            +"persist-key\r\npersist-tun\r\nremote-cert-tls server\r\nauth SHA512\r\ncipher AES-256-CBC\r\n" \
            +"ignore-unknown-option block-outside-dns\r\nverb 3\r\n" + jsonObj["cert"].toString() + "\r\n";
    writeOnFile(configPath,config);
}
