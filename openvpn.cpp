#include "openvpn.h"

openVPN::openVPN(QString configPath, QString ovpnPath)
{
    process = new QProcess(this);
    QStringList arg = {configPath};
    process->setArguments(arg);
    process->setProgram(ovpnPath);
    QStringList dir = ovpnPath.split('\\');
    QString wdir;
    for(int i =0; i< dir.length()-1;i++){
        wdir+= dir[i] + "\\";
    }
    process->setWorkingDirectory(wdir);
    connect(process,&QProcess::readyRead,this,&openVPN::readLine);
    connect(process,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(finished(int, QProcess::ExitStatus)));
}
void openVPN::finished(int exitCode, QProcess::ExitStatus et){
    emit isClosed();
}

void openVPN::start(){
    process->start();
}
void openVPN::stop(){
    process->kill();
}

void openVPN::readLine(){
    while (process->canReadLine()) {
        QString line = process->readLine();
            emit readOutput(line);
            if(line.lastIndexOf("Initialization Sequence Completed") > -1)
                emit isConnected();
            else if(line.lastIndexOf("open_tun") > -1)
                emit isSettingRoute();
            else if(line.lastIndexOf("Restart") > -1)
                emit isReconnecting();
            else if(line.lastIndexOf("auth-failure") > -1)
                emit isAthFail();
        }
}
