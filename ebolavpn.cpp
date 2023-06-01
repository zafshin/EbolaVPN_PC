#include "ebolavpn.h"
#include "ui_ebolavpn.h"

tlsServer* server;
QThread *serverTh;
openVPN* ovpn;
EbolaVPN::EbolaVPN(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EbolaVPN)
{
    //qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();

    ui->setupUi(this);
    this->setWindowTitle(this->windowTitle() + " Version 5");
    workingPath = QDir::tempPath() + "\\EbolaVPN\\Data\\";
    if(!QDir(workingPath).exists()){
        QDir().mkdir(workingPath);
    }
    x = new QProcess(this);
    upPath=workingPath + "up";
    configPath = workingPath + "config.ovpn";
    serverTh = new QThread;
    icmp = new QProcess;
    pingtunnel = new QProcess;
    udpws = new QProcess;
    server = new tlsServer();
    ovpn = new openVPN(workingPath.replace("/","\\") + "config",workingPath + "openvpn.exe");
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
    connect(this,&EbolaVPN::setssl,server,&tlsServer::setssl);
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

bool isValidIPv4Address(const QString& ipAddress)
{
    QRegularExpression regex("^((\\d{1,2}|1\\d{2}|2[0-4]\\d|25[0-5])\\.){3}(\\d{1,2}|1\\d{2}|2[0-4]\\d|25[0-5])$");
    QRegularExpressionMatch match = regex.match(ipAddress);

    return match.hasMatch();
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
                if(isValidIPv4Address(gateway))
                    break;
        }
    }
    //log(output);
    log("Default Gateway: " + gateway);
    return gateway;
}
void EbolaVPN::plinkLog(){

}
void EbolaVPN::setUdp(){
    QString fileName = workingPath + "config";
    QFile file(fileName);
    QString con;
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream in(&file);
        con=in.readAll();
        con = con.replace("proto tcp","proto udp");
        con = con.replace("remote 127.0.0.1 7777","remote 127.0.0.1 8888");
        con = con.replace("socks-proxy 127.0.0.1 8888","#");
        file.close();
    } else {
        // Handle file opening error
    }
    if (file.open(QIODevice::ReadWrite | QFile::Truncate | QIODevice::Text)) {
        file.write(con.toLatin1());
        file.close();
    } else {
        // Handle file opening error
    }
}
void EbolaVPN::setTcp(){
    QString fileName = workingPath + "config";
    QFile file(fileName);
    QString con;
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream in(&file);
        con=in.readAll();
        con = con.replace("proto udp","proto tcp");
        con = con.replace("remote 127.0.0.1 7777","remote 127.0.0.1 8888");
        con = con.replace("socks-proxy 127.0.0.1 8888","#");
        file.close();
    } else {
        // Handle file opening error
    }

    if (file.open(QIODevice::ReadWrite | QFile::Truncate | QIODevice::Text)) {
        file.write(con.toLatin1());
        file.close();
    } else {
        // Handle file opening error
    }

}
void EbolaVPN::setTcpP(){
    QString fileName = workingPath + "config";
    QFile file(fileName);
    QString con;
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream in(&file);
        con=in.readAll();
        con = con.replace("proto udp","proto tcp");
        con = con.replace("remote 127.0.0.1 7777","remote 127.0.0.1 8888");
        con = con.replace("#","socks-proxy 127.0.0.1 8888");
        file.close();
    } else {
        // Handle file opening error
    }

    if (file.open(QIODevice::ReadWrite | QFile::Truncate | QIODevice::Text)) {
        file.write(con.toLatin1());
        file.close();
    } else {
        // Handle file opening error
    }

}
void EbolaVPN::setUdpP(){
    QString fileName = workingPath + "config";
    QFile file(fileName);
    QString con;
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream in(&file);
        con=in.readAll();
        con = con.replace("remote 127.0.0.1 8888","remote 127.0.0.1 7777");
        con = con.replace("proto tcp","proto udp");
        con = con.replace("#","socks-proxy 127.0.0.1 8888");
        file.close();
    } else {
        // Handle file opening error
    }

    if (file.open(QIODevice::ReadWrite | QFile::Truncate | QIODevice::Text)) {
        file.write(con.toLatin1());
        file.close();
    } else {
        // Handle file opening error
    }

}
enum pmode{
    tls=0,gu_u=1,g_u=6,icmp_tcp=2,icmp_udp=3,ws=4,wss=5,icmp=7, gu=8, o=9
};
void EbolaVPN::on_connectBtn_clicked()
{
    ui->logBro->clear();
    checkDriver();
    addRoute();
    ui->connectBtn->setEnabled(false);
    ui->disconnectBtn->setEnabled(true);
    ui->addressEdit->setEnabled(false);
    ui->idEdit->setEnabled(false);
    ui->comboBox->setEnabled(false);
    QStringList add = ui->addressEdit->text().split(":");
    setTcp();
    pmode mode;
    if(ui->comboBox->currentText() == "SSL/TLS")
        mode = pmode::tls;

    if(ui->idEdit->text()=="admin#23066"){
        if(ui->comboBox->currentText() == "ICMP_TCP")
            mode = pmode::icmp_tcp;
        if(ui->comboBox->currentText() == "ICMP_UDP")
            mode = pmode::icmp_udp;
        if(ui->comboBox->currentText() == "WS")
            mode = pmode::ws;
        if(ui->comboBox->currentText() == "Gu_test")
            mode = pmode::gu_u;
        if(ui->comboBox->currentText() == "G_test")
            mode = pmode::g_u;



    }else
        mode = pmode::icmp;
    //    if(ui->comboBox->currentText() == "X_tcp")
    //        mode = pmode::x_tcp;
    if(ui->comboBox->currentText() == "G")
        mode = pmode::icmp;
    if(ui->comboBox->currentText() == "Gu")
        mode = pmode::gu;
    if(ui->comboBox->currentText() == "O")
        mode = pmode::o;

    if(add.length() >0){
        QStringList args;
        log("Connecting.");
        switch(mode){
        case pmode::tls:
            emit setNamePass(ui->idEdit->text());
            emit setTport(add[1]);
            emit setThost(add[0]);
            emit setlport("8888");
            emit setssl(true);
            emit startServer();
            ovpn->start();
            break;
        case pmode::icmp_tcp:
            log("Experimental.");
            log("Set to: TCP.");
            setTcp();

            args << "-type" << "client"  << "-tcp" << "1" << "-noprint" <<"1" << "-nolog" <<"1" << "-key" << "46546846" << "-l" << ":8888" <<"-s" << add[0] << "-t" << add[0]+":8888";
            pingtunnel->start(workingPath+"pingtunnel.exe",args);
            ovpn->start();
            break;
        case pmode::icmp_udp:
            log("Experimental.");
            log("Set to: UDP.");
            setUdp();
            args << "-type" << "client" << "-noprint" <<"1" << "-nolog" <<"1" << "-key" << "46546846" << "-l" << ":8888" <<"-s" << add[0] << "-t" << add[0]+":7777";
            pingtunnel->start(workingPath+"pingtunnel.exe",args);
            ovpn->start();
            break;
            //        case pmode::obf:
            //            log("Experimental.");
            //            log("Set to: TCP Proxy.");
            //            setTcpP();
            //            args << "client" << "127.0.0.1:8888" << add[0] + ":" + add[1] << ui->sniEdit->text() << "asdfa2523rfa%" <<"--tls-ja3" << "769,2570-4865-4866-4867-49195-49199-49196-49200-52393-52392-49171-49172-156-157-47-53,2570-0-23-65281-10-11-35-16-5-13-18-51-45-43-27-17513-2570-21,2570-29-23-24,0" << "--tls-alpn" << "h2,http/1.1" << "--tls-sigalgos" << "1027,2052,1025,1283,2053,1281,2054,1537" << "--tls-versions" << "2570,772,771";
            //            tlsPool->start("noisy-shuttle.exe",args);
            //            ovpn->start();
            //            break;
        case pmode::icmp:
            log("Set to: G.");
            setTcp();
            args << "-L" << "socks5://:9999" << "-F" << "icmp://" + add[0] + ":0";
            icmp->start(workingPath+"gost.exe",args);
            emit setNamePass(ui->idEdit->text());
            emit setTport("10000");
            emit setThost("127.0.0.1");
            emit setlport("8888");
            emit setssl(false);
            emit startServer();
            ovpn->start();
            break;
        case pmode::gu:
            log("Set to: Gu.");
            setTcp();
            args << "-L" << "socks5://:9999?log?level=error" << "-F" << "kcp://"+add[0]+":"+add[1]+"?crypt=aes&mode=fast&key=qtcpebola12&log?level=error";
            icmp->start(workingPath+"gost.exe",args);
            emit setNamePass(ui->idEdit->text());
            emit setTport("10000");
            emit setThost("127.0.0.1");
            emit setlport("8888");
            emit setssl(false);
            emit startServer();
            ovpn->start();
            break;
        case pmode::o:
            log("Set to: O.");
            setTcp();
            args << "-L" << "socks5://:9999?log?level=error" << "-F" << "ss+ohttp://AEAD_CHACHA20_POLY1305:nvPIHGFWFASDe@"+add[0]+":"+add[1]+"?log?level=error";
            icmp->start(workingPath+"gost.exe",args);
            emit setNamePass(ui->idEdit->text());
            emit setTport("10000");
            emit setThost("127.0.0.1");
            emit setlport("8888");
            emit setssl(false);
            emit startServer();
            ovpn->start();
            break;
        case pmode::g_u:
            log("Experimental.");
            log("Set to: G test.");
            setUdpP();
            args << "-L" << "socks5://:8888?udp=true" << "-F" << "icmp://" + add[0] + ":0";
            icmp->start(workingPath+"gost.exe",args);
            ovpn->start();
            break;
        case pmode::gu_u:
            log("Experimental.");
            log("Set to: Gu test.");
            setUdpP();
            args << "-L" << "socks5://:8888?udp=true" << "-F" << "kcp://"+add[0]+":"+add[1]+"?crypt=aes&mode=fast&key=qtcpebola12&udp=true&log?level=error";
            icmp->start(workingPath+"gost.exe",args);
            ovpn->start();
            break;
        }

    }
}
void EbolaVPN::setXHostPort(QString host, QString port, QString sni, QString lport){
    QString conf="";
    QString orifileName = workingPath + "xconfig";
    QString outfileName = workingPath + "config.json";
    QFile file(orifileName);
    QFile fileo(outfileName);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream in(&file);
        conf=in.readAll();
        conf = conf.replace("$host$", host);
        conf = conf.replace("$port$", port);
        conf = conf.replace("$sni$", sni);
        conf = conf.replace("$lport$", lport);

        file.close();
        if (fileo.open(QIODevice::ReadWrite | QFile::Truncate | QIODevice::Text)) {
            fileo.write(conf.toLatin1());
            fileo.close();
        }else{
            log("Error writing on file.");
        }
    }else{
        log("Error openning file.");
    }
}
void EbolaVPN::setCkHostPort(QString host, QString port){
    QString conf="";
    QString orifileName = workingPath + "ck";
    QString outfileName = workingPath + "ck.json";
    QFile file(orifileName);
    QFile fileo(outfileName);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream in(&file);
        conf=in.readAll();
        conf = conf.replace("$host$", host);
        conf = conf.replace("$port$", port);
        file.close();
        if (fileo.open(QIODevice::ReadWrite | QFile::Truncate | QIODevice::Text)) {
            fileo.write(conf.toLatin1());
            fileo.close();
        }else{
            log("Error writing on file.");
        }
    }else{
        log("Error openning file.");
    }
}
void EbolaVPN::checkDriver(){
    QProcess tapctl;
    QStringList args;
    args << "list";
    tapctl.start(workingPath+"tapctl",args);
    tapctl.setWorkingDirectory(workingPath);
    tapctl.waitForFinished();
    QString output = tapctl.readAllStandardOutput() + tapctl.readAllStandardError();
    log("Checking adapter.");
    if(output.indexOf("EbolaVPN") == -1){
        log("Adapter not find, Installing driver.");
        QProcess driver;
        QStringList dargs;
        dargs << "/F" << "/S";
        driver.setWorkingDirectory(workingPath+"Driver");
        driver.start(workingPath+"Driver\\dpinst.exe",dargs);
        driver.waitForFinished();
        log("Driver is installed, Creating adapter.");
        QStringList args2;
        QProcess tapctl2;
        args2 << "create" << "--name" << "EbolaVPN" << "--hwid" << "wintun";
        tapctl2.setWorkingDirectory(workingPath);
        tapctl2.start(workingPath+"tapctl",args2);
        tapctl2.waitForFinished();
        log("Adapter is ready.");
    }
}

