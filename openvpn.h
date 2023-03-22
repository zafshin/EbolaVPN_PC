#ifndef OPENVPN_H
#define OPENVPN_H

#include <QObject>
#include <QProcess>


class openVPN : public QObject
{
    Q_OBJECT
public:
    openVPN(QString configPath, QString ovpnPath);
    void start();
public slots:
    void stop();
signals:
    void readOutput(QString output);
    void isClosed();
    void isSettingRoute();
    void isConnected();
    void isReconnecting();
    void isAthFail();
private:
    QProcess *process;
private slots:
    void readLine();
    void finished(int exitCode, QProcess::ExitStatus et);
};

#endif // OPENVPN_H
