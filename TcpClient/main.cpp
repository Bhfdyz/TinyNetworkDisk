#include "tcpclient.h"
// #include "online.h"
// #include "opewidget.h"
// #include <friend.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // TcpClient w;
    // w.show();
    // OpeWidget pw;
    // pw.show();
    // Online on;
    // on.show();
    // Friend fr;
    // fr.show();
    TcpClient::getInstance().show();


    return a.exec();
}
