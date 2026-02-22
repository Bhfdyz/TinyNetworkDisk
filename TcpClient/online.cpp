#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"

#include <QDebug>
#include <QMessageBox>

Online::Online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showUsr(PDU *pdu)
{
    if (pdu == NULL) {
        return;
    }
    ui->listWidget->clear();
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32];
    for (uint i = 0; i < uiSize; ++i) {
        memcpy(caTmp, (char*)pdu->caMsg + i * 32, 32);
        ui->listWidget->addItem(caTmp);
    }
}

void Online::on_pushButton_clicked()
{
    QListWidgetItem *pItem = ui->listWidget->currentItem();
    if (pItem->text() == TcpClient::getInstance().loginName()) {
        QMessageBox::information(this, "添加好友", "不能添加自己为好友");
        return;
    }
    if (!pItem->text().isEmpty()) {
        QString strPerUsrName = pItem->text();
        QString strLoginName = TcpClient::getInstance().loginName();
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
        memcpy(pdu->caData, strPerUsrName.toStdString().c_str(), strPerUsrName.size());
        memcpy(pdu->caData + 32, strLoginName.toStdString().c_str(), strLoginName.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

