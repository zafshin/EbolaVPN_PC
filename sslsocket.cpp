#include "sslsocket.h"
#include <QSslConfiguration>
#include <QSslCipher>
#include <QDebug>
sslSocket::sslSocket(QTcpSocket *clientSocket,QString Thost, QString Tport, QString sni, QString namePass ,QObject *parent)
    : QObject{parent}, tcpSocket(clientSocket), host(Thost) ,name(namePass), port(Tport), fakeSNI(sni)
{


}


void sslSocket::start(){
    sslsocket = new QSslSocket();
    QSslConfiguration config = sslsocket->sslConfiguration();

    QList<QSslCipher> ciphers;
    ciphers << QSslCipher("TLS_AES_128_GCM_SHA256");
    config.defaultConfiguration();
    config.setSslOption(QSsl::SslOptionDisableCompression, true);
    config.setSslOption(QSsl::SslOptionDisableServerNameIndication, false);
    config.setSslOption(QSsl::SslOptionDisableEmptyFragments,true);
    config.setSslOption(QSsl::SslOptionDisableSessionPersistence,false);
    config.setSslOption(QSsl::SslOptionDisableServerCipherPreference,true);

    config.setCiphers(ciphers);
    config.setPeerVerifyDepth(0);

    config.setPeerVerifyMode(QSslSocket::PeerVerifyMode::VerifyNone);
    config.setProtocol(QSsl::TlsV1_3);

    sslsocket->setSocketOption(QAbstractSocket::SocketOption::LowDelayOption,1);
    tcpSocket->setSocketOption(QAbstractSocket::SocketOption::LowDelayOption,1);
    sslsocket->setSslConfiguration(config);
    sslsocket->setReadBufferSize(16384);
    sslsocket->setPeerVerifyName(fakeSNI);

    connect(sslsocket,&QSslSocket::disconnected,this,&sslSocket::disconnectFromDestination);
    connect(sslsocket,&QSslSocket::readyRead,this,&sslSocket::relayDataFromDestination);
    connect(sslsocket,&QSslSocket::encrypted,this,&sslSocket::connected);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&sslSocket::relayDataFromClient);
    connect(tcpSocket,&QTcpSocket::disconnected,this,&sslSocket::disconnectFromClient);
    sslsocket->connectToHostEncrypted(host,port.toInt());
}

void sslSocket::connected(){
    QByteArray hashValue = QCryptographicHash::hash(name.toLatin1(), QCryptographicHash::Sha1);
    sslsocket->write(hashValue.toBase64()+"\n");
}

void sslSocket::disconnect(){
    tcpSocket->disconnectFromHost();
}

void sslSocket::relayDataFromClient(){
    if(sslsocket->isEncrypted() & ath){
        sslsocket->write(tcpSocket->readAll());
        sslsocket->flush();
    }
}
void sslSocket::relayDataFromDestination(){
    if(sslsocket->isEncrypted()){
    if(!ath){
        if(sslsocket->readLine().indexOf("OK")>-1)
            ath = true;
            relayDataFromClient();
    }else{
        tcpSocket->write(sslsocket->readAll());
        tcpSocket->flush();
    }
    }
}
void sslSocket::disconnectFromClient(){
    sslsocket->disconnectFromHost();
    //qDebug() << "Disconnected from: " << tcpSocket->peerAddress().toString() << "\n";

}
void sslSocket::disconnectFromDestination(){
    tcpSocket->disconnectFromHost();
    //qDebug() << "Disconnected from: " << sslsocket->peerAddress().toString() << "\n";

}
