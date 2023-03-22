#ifndef EBOLAVPN_H
#define EBOLAVPN_H

#include <QMainWindow>

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

private:
    Ui::EbolaVPN *ui;
    void log(QString input);
    void decodeURL(QString input);
    QString getDefaultGateway();
    void loadSettings();
    void addRoute();
    void checkDriver();
    QString dgw;
    void removeRoute();
    void saveSettings();
    void delFiles();
    void writeOnFile(QString path,QString input);
    QString workingPath;
    QString upPath ="";
    QString configPath ="";
};
#endif // EBOLAVPN_H
