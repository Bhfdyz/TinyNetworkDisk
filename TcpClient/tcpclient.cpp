#include "tcpclient.h"
#include "ui_tcpclient.h"

#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();

    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
    connect(&m_tcpSocket, &QTcpSocket::connected, this, &TcpClient::showConnect);


}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    // 读取配置文件
    QFile file(":/client.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QString strData = data.toStdString().c_str();
        file.close();

        // 处理配置文件
        strData.replace("\r\n", " ");
        qDebug() << strData;
        QStringList strList = strData.split(" ");
        // for (auto i : strList) {
        //     qDebug() << "--->" << i;
        // }
        m_strIP = strList[0];
        m_usPort = strList[1].toUShort();
        // qDebug() << "ip:" << m_strIP;
        // qDebug() << "port:" << m_usPort;
    } else {
        qDebug() << "config文件打开失败";
    }
}

void TcpClient::showConnect()
{
    QMessageBox::information(this, "连接服务器", "连接服务器成功");
}
