#ifndef SNDFILESERVER_H
#define SNDFILESERVER_H

#include <QDialog>
#include<QTcpServer>
#include<QTcpSocket>
#include<QFileDialog>
#include<QAbstractSocket>

namespace Ui {
class SndFileServer;
}

class SndFileServer : public QDialog
{
    Q_OBJECT

public:
    explicit SndFileServer(QWidget *parent = 0,QString IPAddr = "");
    ~SndFileServer();
signals:
    void SndFileName(QString FileName,quint16 Port);
private slots:
    void ProcessConnect();//处理并发连接

    void UpdateProgressBar(qint64 bytes);//更新进度条

    void on_Btn_Open_clicked();//打开文件

    void on_Btn_Snd_clicked();//发送文件

    void displayError(QAbstractSocket::SocketError);
private:
    void InitServer();
    void Reset();

private:
    Ui::SndFileServer *ui;
    QString m_IpAddr;
    quint16 m_Port;
    QTcpServer *m_server;
    QString m_FileName;
    QString m_FileRoute;
    QFile *m_localFile;
    qint64 m_TotalByte;
    qint64 m_ByteWriten;
    QByteArray m_outBlock;
    qint64 m_Sndsize;
};

#endif // SNDFILESERVER_H
