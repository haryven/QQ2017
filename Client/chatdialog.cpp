#include "chatdialog.h"
#include "ui_chatdialog.h"

#include<QMessageBox>
#include<QDataStream>

ChatDialog::ChatDialog(QWidget *parent, QString UsrName, QString HostName, QString IPAddr) :
    QDialog(parent),m_UsrName(UsrName),m_HostName(HostName),m_IpAddr(IPAddr),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
    m_ChatSocket = NULL;
    m_FileServer = NULL;
    m_obj = "";
    m_port = 2003;
    InitChatWidget();

    // qDebug()<<m_UsrName<<m_HostName<<m_IpAddr;
    SndMsg(UsrEnter);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

QDataStream& operator>> (QDataStream& in,FontSet& font)//重载Qdatastream>>
{
    in>>font.Bold>>font.Color>>font.Font>>font.FontSize>>font.Italic>>font.Underline;
    return in;
}
QDataStream& operator<< (QDataStream& out,FontSet& font)//重载Qdatastream<<
{
    out<<font.Bold<<font.Color<<font.Font<<font.FontSize<<font.Italic<<font.Underline;
    return out;
}

/*******************************************/
//初始化聊天窗
void ChatDialog::InitChatWidget()
{
    if(m_ChatSocket == NULL)
    {
        m_ChatSocket = new QUdpSocket(this);
        m_ChatSocket->bind(m_port,QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress);
        connect(m_ChatSocket,SIGNAL(readyRead()),this,SLOT(ProcessPendingData()));
    }
    //设置聊天框窗口风格
    this->setWindowTitle(tr("QT2017              %1").arg(m_UsrName));
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint|Qt::WindowMinimizeButtonHint);
    this->setWindowIcon(QIcon(":/images/qq.png"));

    //设置工具栏图标
    ui->ToolBtn_Blod->setIcon(QIcon(":/images/bold.png"));
    ui->ToolBtn_UnderLine->setIcon(QIcon(":/images/under.png"));
    ui->ToolBtn_Color->setIcon(QIcon(":/images/color.png"));
    ui->ToolBtn_Italic->setIcon(QIcon(":/images/italic.png"));
    ui->ToolBtn_SndFile->setIcon(QIcon(":/images/send.png"));
    ui->ToolBtn_LogSave->setIcon(QIcon(":/images/save.png"));
    ui->ToolBtn_Clear->setIcon(QIcon(":/images/clear.png"));
    //设置字体大小下拉框
    QStringList FontSize;
    FontSize<<"8"<<"10"<<"12"<<"14"<<"16"<<"18"<<"20"<<"22"<<"24";
    ui->comboBox_FontSize->addItems(FontSize);
    ui->comboBox_FontSize->setCurrentIndex(2);
    //设置字体下拉框不可编辑
    ui->fontComboBox->setEditable(false);

    //禁止编辑
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //整行选择
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //单行
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setMouseTracking(true);//开启捕获鼠标功能
    ui->tableWidget->setStyleSheet("selection-background-color:blue");  //设置选中行颜色
    QStringList list;
    list<<"用户名"<<"主机名"<<"IP地址";
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setColumnWidth(0,60);
    ui->tableWidget->setColumnWidth(1,60);
    ui->tableWidget->setColumnWidth(2,100);
    ui->tableWidget->setHorizontalHeaderLabels(list);
    ui->tableWidget->verticalHeader()->setHidden(true);//隐藏列表头

    m_font.Bold = false;
    m_font.Underline = false;
    m_font.Font = ui->fontComboBox->font();
    m_font.FontSize = ui->comboBox_FontSize->itemText(2).toInt();
    m_font.Italic = false;
    m_font.Color = Qt::black;
    setFont(m_font);
}

//窗口关闭事件
void ChatDialog::closeEvent(QCloseEvent *e)
{ 
    /*switch( QMessageBox::information(this,tr("提示"),tr("你确定退出该软件?"),tr("确定"), tr("取消"),0,1))
    {
    case 0:
        SndMsg(UsrLeft);
        e->accept();
        break;
    case 1:
    default:
        e->ignore();
        break;
    }*/
    SndMsg(UsrLeft);
    //e->accept();
    //  QWidget::closeEvent(e);

}

