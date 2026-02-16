#include "tcpclient.h"
#include "ui_tcpclient.h"

#include "protocol.h"

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

void TcpClient::on_send_btn_clicked()
{
    QString strMsg = ui->lineEdit->text();
    if (!strMsg.isEmpty()) {
        PDU *pdu = mkPDU(strMsg.size());
        pdu->uiMsgType = 8888;
        memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.size());
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    } else {
        QMessageBox::warning(this, "信息发送", "发送的信息不能为空");
    }
}

