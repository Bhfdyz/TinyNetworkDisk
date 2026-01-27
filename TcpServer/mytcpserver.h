#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>

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

};

#endif // MYTCPSERVER_H