void EbolaVPN::addRoute(){
    if(ui->addressEdit->text().split(":")[0] != "127.0.0.1"){
        ips.clear();
        QProcess route;
        QString gateway = "";
        dgw = getDefaultGateway();
        QHostAddress address;
        if (address.setAddress(ui->addressEdit->text().split(":")[0])) {
            ips.push_back(address.toString());
            QStringList args={"add",address.toString(),"mask","255.255.255.255",dgw};
            route.start("route",args);
            route.waitForFinished();
            QString output = route.readAllStandardOutput();
            output += route.readAllStandardError();
            if(output.indexOf("OK")>-1)
                log("Route is set.");
        } else {
            QHostInfo hostInfo = QHostInfo::fromName(ui->addressEdit->text().split(":")[0]);
            if (hostInfo.error() != QHostInfo::NoError) {
                log(hostInfo.errorString());
            }
            for (const QHostAddress &address : hostInfo.addresses()) {
                if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                    ips.push_back(address.toString());
                    QStringList args={"add",address.toString(),"mask","255.255.255.255",dgw};
                    route.start("route",args);
                    route.waitForFinished();
                    QString output = route.readAllStandardOutput();
                    output += route.readAllStandardError();
                    if(output.indexOf("OK")>-1)
                        log("Route is added for: " + address.toString());
                }
            }

        }
    }

}
void EbolaVPN::removeRoute(){
    QProcess route;
    for (const QString element : ips) {
        QStringList args={"delete",element,"mask","255.255.255.255",dgw};
        route.start("route",args);
        route.waitForFinished();
        QString output = route.readAllStandardOutput();
        output += route.readAllStandardError();
        if(output.indexOf("OK")>-1)
            log("Route is deleted for: " + element);
    }
}


