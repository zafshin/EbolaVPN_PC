#ifndef SSLSOCKET_H
#define SSLSOCKET_H

#include <QObject>
#include <QThread>
#include <QSslSocket>
#include <QTcpServer>
#include <QTcpSocket>

class sslSocket : public QObject
{
    Q_OBJECT
public:
    explicit sslSocket(QTcpSocket *clientSocket,QString Thost, QString Tport, QString sni, QString namePass, bool ssl,QObject *parent = nullptr);

signals:
public slots:
    void start();
    void disconnect();
private slots:
    void relayDataFromClient();
    void disconnectFromClient();
    void disconnectFromDestination();
    void relayDataFromDestination();
    void connected();
private:
    QSslSocket *sslsocket;
    QTcpSocket *tcpSocket;
    QTcpSocket *nonsslsocket;
    bool lssl;
    QString host;
    QString name;
    QString port;
    QString fakeSNI;
    bool ath = false;
};

#endif // SSLSOCKET_H
