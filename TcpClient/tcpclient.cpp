#include "tcpclient.h"
#include "ui_tcpclient.h"
#include "privatechat.h"

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

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;

    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::loginName()
{
    return m_strLoginName;
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
                OpeWidget::getInstance().show();
                this->hide();

            } else if (strcmp(pdu->caData, LOGIN_FAILED) == 0) {
                QMessageBox::information(this, "登录", LOGIN_FAILED);
            }
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:{
            OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:{
            if (strcmp(SEARCH_USR_NO, pdu->caData) == 0) {
                QMessageBox::information(this, "搜索", QString("%1:not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
            } else if (strcmp(SEARCH_USR_ONLINE, pdu->caData) == 0) {
                QMessageBox::information(this, "搜索", QString("%1:online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
            } else if (strcmp(SEARCH_USR_OFFLINE, pdu->caData) == 0) {
                QMessageBox::information(this, "搜索", QString("%1:offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
            char caPerName[32] = {'\0'};
            char caName[32] = {'\0'};
            strncpy(caPerName, pdu->caData, 32);
            strncpy(caName, pdu->caData + 32, 32);
            int ret = QMessageBox::information(this, "添加好友", QString("%1 want to add you as friend?").arg(caName),
                                     QMessageBox::Yes, QMessageBox::No);
            PDU *respdu = mkPDU(0);
            memcpy(respdu->caData, pdu->caData, 32);
            memcpy(respdu->caData + 32, pdu->caData + 32, 32);
            if (ret == QMessageBox::Yes) {
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
            } else {
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
            }
            m_tcpSocket.write((char*)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
            QMessageBox::information(this, "添加好友", pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{
            OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:{
            QMessageBox::information(this, "删除好友", "删除好友成功");
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{
            PrivateChat::getInstance().updateMsg(pdu);

            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:{
            OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);
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
        m_strLoginName = strName;
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

