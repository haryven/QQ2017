#include "rcvfileclient.h"
#include "ui_rcvfileclient.h"

rcvFileClient::rcvFileClient(QWidget *parent,QString route,QString IPAddr, quint16 port) :
    QDialog(parent),m_route(route),m_Addr(IPAddr),m_Port(port),
    ui(new Ui::rcvFileClient)
{
    ui->setupUi(this);

    m_TotalBytes = 0;
    m_RcvdBytes = 0;
    m_localFile = NULL;
    ui->progressBar->reset();
    m_tcpCilnt = new QTcpSocket(this);
    m_tcpCilnt->connectToHost(QHostAddress(m_Addr),m_Port);
    connect(m_tcpCilnt,SIGNAL(readyRead()),this,SLOT(RcvData()));
    if(m_localFile == NULL)
    {
        m_localFile = new QFile(m_route);
    }

}

rcvFileClient::~rcvFileClient()
{
    delete ui;
}

void rcvFileClient::RcvData()
{
    if(!m_localFile->isOpen())
    {
        if(!m_localFile->open(QFile::WriteOnly))
        {
            qDebug()<<"open file error";
            ui->label_Status->setText(tr("open file error"));
        }
    }
    qDebug()<<"正在接受：";
    QDataStream in(&m_InBlock,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_4);
    if(m_RcvdBytes <= sizeof(qint64))
    {
       if(m_tcpCilnt->bytesAvailable() >= sizeof(qint64))
       {
            m_InBlock = m_tcpCilnt->read(sizeof(qint64));
            m_RcvdBytes += m_InBlock.size();
            qDebug()<<"读了："<<m_InBlock.size()<<m_RcvdBytes;
            in>>m_TotalBytes;
            qDebug()<<"m_total:"<<m_TotalBytes;
       }
       else
       {
           return;
       }
    }

    if(m_RcvdBytes < m_TotalBytes)
    {
        m_InBlock = m_tcpCilnt->readAll();
        m_RcvdBytes += m_InBlock.size();
        qDebug()<<"2读了："<<m_InBlock.size()<<m_RcvdBytes;
        qDebug()<<"2xie了："<<m_localFile->write(m_InBlock);
    } 
    ui->progressBar->setMaximum(m_TotalBytes);
    ui->progressBar->setValue(m_RcvdBytes);

    if(m_RcvdBytes == m_TotalBytes)
    {

        qDebug()<<"接受了："<<m_RcvdBytes;
        ui->label_Status->setText("已完成");
        m_localFile->close();
        m_tcpCilnt->close();
    }
    
}
void rcvFileClient::displayError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "socket error:" << m_tcpCilnt->errorString();
    m_tcpCilnt->close();
    if(m_localFile->isOpen())
    {
       m_localFile->close();
    }

    ui->label_Status->setText(tr("try again!"));
    ui->progressBar->reset();
}
void rcvFileClient::resetVar()
{
    m_TotalBytes = 0;
    m_RcvdBytes = 0;
    m_InBlock.resize(0);
}
void rcvFileClient::on_Btn_Cancle_clicked()
{
    this->close();
}

void rcvFileClient::on_pushButton_Retrail_clicked()
{
    m_tcpCilnt->close();
    if(m_localFile->isOpen())
    {
       m_localFile->close();
    }
    resetVar();

    if(!m_localFile->open(QFile::WriteOnly))
    {
        qDebug()<<"open file error";
        ui->label_Status->setText(tr("open file error"));
    }

    m_tcpCilnt->connectToHost(QHostAddress(m_Addr),m_Port);
    connect(m_tcpCilnt,SIGNAL(readyRead()),this,SLOT(RcvData()));
}
