#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include "privatechat.h"

#include <QInputDialog>
#include <QMessageBox>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTE = new QTextEdit;
    m_pFriendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;

    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pShowOnlineUserPB = new QPushButton("显示在线用户");
    m_pSearchUsrPB = new QPushButton("查找用户");
    m_pMsgSendPB = new QPushButton("信息发送");
    m_pPrivateChatPB = new QPushButton("私聊");

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDelFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pShowOnlineUserPB);
    pRightPBVBL->addWidget(m_pSearchUsrPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVBL);

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline = new Online;

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUserPB, &QPushButton::clicked, this, &Friend::showOnline);
    connect(m_pSearchUsrPB, &QPushButton::clicked, this, &Friend::searchUsr);
    connect(m_pFlushFriendPB, &QPushButton::clicked, this, &Friend::flushFriend);
    connect(m_pDelFriendPB, &QPushButton::clicked, this, &Friend::delFriend);
    connect(m_pPrivateChatPB, &QPushButton::clicked, this, &Friend::privateChat);
    connect(m_pMsgSendPB, &QPushButton::clicked, this, &Friend::groupChat);

}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if (pdu == NULL) {
        return;
    }
    m_pOnline->showUsr(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    m_pFriendListWidget->clear();
    if (pdu == NULL) {
        return;
    }
    uint uiSize = pdu->uiMsgLen / 32;
    char caName[32] = {'\0'};
    for (uint i = 0; i < uiSize; ++i) {
        qDebug() << 1;
        memcpy(caName, (char*)(pdu->caMsg) + i * 32, 32);
        m_pFriendListWidget->addItem(caName);
    }
}

void Friend::updateGroupMsg(PDU *pdu)
{
    QString strMsg = QString("%1 says: %2").arg(pdu->caData).arg((char*)pdu->caMsg);
    m_pShowMsgTE->append(strMsg);
}

void Friend::showOnline()
{
    if (m_pOnline->isHidden()) {
        m_pOnline->show();

        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    } else {
        m_pOnline->hide();
    }
}

void Friend::searchUsr()
{
    QString name = QInputDialog::getText(this, "搜索", "用户名");
    m_strSearchName = name;
    if (!name.isEmpty()) {
        qDebug() << name;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        memcpy(pdu->caData, name.toStdString().c_str(), name.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }

}

void Friend::flushFriend()
{
    QString strName = TcpClient::getInstance().loginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData, strName.toStdString().c_str(), strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::delFriend()
{
    if (m_pFriendListWidget->currentItem() == NULL) {
        return;
    }
    QString strName = m_pFriendListWidget->currentItem()->text();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    QString loginName = TcpClient::getInstance().loginName();
    memcpy(pdu->caData, loginName.toStdString().c_str(), loginName.size());
    memcpy(pdu->caData + 32, strName.toStdString().c_str(), strName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

}

void Friend::privateChat()
{
    if (m_pFriendListWidget->currentItem() == NULL) {
        return;
    }
    QString strChatName = m_pFriendListWidget->currentItem()->text();
    PrivateChat::getInstance().setChatName(strChatName);
    if (PrivateChat::getInstance().isHidden()) {
        PrivateChat::getInstance().show();
    }


}

void Friend::groupChat()
{
    QString strMsg = m_pInputMsgLE->text();
    if (!strMsg.isEmpty()) {
        PDU *pdu = mkPDU(strMsg.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
        strncpy(pdu->caData, TcpClient::getInstance().loginName().toStdString().c_str(), TcpClient::getInstance().loginName().size());
        strncpy((char*)(pdu->caMsg), strMsg.toStdString().c_str(), strMsg.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        m_pInputMsgLE->clear();
    }
}
