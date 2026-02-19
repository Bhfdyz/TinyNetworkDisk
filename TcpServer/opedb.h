#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class OpeDB : public QObject
{
    Q_OBJECT
public:
    static OpeDB &getInstance();
    ~OpeDB();

    bool handleRegist(const char *name, const char *pwd);
    bool handleLogin(const char *name, const char *pwd);
    void handleOffline(const char *name);

private:
    explicit OpeDB(QObject *parent = nullptr);
    void init();

    QSqlDatabase m_db; // 连接数据库

signals:
};

#endif // OPEDB_H
