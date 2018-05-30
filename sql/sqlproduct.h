#ifndef SQLPRODUCT_H
#define SQLPRODUCT_H

#include <QLabel>
#include <QDebug>
#include <QWidget>
#include <QLayout>
#include <QDateEdit>
#include <QShowEvent>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlTableModel>

#include "sqlsnowuid.h"

class SqlProduct : public QWidget
{
    Q_OBJECT
public:
    explicit SqlProduct(QWidget *parent = 0);
private slots:
    void initUI();
    void initView();
    void initText();
    void initLayout();
    void createSqlite();
    void updateSqlite();
    void insertSqlite();
    void selectSqlite();
    virtual void showEvent(QShowEvent *e);
    void recvSqlMap(QVariantMap msg);
private:
    QHBoxLayout *btnLayout;
    QDateEdit *stop;

    QString sqlName;
    QTableView *view;
    QSqlTableModel *mView;
};

#endif // SQLPRODUCT_H
