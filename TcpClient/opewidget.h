#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include "friend.h"
#include "book.h"

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>

class OpeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OpeWidget(QWidget *parent = nullptr);

private:
    QListWidget *m_pListW;
    Friend *m_pFriend;
    Book *m_pBook;
    QStackedWidget *m_pSW;

signals:


};

#endif // OPEWIDGET_H
