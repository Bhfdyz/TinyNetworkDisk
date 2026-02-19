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
    connect(&m_tcpSocket, &QTcpSocket::readyRead, this, &TcpClient::recvMsg);

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

void TcpClient::recvMsg()
{
    qDebug() << m_tcpSocket.bytesAvailable();
    uint uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    switch(pdu->uiMsgType) {
        case ENUM_MSG_TYPE_REGIST_RESPOND:{
            if (strcmp(pdu->caData, REGIST_OK) == 0) {
                QMessageBox::information(this, "注册", REGIST_OK);
            } else if (strcmp(pdu->caData, REGIST_FAILED) == 0) {
                QMessageBox::information(this, "注册", REGIST_FAILED);
            }
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_RESPOND:{
            if (strcmp(pdu->caData, LOGIN_OK) == 0) {
                QMessageBox::information(this, "登录", LOGIN_OK);
            } else if (strcmp(pdu->caData, LOGIN_FAILED) == 0) {
                QMessageBox::information(this, "登录", LOGIN_FAILED);
            }
            break;
        }
        default:
            break;
    }

    free(pdu);
    pdu = NULL;
}

void TcpClient::on_login_btn_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();

    if (!strName.isEmpty() && !strPwd.isEmpty()) {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    } else {
        QMessageBox::critical(this, "登录", "登录失败：用户名或密码为空");
    }
}


void TcpClient::on_register_btn_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();

    if (!strName.isEmpty() && !strPwd.isEmpty()) {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    } else {
        QMessageBox::critical(this, "注册", "注册失败：用户名或密码为空");
    }
}


void TcpClient::on_cancel_btn_clicked()
{

}