void EbolaVPN::on_disconnectBtn_clicked()
{
    ui->disconnectBtn->setEnabled(false);
    ui->addressEdit->setEnabled(true);
    ui->idEdit->setEnabled(true);
    ui->comboBox->setEnabled(true);
    x->kill();
    icmp->kill();
    pingtunnel->kill();
    emit disconnect();
    emit stopServer();
    delFiles();
    removeRoute();
    if(waiting_ovpn){
        ui->connectBtn->setEnabled(true);
        waiting_ovpn = false;
    }

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
    ui->comboBox->setCurrentText(settings.value("proto").toString());
}
void EbolaVPN::saveSettings(){
    QSettings settings("HKEY_CURRENT_USER\\Software\\EbolaVPN", QSettings::NativeFormat);
    settings.setValue("address", ui->addressEdit->text());
    settings.setValue("id", ui->idEdit->text());
    settings.setValue("proto", ui->comboBox->currentText());

}
void EbolaVPN::delFiles(){
    if(QFile::exists(configPath))
        QFile::remove(configPath);
    if(QFile::exists(upPath))
        QFile::remove(upPath);
}

void EbolaVPN::killAll(){
    {
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
    //    {
    //        QProcess process;
    //        process.start("tasklist.exe");
    //        process.waitForFinished();
    //        QString output = process.readAllStandardOutput();
    //        QRegularExpression regExp("wstunnel.exe\\s+(\\d+)");
    //        QRegularExpressionMatch match = regExp.match(output);
    //        if (match.hasMatch()) {
    //            QString processId = match.captured(1);
    //            QStringList arguments;
    //            arguments << "/PID" << processId << "/F";
    //            process.start("taskkill.exe",arguments);
    //            process.waitForFinished();
    //        }
    //    }
    //    {
    //        QProcess process;
    //        process.start("tasklist.exe");
    //        process.waitForFinished();
    //        QString output = process.readAllStandardOutput();
    //        QRegularExpression regExp("pingtunnel.exe\\s+(\\d+)");
    //        QRegularExpressionMatch match = regExp.match(output);
    //        if (match.hasMatch()) {
    //            QString processId = match.captured(1);
    //            QStringList arguments;
    //            arguments << "/PID" << processId << "/F";
    //            process.start("taskkill.exe",arguments);
    //            process.waitForFinished();
    //        }
    //    }
    //    {
    //        QProcess process;
    //        process.start("tasklist.exe");
    //        process.waitForFinished();
    //        QString output = process.readAllStandardOutput();
    //        QRegularExpression regExp("xray.exe\\s+(\\d+)");
    //        QRegularExpressionMatch match = regExp.match(output);
    //        if (match.hasMatch()) {
    //            QString processId = match.captured(1);
    //            QStringList arguments;
    //            arguments << "/PID" << processId << "/F";
    //            process.start("taskkill.exe",arguments);
    //            process.waitForFinished();
    //        }
    //    }
    {
        QProcess process;
        process.start("tasklist.exe");
        process.waitForFinished();
        QString output = process.readAllStandardOutput();
        QRegularExpression regExp("gost.exe\\s+(\\d+)");
        QRegularExpressionMatch match = regExp.match(output);
        if (match.hasMatch()) {
            QString processId = match.captured(1);
            QStringList arguments;
            arguments << "/PID" << processId << "/F";
            process.start("taskkill.exe",arguments);
            process.waitForFinished();
        }
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
