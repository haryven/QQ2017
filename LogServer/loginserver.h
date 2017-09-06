#ifndef LOGINSERVER_H
#define LOGINSERVER_H

#include <QWidget>
#include<QUdpSocket>
#include<QHostAddress>
#include<QHostInfo>
#include<QDebug>

namespace Ui {
class LoginServer;
}

class LoginServer : public QWidget
{
    Q_OBJECT

public:
    explicit LoginServer(QWidget *parent = 0);
    ~LoginServer();
private slots:
    void ReadPendingData();
protected:
    void InitCtrls();
    QString GetIP();
    void ProcessSignIn(QString usrName,QString passwd, QHostAddress sender, quint16 port);
    void ProcessSignUp(QString usrName,QString passwd, QHostAddress sender, quint16 port);
private:
    Ui::LoginServer *ui;
    QUdpSocket *m_svrSocket;
    //QMap<QHostAddress,int> m_clientAddr;
    QMap<QString,QString> *m_clientInfo;
    bool m_type;

};

#endif // LOGINSERVER_H
