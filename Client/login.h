#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include<QUdpSocket>
namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
private slots:
    void on_Btn_login_clicked();
    void ProcessReact();
    void on_Btn_signup_clicked();

public:QString GetIP();

protected:
    void SndLogRequest();
    void SndSignUpRequest();
    bool RequestConnect();
private:
    void InitLogDialog();
private:
    Ui::Login *ui;
    QUdpSocket *m_logSocket;
    QString m_svrAddr;
    quint16 m_svrPort;
    bool m_type;


};

#endif // LOGIN_H
