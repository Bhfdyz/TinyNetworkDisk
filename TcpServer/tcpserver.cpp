#include "tcpserver.h"
#include "ui_tcpserver.h"
#include "mytcpserver.h"

#include <QFile>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);

    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);


}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    // 读取配置文件
    QFile file(":/server.txt");
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
