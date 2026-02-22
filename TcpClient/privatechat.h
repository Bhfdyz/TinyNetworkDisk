#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include "protocol.h"

#include <QWidget>

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    static PrivateChat &getInstance();

    ~PrivateChat();

    void setChatName(QString strName);
    void updateMsg(const PDU *pdu);

private slots:
    void on_sendMsg_pb_clicked();

private:
    explicit PrivateChat(QWidget *parent = nullptr);

    Ui::PrivateChat *ui;
    QString m_strChatName;
    QString m_strLoginName;
};

#endif // PRIVATECHAT_H