void ChatDialog::ProcessPendingData()  //接收并处理并发UDP数据
{

    while(m_ChatSocket->hasPendingDatagrams())
    {
        QByteArray rcv;
        QDataStream in(&rcv,QIODevice::ReadOnly);
        // QHostAddress sender;
        // quint16 port;
        QString sUsrName,sAddr,sHostName,sMsg,CurrenTime,sFileName;
        quint16 sPort;
        CurrenTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        rcv.resize(m_ChatSocket->pendingDatagramSize());
        qDebug()<<"read:"<<m_ChatSocket->readDatagram(rcv.data(),rcv.size()/*,&sender,&port*/);

        int msgtype;
        in>>msgtype;
        qDebug()<<"msgtype"<<msgtype;
        switch(msgtype)
        {
            case Msg:
            {
                qDebug()<<"usr Msg:";
                FontSet font;
                in>>sUsrName>>font>>sMsg;
                ShowMsg(sUsrName,font,sMsg);
                break;
            }
            case UsrEnter:
            {
                qDebug()<<"usr UsrEnter:";
                in>>sUsrName>>sHostName>>sAddr;
                ProcessUsrEnter(sUsrName,sHostName,sAddr);
                break;
            }
            case UsrLeft:
            {
                qDebug()<<"usr UsrLeft:";
                in>>sUsrName;
                ProcessUsrLeft(sUsrName,CurrenTime);
                break;
            }
            case SndFile:
            {
                qDebug()<<"usr SndFile:";
                in>>sUsrName>>sAddr>>sPort>>sFileName;
                ProcessFile(sUsrName,sAddr,sPort,sFileName);
                break;
            }
            case RefuseFile:
            {
                qDebug()<<"RefuseFile";
                in>>sUsrName;
                if(m_UsrName == sUsrName)
                {
                   m_FileServer->close();
                   m_FileServer = NULL;
                }
                break;
            }
        }
    }

}
//发送UDP消息
void ChatDialog::SndMsg(MsgType type, QString desAddr)
{

    QByteArray snd;
    QDataStream out(&snd,QIODevice::WriteOnly);
    snd.clear();
    // QString
    out<<type;
    switch (type)
    {
        case Msg:
        {
            if(ui->textEdit->toPlainText() == "")
            {
                QMessageBox::warning(0,tr("警告"),tr("输入内容不能为空"),QMessageBox::Ok);
                return;
            }
            out<<m_UsrName<<m_font<<ui->textEdit->toPlainText();
            ui->textEdit->clear();
            //  ui->textEditMsg->setFocus();
            break;
        }
        case UsrEnter:
        {
            qDebug()<<"UsrEnter"<<type;
            out<<m_UsrName<<m_HostName<<m_IpAddr;
            break;
        }
        case UsrLeft:
        {
            out<<m_UsrName;
            break;
        }
        case SndFile:
        {
            out<<m_UsrName<<m_IpAddr<<m_TcpPort<<m_FileName;
            break;
        }
        case RefuseFile:
        {
            out<<m_obj;//防止在本地调试的时候IP地址一样造成自己也收到发文件消息消息，然后发送拒绝；
            break;
        }
    }
    if(desAddr.length() == 0)
    {
        qDebug()<<tr("write to %1").arg(GetBrodcastAddr().toString())<<m_ChatSocket->writeDatagram(snd,snd.length(),GetBrodcastAddr(),m_port);
    }
    else
    {
        qDebug()<<tr("write to %1").arg(desAddr)<<m_ChatSocket->writeDatagram(snd,QHostAddress(desAddr),m_port);
    }
}
//处理用户加入
void ChatDialog::ProcessUsrEnter(QString UserName,QString HostName,QString IpAddress)
{
    // qDebug()<<UserName<<HostName<<IpAddress;
    bool isEmpty = ui->tableWidget->findItems(UserName,Qt::MatchExactly).isEmpty();
    if(isEmpty)
    {
        QTableWidgetItem *usr = new QTableWidgetItem(UserName);
        QTableWidgetItem *addr= new QTableWidgetItem(IpAddress);
        QTableWidgetItem *host= new QTableWidgetItem(HostName);
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0,0,usr);
        ui->tableWidget->setItem(0,1,host);
        ui->tableWidget->setItem(0,2,addr);
        ui->textBrowser_ChatMsg->setTextColor(Qt::gray);
        ui->textBrowser_ChatMsg->setCurrentFont(QFont("Times New Roman"));
        ui->textBrowser_ChatMsg->append(tr("%1在线!").arg(UserName));
        ui->label_OnlineNum->setText(tr("在线人数：%1").arg(ui->tableWidget->rowCount()));
        SndMsg(UsrEnter);
    }
}
//处理用户离开
void ChatDialog::ProcessUsrLeft(QString UsrName,QString CurrentTime)
{
    if(!ui->tableWidget->findItems(UsrName,Qt::MatchExactly).isEmpty())
    {
        int rowNum = ui->tableWidget->findItems(UsrName,Qt::MatchExactly).first()->row();
        ui->tableWidget->removeRow(rowNum);
        ui->textBrowser_ChatMsg->setTextColor(Qt::gray);
        ui->textBrowser_ChatMsg->setCurrentFont(QFont("Times New Roman"));
        ui->textBrowser_ChatMsg->append(tr("%1于%2离开！").arg(UsrName).arg(CurrentTime));
        ui->label_OnlineNum->setText(tr("在线人数：%1").arg(ui->tableWidget->rowCount()));
    }
}
//加粗
void ChatDialog::on_ToolBtn_Blod_clicked()
{
    if(m_font.Bold)
    {
        m_font.Bold = false;
    }
    else
    {
        m_font.Bold = true;
    }
    setFont(m_font);
}
//下划线
void ChatDialog::on_ToolBtn_UnderLine_clicked()
{
    if(m_font.Underline)
    {
        m_font.Underline = false;
    }
    else
    {
        m_font.Underline = true;
    }
    setFont(m_font);
}
//斜体
void ChatDialog::on_ToolBtn_Italic_clicked()
{
    if(m_font.Italic)
    {
        m_font.Italic = false;
    }
    else
    {
        m_font.Italic = true;
    }
    setFont(m_font);
}
//颜色
void ChatDialog::on_ToolBtn_Color_clicked()
{
    QColorDialog FontColor(this);
    FontColor.exec();
    m_font.Color = FontColor.currentColor();
    setFont(m_font);
}

