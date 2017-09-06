#ifndef RCVFILECLIENT_H
#define RCVFILECLIENT_H

#include <QDialog>
#include<QTcpSocket>
#include<QFile>
#include<QHostAddress>

namespace Ui {
class rcvFileClient;
}

class rcvFileClient : public QDialog
{
    Q_OBJECT

public:
    explicit rcvFileClient(QWidget *parent = 0,QString route = "", QString IPAddr = "", quint16 Port = 0);
    ~rcvFileClient();

private slots:
    void RcvData();    
    void on_Btn_Cancle_clicked();
    void displayError(QAbstractSocket::SocketError socketError);
    void resetVar();

    void on_pushButton_Retrail_clicked();

private:
    Ui::rcvFileClient *ui;
    QString m_Addr;
    quint16 m_Port;
    QTcpSocket *m_tcpCilnt;
    QByteArray m_InBlock;
    qint64 m_TotalBytes;
    qint64 m_RcvdBytes;
    QFile *m_localFile;
    QString m_route;
    
};

#endif // RCVFILECLIENT_H
