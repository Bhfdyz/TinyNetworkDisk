#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "opedb.h"

#include <QTcpSocket>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();

private:
    QString m_strName;

signals:
    void offline(MyTcpSocket *mysocket);

public slots:
    void recvMsg();
    void clientOffline();

};

#endif // MYTCPSOCKET_H