//设置字体

void ChatDialog::setFont(FontSet font)
{
    // ui->plainTextEdit->setfo
    ui->textEdit->setFont(font.Font);
    if(font.Bold)
    {
        ui->textEdit->setFontWeight(QFont::Bold);
    }
    else
    {
        ui->textEdit->setFontWeight(QFont::Normal);
    }
    ui->textEdit->setFontItalic(font.Italic);
    ui->textEdit->setFontUnderline(font.Underline);
    ui->textEdit->setFontPointSize(font.FontSize);
    ui->textEdit->setTextColor(font.Color);
    QString str_Msg = ui->textEdit->toPlainText();
    ui->textEdit->clear();
    ui->textEdit->append(str_Msg); //更新当前文本的字体
}

//保存聊天记录
void ChatDialog::on_ToolBtn_LogSave_clicked()
{
    if(ui->textBrowser_ChatMsg->document()->isEmpty())
    {
        QMessageBox::warning(0,tr("警告"),tr("聊天记录为空，无法保存！"),QMessageBox::Ok);
        //return;
    }
    else
    {
        QString FileName = QFileDialog::getSaveFileName(this,tr("保存聊天记录"),tr("聊天记录.txt"),tr("文本（*.txt）;;所有文件（*.*） ")) ;
        if(!FileName.isEmpty())
            SaveChatLog(FileName);
    }
}
//保存聊聊天日志文件
bool ChatDialog::SaveChatLog(const QString& filename)
{
    QFile file(filename);
    if(!file.open(QFile::WriteOnly|QFile::Text))
    {
        QMessageBox::warning(this,tr("保存文件"),tr("文件无法保存%1：\n%2").arg(filename).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out<<ui->textBrowser_ChatMsg->toPlainText();
    return true;
}
//发送文件
void ChatDialog::on_ToolBtn_SndFile_clicked()
{

    if(ui->tableWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(this,"温馨提示：","请选择目标用户",QMessageBox::Ok);
        return;
    }
    int row = ui->tableWidget->currentRow();
    if(ui->tableWidget->item(row,0)->text() == m_UsrName)
    {
        QMessageBox::warning(this,"温馨提示","请选择其他用户",QMessageBox::Ok);
        return;
    }

    m_FileServer = new SndFileServer(this,m_IpAddr);
    m_FileServer->setAttribute(Qt::WA_DeleteOnClose,true);
     disconnect(m_FileServer,SIGNAL(SndFileName(QString,quint16)),this,SLOT(GetFileName(QString,quint16)));
     connect(m_FileServer,SIGNAL(SndFileName(QString,quint16)),this,SLOT(GetFileName(QString,quint16)));
     m_FileServer->show();
}

//从文件服务器获取文件名和端口号并通过udp发送给目标
void ChatDialog::GetFileName(QString FileName,quint16 Port)
{
    m_FileName = FileName;
    m_TcpPort = Port;
    int row = ui->tableWidget->currentRow();
    QString IPAddr = ui->tableWidget->item(row,2)->text(); 
    SndMsg(SndFile,IPAddr);
}

//处理收到的发文件消息
void ChatDialog::ProcessFile(QString sUsrName,QString sAddr,quint16 sPort,QString sFileName)
{
    if(m_UsrName == sUsrName)//过滤自己发文件请求
    {
        return;
    }
    int btn = QMessageBox::information(this,tr("接收文件"),tr("来自%1的文件：%2，是否接收？").
                                       arg(sUsrName).arg(sFileName)
                                       ,QMessageBox::Yes,QMessageBox::No);
    if(btn==QMessageBox::Yes)
    {
        QString name= QFileDialog::getSaveFileName(0,tr("保存文件"),sFileName);
        if(!name.isEmpty())
        {

            rcvFileClient *client = new rcvFileClient(this,name,sAddr,sPort);
            client->setAttribute(Qt::WA_DeleteOnClose,true);
            client->show();
        }
        else
        {
            qDebug()<<"打开文件失败！";
            return;
        }
    }
    else
    {
        m_obj = sUsrName;
        SndMsg(RefuseFile,GetBrodcastAddr().toString());
        //SndMsg(RefuseFile,sAddr);
    }


}
//清除聊天记录
void ChatDialog::on_ToolBtn_Clear_clicked()
{
    ui->textBrowser_ChatMsg->clear();
}
//发送聊天消息
void ChatDialog::on_Btn_SndMsg_clicked()
{
    SndMsg(Msg);
}
//显示消息
void ChatDialog::ShowMsg(QString UsrName,FontSet font, QString Msg)
{
    ui->textBrowser_ChatMsg->setFont(font.Font);
    if(font.Bold)
    {
        ui->textBrowser_ChatMsg->setFontWeight(QFont::Bold);
    }
    else
    {
        ui->textBrowser_ChatMsg->setFontWeight(QFont::Normal);
    }
    ui->textBrowser_ChatMsg->setFontItalic(font.Italic);
    ui->textBrowser_ChatMsg->setFontUnderline(font.Underline);
    ui->textBrowser_ChatMsg->setFontPointSize(font.FontSize);
    ui->textBrowser_ChatMsg->setTextColor(font.Color);

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->textBrowser_ChatMsg->append("["+UsrName+"] "+time);
    ui->textBrowser_ChatMsg->append(Msg);
}
//字体
void ChatDialog::on_fontComboBox_currentFontChanged(const QFont &f)
{
    m_font.Font = f;
    setFont(m_font);
}
//字体大小
void ChatDialog::on_comboBox_FontSize_currentIndexChanged(int index)
{
    m_font.FontSize = ui->comboBox_FontSize->itemText(index).toInt();
    setFont(m_font);
}
QHostAddress ChatDialog::GetBrodcastAddr()//获取本局域网广播地址
{
    int nPos = m_IpAddr.lastIndexOf(".")+1;
    QString strIp = m_IpAddr.mid(0,nPos);  //指定要用于通信的局域网网络号;本机测试使用192.168.1.x
    strIp.append("255");
     qDebug()<<"广播地址："<<strIp;
    return QHostAddress(strIp);

}


/******************/
