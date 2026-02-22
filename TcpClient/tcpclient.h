#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "opewidget.h"

#include <QWidget>
#include <QFile>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    ~TcpClient();
    void loadConfig();

    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();
    QString loginName();

private:
    TcpClient(QWidget *parent = nullptr);
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    QTcpSocket m_tcpSocket;
    QString m_strLoginName;

public slots:
    void showConnect();
    void recvMsg();

private slots:
    void on_login_btn_clicked();
    void on_register_btn_clicked();
    void on_cancel_btn_clicked();
};
#endif // TCPCLIENT_H
