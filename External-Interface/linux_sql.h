#ifndef LINUX_SQL_H
#define LINUX_SQL_H

#include <QWidget>
///////////////////////////////////////////////////////////////////////////////
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QSettings>
///////////////////////////////////////////////////////////////////////////////
class LINUX_SQL : public QWidget
{
    Q_OBJECT
public:
    explicit LINUX_SQL(QWidget *parent = 0);
signals:
public slots:
    ///////////////////////////////////////////////////////////////////////////////
public:
    QVariant Buffer[10000];
    QSqlDatabase db, dbtwo;
public slots:
    int selectMax(QVariant table);
    int selectMax_net(QVariant table);

    bool openSql(QVariant sqldata);
    bool open_net_Sql(QVariant sqldata);

    bool closeSql();
    bool createSql(QVariant table, QVariant param);
    bool insertSql(QVariant table, QVariant title, QVariant param,  QVariant id);
    bool updateSql(QVariant table, QVariant title, QVariant param,  QVariant id);
    QVariant selectSql(QVariant table,  QVariant title,  QVariant id);
    bool deleteSql(QVariant table,  QVariant id);
    bool deleteAll(QVariant table);
    bool deleteTab(QVariant table);
    bool isTabExst(QVariant table, QSqlDatabase);
    ///////////////////////////////////////////////////////////////////////////////
};

#endif // LINUX_SQL_H
