#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QTcpServer>
#include <QList>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    static MyTcpServer &getInstance();
    MyTcpServer &operator=(const MyTcpServer&) = delete;
    MyTcpServer(const MyTcpServer&) = delete;
    void incomingConnection(qintptr socketDescriptor);


private:
    MyTcpServer();

    QList<MyTcpSocket*> m_tcpSocketList;

public slots:
    void deleteSocket(MyTcpSocket *mysocket);

};

#endif // MYTCPSERVER_H
