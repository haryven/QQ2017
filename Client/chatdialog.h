#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include<QCloseEvent>
#include<QUdpSocket>
#include<QColorDialog>
#include<QAbstractScrollArea>

#include<QDateTime>
#include<QFileDialog>

#include"msgtype.h"
#include"sndfileserver.h"
#include"rcvfileclient.h"

namespace Ui {
class ChatDialog;
}


class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = 0, QString UsrName = "", QString HostName = "", QString IPAddr = "");
    ~ChatDialog();
    friend QDataStream& operator>>(QDataStream& in,FontSet& Font);
    friend QDataStream& operator<<(QDataStream& out,FontSet& Font);

private slots:
    void ProcessPendingData();//接收数据

    void GetFileName(QString FileName,quint16 Port);//从文件服务器获取文件名

    void ProcessFile(QString sUsrName,QString sAddr,quint16 sPort,QString sFileName);//处理收文件消息

    void on_ToolBtn_Blod_clicked();//加粗

    void on_ToolBtn_UnderLine_clicked();//下划线

    void on_ToolBtn_Italic_clicked();//斜体

    void on_ToolBtn_Color_clicked();//颜色

    void on_ToolBtn_LogSave_clicked();//保存聊天记录

    void on_ToolBtn_SndFile_clicked();//发送文件

    void on_ToolBtn_Clear_clicked();//清除聊天记录

    void on_Btn_SndMsg_clicked();//发送消息

    void on_fontComboBox_currentFontChanged(const QFont &f);//设置字体

    void on_comboBox_FontSize_currentIndexChanged(int index);//设置字体大小

    QHostAddress GetBrodcastAddr();
protected:
    void closeEvent(QCloseEvent *e);//窗口关闭事件
    void InitChatWidget();           //初始化聊天窗
    void SndMsg(MsgType type,QString desAddr = "");    //发送消息
    void  ProcessUsrEnter(QString sUsrName,QString sHostName,QString sAddr);//处理用户加入
    void  ProcessUsrLeft(QString sUsrName,QString CurrentTime);//处理用户离开
    //FontSet GetCurrentFontSet();   //获取当前字体
    void setFont(FontSet font);     //设置文本编辑框字体
    void ShowMsg(QString UsrName,FontSet Font, QString Msg);//显示聊天框消息
    bool SaveChatLog(const QString& filename);

private:
    Ui::ChatDialog *ui;
    QString m_UsrName;
    QString m_HostName;
    QString m_IpAddr;
    quint16 m_port;
    QUdpSocket *m_ChatSocket;
    FontSet m_font;
    QString m_FileName;
    quint16 m_TcpPort;
    SndFileServer* m_FileServer;
    QString m_obj;


};

#endif // CHATDIALOG_H
