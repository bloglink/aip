#ifndef SQLDESKTOP_H
#define SQLDESKTOP_H

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QWidget>
#include <QThread>
#include <QProcess>
#include <QDateTime>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QStackedWidget>
#include <QProgressDialog>

#include "sqldisplay.h"
#include "sqlnetwork.h"
#include "sqlexports.h"
#include "sqlsnowuid.h"
#include "sqlhistory.h"
#include "sqlproduct.h"

#define WIDTH 125

class SqlDesktop : public QWidget
{
    Q_OBJECT
public:
    explicit SqlDesktop(QWidget *parent = 0);
    ~SqlDesktop();
signals:
    void sendAppMsg(QVariantMap msg);
    void sendSqlMap(QVariantMap msg);
private slots:
    void initUI();
    void initDir();
    void initSql();
    void initLayout();
    void initSqlDisplay();
    void initSqlProduct();
    void initSqlNetwork();
    void initSqlHistory();
    void initSqlExports();
    void initSqlSignOut();
    void sendSqlExports(QVariantMap msg);
    void initProgressBar();
    void updateSqlExport();
    void createSqlRecord();
    void backupSqlRecord();
    void vacuumSqlRecord();
    void clickButton();
    QString existsFlashDisk();
    void deleteFlashDisk();
    void recvQuan(QVariantMap msg);
    void recvOver(QVariantMap msg);
    void recvSqlMap(QVariantMap msg);
    void recvSqlDat(QVariantMap msg);
    int getNumber(QString numb);
private:
    QStackedWidget *stack;
    QVBoxLayout *btnLayout;
    QList<QPushButton*> buttons;
    QVariantMap tmpMap;
    QThread *thread;
    QProgressDialog *pBar;
    QTimer *timer;
    int time;
    int quan;
    QLabel *text;
    QMap<int, int> items;
};

#endif // SQLDESKTOP_H
