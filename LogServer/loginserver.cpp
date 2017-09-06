#include "loginserver.h"
#include "ui_loginserver.h"

LoginServer::LoginServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginServer)
{
    ui->setupUi(this);
    m_svrSocket = NULL;
    m_clientInfo = NULL;
    InitCtrls();
}

LoginServer::~LoginServer()
{
    delete ui;
    if(m_svrSocket)
    {
        delete m_svrSocket;
        free(m_svrSocket);
    }
}

/***************************************************************/
//初始化
void LoginServer::InitCtrls()
{
    if(m_svrSocket == NULL)
    {
        m_svrSocket = new QUdpSocket(this);
        qDebug()<<GetIP();
        if(m_svrSocket)
        {
            m_svrSocket->bind(QHostAddress(GetIP()),2018,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
            connect(m_svrSocket,SIGNAL(readyRead()),this,SLOT(ReadPendingData()));
        }
    }
    if(m_clientInfo == NULL)
    {
        m_clientInfo = new QMap<QString,QString>;
        m_clientInfo->insert("hhw","123");
       // m_clientInfo[QString("hhw")] = QString("123");
    }
    this->setWindowTitle("QQ Login Server");

}

//读取请求
void LoginServer::ReadPendingData()
{
    qDebug()<<"read";
    QByteArray rcv;
    QDataStream in(&rcv,QIODevice::ReadOnly);
    QString usrName,passwd;
    QHostAddress sender;
    quint16 port;

    while(m_svrSocket->hasPendingDatagrams())
    {
        rcv.resize(m_svrSocket->pendingDatagramSize());
        m_svrSocket->readDatagram(rcv.data(),rcv.size(),&sender,&port);
        in>>m_type;
        switch (m_type)
        {
        case 1:
        {
            qDebug()<<"sign in";

            in>>usrName>>passwd;
            qDebug()<<usrName<<passwd;
            ProcessSignIn(usrName,passwd,sender,port);
            break;
        }
        case 0:
        {
            qDebug()<<"sign up";
            in>>usrName>>passwd;
            qDebug()<<usrName<<passwd;
            ProcessSignUp(usrName,passwd,sender,port);
            break;
        }
        default:
              break;
        }
    }
}
//处理登录
void LoginServer::ProcessSignIn(QString usrName,QString passwd, QHostAddress sender, quint16 port)
{
    QByteArray snd;
    qDebug()<<usrName<<passwd;
    QDataStream out(&snd,QIODevice::WriteOnly);
    QMap<QString,QString>::iterator iter;
    for(iter = m_clientInfo->begin();iter != m_clientInfo->end();++iter)
    {
       /*方法一
        if(usrName == iter.key())
        {
            qDebug()<<usrName<<passwd;
        }*/
        //方法二

        if(!usrName.compare(iter.key(),Qt::CaseInsensitive) \
                && !passwd.compare(iter.value(),Qt::CaseInsensitive))
        {
            ui->textBrowser->append(QString("用户名：%1,密码：%2,IP:%3,端口：%4").arg(usrName,passwd,sender.toString(),QString::number(port)));
            out<<QString("permit");
            break;
        }
    }
    if(snd.length() == 0)
    {
        out<<QString("reject");
    }
    m_svrSocket->writeDatagram(snd,sender,port);
}

//获取本机IP
QString LoginServer::GetIP()
{
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    QHostAddress address;
    foreach(QHostAddress addr,info.addresses())
    {
        if(addr.protocol() == QAbstractSocket::IPv4Protocol /*&& addr.toString().section( ".",-1,-1 ) != "1"*/)
        {
            qDebug() <<"IPV4 Address: "<< addr.toString();
            address = addr;
        }
    }
    return address.toString();
}
//处理注册
void LoginServer::ProcessSignUp(QString usrName,QString passwd, QHostAddress sender, quint16 port)
{
    QByteArray snd;
    QDataStream out(&snd,QIODevice::WriteOnly);
    QMap<QString,QString>::iterator iter;
    for(iter = m_clientInfo->begin();iter != m_clientInfo->end();++iter)
    {
        if(!usrName.compare(iter.key(),Qt::CaseInsensitive))
        {
           out<<QString("existed");
           break;
        }
    }
    if(snd.length() == 0)
    {
        m_clientInfo->insert(usrName,passwd);
        out<<QString("OK");
    }
    m_svrSocket->writeDatagram(snd,sender,port);
}
