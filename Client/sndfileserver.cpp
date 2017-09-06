#include "sndfileserver.h"
#include "ui_sndfileserver.h"

SndFileServer::SndFileServer(QWidget *parent,QString IPAddr) :
    QDialog(parent),m_IpAddr(IPAddr),
    ui(new Ui::SndFileServer)
{
    ui->setupUi(this);
    m_Port = 5555;
    m_server = NULL;
    m_localFile = NULL;
    m_TotalByte = 0;
    m_ByteWriten = 0;
    m_Sndsize = 4*1024;
    InitServer();

}

SndFileServer::~SndFileServer()
{
    if(m_localFile != NULL)
    {
        delete m_localFile;
        m_localFile = NULL;
    }
    delete ui;
}

/****************************/
void SndFileServer::InitServer()
{
    this->setWindowTitle("发送文件");
    ui->Btn_Snd->setEnabled(false);
    ui->progressBar->reset();
    ui->label_Status->setText("选择文件：");
    if(m_server == NULL)
    {
        m_server = new QTcpServer(this);
    }
    m_server->close();
}

//打开文件
void SndFileServer::on_Btn_Open_clicked()
{
    QString FileName = QFileDialog::getOpenFileName(this, tr("Open File"));
    if(!FileName.isEmpty())
      {
           m_FileName = FileName.right(FileName.size()-FileName.lastIndexOf('/')-1);
           ui->label_Status->setText(tr("要传送的文件：%1").arg(m_FileName));
           ui->Btn_Snd->setEnabled(true);
           //ui->Btn_Open->setEnabled(false);
       }
    m_FileRoute = FileName;
    m_localFile= new QFile(m_FileRoute);
}

//处理并发连接
void SndFileServer::ProcessConnect()
{

    Reset();
    ui->label_Status->setText(tr("正在发送:%1").arg(m_FileName));
    QTcpSocket *pendSockt = m_server->nextPendingConnection();
    connect(pendSockt,SIGNAL(bytesWritten(qint64)),this,SLOT(UpdateProgressBar(qint64)));
    connect(pendSockt,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));


    QDataStream out(&m_outBlock,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    if(!m_localFile->open(QFile::ReadOnly))
    {
        qDebug()<<"open file error";
        ui->label_Status->setText(tr("open file error"));
        return;
    }
    m_TotalByte = m_localFile->size() + sizeof(quint64);
    out<<m_TotalByte;
    pendSockt->write(m_outBlock);
    m_outBlock.resize(0);
}
//更新进度条
void SndFileServer::UpdateProgressBar(qint64 bytes)
{
    m_ByteWriten += bytes;
    QTcpSocket * ptcpSocket = (QTcpSocket*)sender();
    ui->progressBar->setMaximum(m_TotalByte);
    ui->progressBar->setValue(m_ByteWriten);

    if(m_TotalByte > m_ByteWriten)
    {
        m_outBlock = m_localFile->read(qMin(m_Sndsize,m_TotalByte-m_ByteWriten));
        ptcpSocket->write(m_outBlock);
        m_outBlock.resize(0);
    }
    else
    {
        qDebug()<<"发送了："<<m_ByteWriten;
        ui->label_Status->setText(tr("发送文件 %1 成功").arg(m_FileName));
        m_localFile->close();
        ptcpSocket->close();
        ui->Btn_Open->setEnabled(true);
        ui->Btn_Open->setEnabled(true);
    }
}
//发送
void SndFileServer::on_Btn_Snd_clicked()
{
    m_server->disconnect(m_server,SIGNAL(newConnection()),this,SLOT(ProcessConnect()));
    m_server->close();
    if(!m_server->listen(QHostAddress(m_IpAddr),m_Port))
    {
        qDebug()<<m_server->errorString();
        close();
        return;
    }
    connect(m_server,SIGNAL(newConnection()),this,SLOT(ProcessConnect()));
    ui->label_Status->setText(tr("等待对方接受"));
    ui->Btn_Open->setEnabled(false);

    emit SndFileName(m_FileName,m_Port);
}

void SndFileServer::Reset()//重置服务器参数
{
    m_outBlock.resize(0);
    m_TotalByte = 0;
    m_ByteWriten = 0;
    if(m_localFile->isOpen())
    {
        m_localFile->close();
    }
}

void SndFileServer::displayError(QAbstractSocket::SocketError)
{
    QTcpSocket *socket = (QTcpSocket*)sender();
    qDebug() << "tcpClient error:" <<socket->errorString();
    socket->close();
    ui->progressBar->reset();
    ui->label_Status->setText(tr("error and reset"));
    Reset();
}
