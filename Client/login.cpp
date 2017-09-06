#include "login.h"
#include "ui_login.h"

#include"chatdialog.h"

#include<QHostAddress>
#include<QHostInfo>
#include<QMessageBox>


Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
   // this->setAttribute(Qt::);
    m_logSocket = NULL;
    m_svrAddr = "192.168.43.77";
    m_svrPort = 2018;
    m_type = true;
    InitLogDialog();
}
void Login::InitLogDialog()
{
    if(m_logSocket == NULL)
    {
        m_logSocket = new QUdpSocket(this);
    }
    this->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
    this->setWindowIcon(QIcon(":/images/qq.png"));
    this->setWindowTitle("QQ2018");
    ui->lineEdit_passwd->setEchoMode(QLineEdit::Password);
   //shezhibaomi

}

Login::~Login()
{
    delete ui;
    if(m_logSocket)
    {
        delete m_logSocket;
        m_logSocket = NULL;
    }
}
//登录
void Login::on_Btn_login_clicked()
{
    if(ui->lineEdit_passwd->text() == "" || ui->lineEdit_usrName->text() == "")
    {
        QMessageBox::warning(this,"温馨提示：","账号或密码不能为空",QMessageBox::Ok);
        return;
    }
    if(!RequestConnect())
    {
        QMessageBox::warning(this,"温馨提示：","连接失败",QMessageBox::Ok);
        return;
    }
     m_type = true;
    SndLogRequest();
}


void Login::SndLogRequest()
{
    QByteArray sndInfo;
    QDataStream out(&sndInfo,QIODevice::WriteOnly);
    QString usrName = ui->lineEdit_usrName->text().trimmed();
    QString passwd = ui->lineEdit_passwd->text().trimmed();
    out<<m_type<<usrName<<passwd;
    m_logSocket->writeDatagram(sndInfo,QHostAddress(m_svrAddr),m_svrPort);
}
//处理服务器反馈
void Login::ProcessReact()
{
    QByteArray rcv;
    QDataStream in(&rcv,QIODevice::ReadOnly);
    QString Strreact;
    QHostAddress sender;
    quint16 port;
    while(m_logSocket->hasPendingDatagrams())
    {
        rcv.resize(m_logSocket->pendingDatagramSize());
        m_logSocket->readDatagram(rcv.data(),rcv.size(),&sender,&port);
        in>>Strreact;
    }
    qDebug()<<Strreact;
    if(m_logSocket)
    {
        disconnect(m_logSocket,SIGNAL(readyRead()),this,SLOT(ProcessReact()));
        m_logSocket->abort();
    }
    if(Strreact == "permit")//提前释放udp socket
    {
        QMessageBox::warning(this,"温馨提示","登陆成功",QMessageBox::Ok);
        ChatDialog *ChatClient = new ChatDialog(this,ui->lineEdit_usrName->text(),QHostInfo::localHostName(),GetIP());     
        ChatClient->exec();

        //delete ChatClient;
        //free(ChatClient);
    }
    else if(Strreact == "reject")
    {
        QMessageBox::warning(this,"温馨提示","账号不存在，或密码错误",QMessageBox::Ok);
    }
    else if(Strreact == "OK")
    {
        QMessageBox::warning(this,"温馨提示","注册成功",QMessageBox::Ok);   
    }
    else if(Strreact == "existed")
    {
        QMessageBox::warning(this,"温馨提示","注册失败，账户已存在",QMessageBox::Ok);
    }
}
//获取本机IP
QString Login::GetIP()
{
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    QHostAddress addr;
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol /*&& address.toString().section( ".",-1,-1 ) != "1"*/)
        {
             addr = address;
        }
    }
    return addr.toString();
}

//注册
void Login::on_Btn_signup_clicked()
{
    if(ui->lineEdit_passwd->text() == "" || ui->lineEdit_usrName->text() == "")
    {
        QMessageBox::warning(this,"温馨提示：","账号或密码不能为空",QMessageBox::Ok);
        return;
    }
    if(!RequestConnect())
    {
        QMessageBox::warning(this,"温馨提示：","连接失败",QMessageBox::Ok);
        return;
    }
     m_type = false;
    SndSignUpRequest();
}
//绑定UDPsocket
bool Login::RequestConnect()
{
    if(m_logSocket == NULL)
    {
        return false;
    }
    m_logSocket->abort();
    disconnect(m_logSocket,SIGNAL(readyRead()),this,SLOT(ProcessReact()));
    m_logSocket->bind(QHostAddress(GetIP()),2002,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(m_logSocket,SIGNAL(readyRead()),this,SLOT(ProcessReact()));
    return true;
}
//发送注册请求
void Login::SndSignUpRequest()
{
    //先填写信息
    //发送信息
    QByteArray sndInfo;
    QDataStream out(&sndInfo,QIODevice::WriteOnly);
    QString usrName = ui->lineEdit_usrName->text().trimmed();
    QString passwd = ui->lineEdit_passwd->text().trimmed();
    out<<m_type<<usrName<<passwd;
    m_logSocket->writeDatagram(sndInfo,QHostAddress(m_svrAddr),m_svrPort);

}

