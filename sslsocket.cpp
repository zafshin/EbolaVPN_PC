#include "sslsocket.h"
#include <QSslConfiguration>
#include <QSslCipher>
#include <QNetworkProxy>
#include <QDebug>
sslSocket::sslSocket(QTcpSocket *clientSocket,QString Thost, QString Tport, QString sni, QString namePass,bool ssl ,QObject *parent)
    : QObject{parent}, tcpSocket(clientSocket), host(Thost) ,name(namePass), port(Tport), fakeSNI(sni), lssl(ssl)
{


}

QNetworkProxy proxy;
void sslSocket::start(){

    if (lssl){
        sslsocket = new QSslSocket(this);
        QSslConfiguration config = sslsocket->sslConfiguration();

        QList<QSslCipher> ciphers;
        //ciphers << QSslCipher("TLS_AES_128_GCM_SHA256");
        config.defaultConfiguration();
        config.setSslOption(QSsl::SslOptionDisableCompression, true);
        config.setSslOption(QSsl::SslOptionDisableServerNameIndication, false);
        config.setSslOption(QSsl::SslOptionDisableEmptyFragments,true);
        config.setSslOption(QSsl::SslOptionDisableSessionPersistence,false);
        config.setSslOption(QSsl::SslOptionDisableServerCipherPreference,true);

        config.setCiphers(ciphers);
        config.setPeerVerifyDepth(0);

        config.setPeerVerifyMode(QSslSocket::PeerVerifyMode::VerifyNone);
        config.setProtocol(QSsl::SslProtocol::AnyProtocol);

        sslsocket->setSocketOption(QAbstractSocket::SocketOption::LowDelayOption,1);
        sslsocket->setSslConfiguration(config);
        sslsocket->setReadBufferSize(16384);
        sslsocket->setPeerVerifyName(fakeSNI);
        connect(sslsocket,&QSslSocket::disconnected,this,&sslSocket::disconnectFromDestination);
        connect(sslsocket,&QSslSocket::readyRead,this,&sslSocket::relayDataFromDestination);
        connect(sslsocket,&QSslSocket::encrypted,this,&sslSocket::connected);
    }else{

        proxy.setType(QNetworkProxy::Socks5Proxy);
        proxy.setHostName("127.0.0.1");
        proxy.setPort(9999);
        proxy.setUser("");
        proxy.setPassword("");
        nonsslsocket = new QTcpSocket(this);
        nonsslsocket->setProxy(proxy);
        connect(nonsslsocket,&QSslSocket::disconnected,this,&sslSocket::disconnectFromDestination);
        connect(nonsslsocket,&QSslSocket::readyRead,this,&sslSocket::relayDataFromDestination);
        connect(nonsslsocket,&QSslSocket::connected,this,&sslSocket::connected);
    }
    tcpSocket->setSocketOption(QAbstractSocket::SocketOption::LowDelayOption,1);



    connect(tcpSocket,&QTcpSocket::readyRead,this,&sslSocket::relayDataFromClient);
    connect(tcpSocket,&QTcpSocket::disconnected,this,&sslSocket::disconnectFromClient);
    if(!lssl){

        nonsslsocket->connectToHost("127.0.0.1",10000);
    }else
        sslsocket->connectToHostEncrypted(host,port.toInt());
}

void sslSocket::connected(){
    QByteArray hashValue = QCryptographicHash::hash(name.toLatin1(), QCryptographicHash::Sha1);
    if(lssl){
        sslsocket->write(hashValue.toBase64()+"\n");
        sslsocket->waitForBytesWritten(10);
    }else{
        nonsslsocket->write(hashValue.toBase64()+"\n");
        nonsslsocket->waitForBytesWritten(10);
    }
}

void sslSocket::disconnect(){
    tcpSocket->disconnectFromHost();
}

void sslSocket::relayDataFromClient(){
    if(lssl){
        if(sslsocket->isEncrypted() & ath){
            QByteArray data = tcpSocket->readAll();
            sslsocket->write(data);
            sslsocket->flush();
            sslsocket->waitForBytesWritten(10);
        }}else{
        if(ath){
            QByteArray data = tcpSocket->readAll();
            nonsslsocket->write(data);
            nonsslsocket->flush();
            nonsslsocket->waitForBytesWritten(10);
        }
    }
}
void sslSocket::relayDataFromDestination(){
    if(lssl){
        if(sslsocket->isEncrypted()){
            if(!ath){
                if(sslsocket->readLine().indexOf("OK")>-1)
                    ath = true;
                relayDataFromClient();
            }else{
                QByteArray data = sslsocket->readAll();
                tcpSocket->write(data);
                tcpSocket->flush();
                tcpSocket->waitForBytesWritten(10);
            }
        }}else{
        if(!ath){
            if(nonsslsocket->readLine().indexOf("OK")>-1)
                ath = true;
            relayDataFromClient();
        }else{
            QByteArray data = nonsslsocket->readAll();
            tcpSocket->write(data);
            tcpSocket->flush();
            tcpSocket->waitForBytesWritten(10);
        }
    }
}
void sslSocket::disconnectFromClient(){
    if(lssl)
        sslsocket->disconnectFromHost();
    else
        nonsslsocket->disconnectFromHost();
    //qDebug() << "Disconnected from: " << tcpSocket->peerAddress().toString() << "\n";
}
void sslSocket::disconnectFromDestination(){
    tcpSocket->disconnectFromHost();
    //qDebug() << "Disconnected from: " << sslsocket->peerAddress().toString() << "\n";
}
