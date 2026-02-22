#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    static OpeDB &getInstance();
    ~OpeDB();

    bool handleRegist(const char *name, const char *pwd);
    bool handleLogin(const char *name, const char *pwd);
    void handleOffline(const char *name);
    QStringList handleAllOnline();
    int handleSearchUsr(const char *name);
    int handleAddFriend(const char *pername, const char *name);
    void handleAddFriendFin(const char *pername, const char *name);
    QStringList handleFlushFriend(const char *name);
    void handleDelFriend(const char *name, const char *friendname);

private:
    explicit OpeDB(QObject *parent = nullptr);
    void init();

    QSqlDatabase m_db; // 连接数据库

signals:
};

#endif // OPEDB_H
