#ifndef TLSSERVER_H
#define TLSSERVER_H

#include <QObject>
#include <QSslSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

class tlsServer : public QObject
{
    Q_OBJECT
public:
    explicit tlsServer(QString Thost = "" ,QString Tport = "" , QString lport = "", QString fakeSNI = "", bool ssl = true, QObject *parent = nullptr);

signals:
    void disconnect();
public slots:
    void setThost(QString Thost);
    void setTport(QString Tport);
    void setlport(QString lport);
    void setssl(bool status);
    void setfakeSNI(QString fakeSNI);
    void setName(QString namePass);
    void start();
    void startServer();
    void stop();
    void disconnectAll();
private slots:
    void newConnection();

private:
    void makeSSL();
    QTcpServer *server;
    //QSslSocket socket;
    QString sni;
    bool lssl;
    QString name;
    QString portLocal;
    QString portHost;
    QString host;

};

#endif // TLSSERVER_H
