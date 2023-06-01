#ifndef EBOLAVPN_H
#define EBOLAVPN_H

#include <QMainWindow>
#include <tlsserver.h>
#include <openvpn.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostInfo>
#include <QFile>
#include <QThread>
#include <QProcess>
#include <QTimer>
#include <QDir>
#include <QRegularExpression>
#include <QSettings>
QT_BEGIN_NAMESPACE
namespace Ui { class EbolaVPN; }
QT_END_NAMESPACE

class EbolaVPN : public QMainWindow
{
    Q_OBJECT

public:
    EbolaVPN(QWidget *parent = nullptr);
    ~EbolaVPN();
signals:
    void setThost(QString Thost);
    void setTport(QString Tport);
    void setlport(QString lport);
    void setfakeSNI(QString fakeSNI);
    void setssl(bool status);
    void setNamePass(QString input);
    void startServer();
    void stopServer();
    void disconnect();
private slots:
    void on_connectBtn_clicked();
    void killAll();
    void on_disconnectBtn_clicked();
    void isConnected();
    void isReconnecting();
    void isAthFail();
    void isSetRoute();
    void isFinished();
    void writeLog(QString input);
    void plinkLog();
    void setUdp();
    void setTcp();
    void setTcpP();
    void setUdpP();

private:
    Ui::EbolaVPN *ui;
    bool waiting_ovpn = false;
    QProcess *x;
    QProcess *pingtunnel;
    QProcess *icmp;
    QProcess *ck;
    QProcess *udpws;
    QStringList ips;
    void log(QString input);
    void decodeURL(QString input);
    QString getDefaultGateway();
    void loadSettings();
    void addRoute();
    void checkDriver();
    QString dgw;
    void removeRoute();
    void saveSettings();
    void setCkHostPort(QString host, QString port);
    void setXHostPort(QString host, QString port, QString sni, QString lport);
    void delFiles();
    void writeOnFile(QString path,QString input);
    QString workingPath;
    QString upPath ="";
    QString configPath ="";
};
#endif // EBOLAVPN_H
