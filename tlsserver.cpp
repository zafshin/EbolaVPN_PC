#include "tlsserver.h"
#include <QDebug>
#include <QThread>
#include <sslsocket.h>

tlsServer::tlsServer(QString Thost ,QString Tport , QString lport, QString fakeSNI, QObject *parent)
    : QObject{parent}, sni(fakeSNI), portLocal(lport), portHost(Tport) , host(Thost)
{


}

void tlsServer::newConnection(){
    QTcpSocket* socket = server->nextPendingConnection();
    QThread* newQThread = new QThread;
    sslSocket* ss = new sslSocket(socket,host,portHost,sni,name);
    //ss->moveToThread(newQThread);
    //socket->setParent(nullptr);
    //socket->moveToThread(newQThread);
    connect(newQThread, &QThread::started, ss, &sslSocket::start);
    connect(this, &tlsServer::disconnect, ss, &sslSocket::disconnect);
    ss->start();
    //qDebug() << "New connection from: " << socket->peerAddress().toString() << "\n";
}
void tlsServer::setName(QString namePass){
    name = namePass;
}

void tlsServer::setThost(QString Thost){
    host = Thost;
}
void tlsServer::setTport(QString Tport){
    portHost = Tport;
}
void tlsServer::setlport(QString lport){
    portLocal = lport;
}
void tlsServer::setfakeSNI(QString fakeSNI){
    sni = fakeSNI;
}

void tlsServer::disconnectAll(){
    server->close();
    emit disconnect();
}

void tlsServer::stop(){
    server->close();
    emit disconnect();
}
void tlsServer::start(){
    server = new QTcpServer(this);
    connect(server,&QTcpServer::newConnection,this,&tlsServer::newConnection);
}
void tlsServer::startServer(){
    //qDebug() << "Server listening: " << "\n";
    server->listen(QHostAddress::Any,portLocal.toInt());
}
