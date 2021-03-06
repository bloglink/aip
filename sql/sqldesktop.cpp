#include "sqldesktop.h"
#include <QApplication>

SqlDesktop::SqlDesktop(QWidget *parent) : QWidget(parent)
{
    initUI();
    initDir();
    initSql();
}

SqlDesktop::~SqlDesktop()
{
    thread->quit();
    thread->wait();
}

void SqlDesktop::initUI()
{
    initLayout();
    initSqlDisplay();
    initSqlProduct();
    initSqlHistory();
    initSqlNetwork();
    initSqlExports();
    initSqlSignOut();
}

void SqlDesktop::initDir()
{
    QDir dir;
    if (!dir.exists("nandflash")) {
        dir.mkdir("nandflash");
    }
    if (!dir.exists("./nandflash/sqlite.db")) {
        QFile file("./nandflash/sqlite.db");
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.close();
    }
    if (!dir.exists("nandflash/record.db")) {
        QFile file("nandflash/record.db");
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.close();
    }
}

void SqlDesktop::initSql()
{
    QSqlDatabase sqlite = QSqlDatabase::addDatabase("QSQLITE", "sqlite");
    sqlite.setDatabaseName("./nandflash/sqlite.db");
    if (sqlite.open()) {
    } else {
        qDebug() << "sqlite:" << sqlite.lastError();
    }
    QSqlDatabase record = QSqlDatabase::addDatabase("QSQLITE", "record");
    record.setDatabaseName("./nandflash/record.db");
    if (record.open()) {
    } else {
        qDebug() << "record:" << record.lastError();
    }
    backupSqlRecord();
    createSqlRecord();
    tmpMap.insert("enum", QMessageBox::Open);
    tmpMap.insert("sqliteName", "sqlite");
    tmpMap.insert("recordName", "record");
    emit sendSqlMap(tmpMap);
    tmpMap.clear();
}

void SqlDesktop::initLayout()
{
#ifdef __arm__
    this->setWindowFlags(Qt::FramelessWindowHint);
#endif
    stack = new QStackedWidget(this);
    btnLayout = new QVBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->setSpacing(0);

    text = new QLabel(this);
    text->setWordWrap(true);
    text->setFixedSize(WIDTH, 200);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(btnLayout);
    leftLayout->addStretch();
    leftLayout->addWidget(text);

    QWidget *leftWidget = new QWidget(this);
    leftWidget->setObjectName("leftWidget");
    leftWidget->setLayout(leftLayout);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(leftWidget);
    layout->addWidget(stack);

    this->setLayout(layout);
    this->resize(800, 600);
}

void SqlDesktop::initSqlDisplay()
{
    QString objName = "sqldisplay";
    SqlDisplay *sqldisplay = new SqlDisplay(this);
    sqldisplay->setObjectName(objName);
    stack->addWidget(sqldisplay);
    connect(sqldisplay, SIGNAL(sendSqlMap(QVariantMap)), this, SLOT(recvSqlMap(QVariantMap)));
    connect(this, SIGNAL(sendSqlMap(QVariantMap)), sqldisplay, SLOT(recvSqlMap(QVariantMap)));

    QPushButton *btnsqldisplay = new QPushButton(tr("近期数据"), this);
    btnsqldisplay->setFlat(true);
    btnsqldisplay->setCheckable(true);
    btnsqldisplay->setFixedSize(WIDTH, 60);
    btnsqldisplay->setFocusPolicy(Qt::NoFocus);
    buttons.append(btnsqldisplay);
    btnLayout->addWidget(btnsqldisplay);
    btnsqldisplay->setObjectName(objName);
    connect(btnsqldisplay, SIGNAL(clicked(bool)), this, SLOT(clickButton()));
}

void SqlDesktop::initSqlProduct()
{
    QString objName = "sqlproduct";
    SqlProduct *sqlproduct = new SqlProduct(this);
    sqlproduct->setObjectName(objName);
    stack->addWidget(sqlproduct);
    connect(this, SIGNAL(sendSqlMap(QVariantMap)), sqlproduct, SLOT(recvSqlMap(QVariantMap)));

    QPushButton *btnsqlproduct = new QPushButton(tr("近期产量"), this);
    btnsqlproduct->setFlat(true);
    btnsqlproduct->setCheckable(true);
    btnsqlproduct->setFixedSize(WIDTH, 60);
    btnsqlproduct->setFocusPolicy(Qt::NoFocus);
    buttons.append(btnsqlproduct);
    btnLayout->addWidget(btnsqlproduct);
    btnsqlproduct->setObjectName(objName);
    connect(btnsqlproduct, SIGNAL(clicked(bool)), this, SLOT(clickButton()));
}

void SqlDesktop::initSqlNetwork()
{
    SqlNetwork *sqlnetwork = new SqlNetwork(this);
    sqlnetwork->setObjectName("sqlnetwork");
    stack->addWidget(sqlnetwork);
    connect(this, SIGNAL(sendSqlMap(QVariantMap)), sqlnetwork, SLOT(recvSqlMap(QVariantMap)));

    QPushButton *btnsqlnetwork = new QPushButton(tr("网络管理"), this);
    btnsqlnetwork->setFlat(true);
    btnsqlnetwork->setCheckable(true);
    btnsqlnetwork->setFixedSize(WIDTH, 60);
    btnsqlnetwork->setFocusPolicy(Qt::NoFocus);
    buttons.append(btnsqlnetwork);
    btnLayout->addWidget(btnsqlnetwork);
    btnsqlnetwork->setObjectName("sqlnetwork");
    connect(btnsqlnetwork, SIGNAL(clicked(bool)), this, SLOT(clickButton()));
}

void SqlDesktop::initSqlHistory()
{
    SqlHistory *sqlhistory = new SqlHistory(this);
    sqlhistory->setObjectName("sqlhistory");
    stack->addWidget(sqlhistory);
    connect(sqlhistory, SIGNAL(sendSqlMap(QVariantMap)), this, SLOT(recvSqlMap(QVariantMap)));

    QPushButton *btnsqlnetwork = new QPushButton(tr("历史数据"), this);
    btnsqlnetwork->setFlat(true);
    btnsqlnetwork->setCheckable(true);
    btnsqlnetwork->setFixedSize(WIDTH, 60);
    btnsqlnetwork->setFocusPolicy(Qt::NoFocus);
    buttons.append(btnsqlnetwork);
    btnLayout->addWidget(btnsqlnetwork);
    btnsqlnetwork->setObjectName("sqlhistory");
    connect(btnsqlnetwork, SIGNAL(clicked(bool)), this, SLOT(clickButton()));
}

void SqlDesktop::initSqlExports()
{
    thread = new QThread(this);
    SqlExports *sqlexports = new SqlExports;
    sqlexports->setObjectName("sqlexports");
    sqlexports->moveToThread(thread);
    thread->start();
    connect(sqlexports, SIGNAL(sendSqlMap(QVariantMap)), this, SLOT(recvSqlMap(QVariantMap)));
    connect(this, SIGNAL(sendSqlMap(QVariantMap)), sqlexports, SLOT(recvSqlMap(QVariantMap)));
}

void SqlDesktop::initSqlSignOut()
{
    QPushButton *btnLogout = new QPushButton(tr("退出设置"), this);
    btnLogout->setFlat(true);
    btnLogout->setCheckable(true);
    btnLogout->setFixedSize(WIDTH, 60);
    btnLogout->setFocusPolicy(Qt::NoFocus);
    buttons.append(btnLogout);
    btnLayout->addWidget(btnLogout);
    btnLogout->setObjectName("");
    connect(btnLogout, SIGNAL(clicked(bool)), this, SLOT(hide()));
}

void SqlDesktop::sendSqlExports(QVariantMap msg)
{
    QString path = existsFlashDisk();
    if (path.isEmpty()) {
#ifdef __arm__
        text->setText(tr("未发现U盘"));
        QMessageBox::warning(this, "", tr("请插入U盘"), QMessageBox::Ok);
#else
        text->setText(tr("导出取消"));
#endif
        return;
    }
    tmpMap = msg;
    tmpMap.insert("path", path);
    emit sendSqlMap(tmpMap);
    tmpMap.clear();
    initProgressBar();
}

void SqlDesktop::initProgressBar()
{
    pBar = new QProgressDialog(this, Qt::Dialog);
    pBar->setWindowTitle(tr("数据导出进度"));
    pBar->setLabelText(tr("正在查询数据"));
    pBar->setFixedWidth(300);
    pBar->show();
}

void SqlDesktop::createSqlRecord()
{
    QSqlQuery query(QSqlDatabase::database("record"));
    QString cmd;
    cmd = "create table if not exists aip_record (";
    cmd += "uuid bigint primary key, guid bigint, numb integer,";
    cmd += "item text,parm text,rslt text,pass text)";
    if (!query.exec(cmd))
        qDebug() << "aip_record:" << query.lastError();
}

void SqlDesktop::backupSqlRecord()
{
#ifdef __arm__
    QProcess cmd;
    cmd.start("du -s /mnt/nandflash/record.db");
    cmd.waitForFinished();
    int ss = 0;
    QByteArray df = cmd.readAll();
    QString sf = df.mid(0, df.indexOf("\t"));
    ss = sf.toInt();
    if (ss > 80000) {
        QString w = tr("当前存储的数据量较多,请点击OK后备份到SD卡");
        QMessageBox::warning(this, "", w, QMessageBox::Ok);
        QString name = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        QString p = "cp /mnt/nandflash/record.db /mnt/sdcard/";
        p.append(name + ".db");
        cmd.start(p);
        cmd.waitForFinished();
        vacuumSqlRecord();
    }
#endif
}

void SqlDesktop::vacuumSqlRecord()
{
    QSqlDatabase::database("record").close();
    QSqlDatabase::database("record").open();

    QSqlQuery query(QSqlDatabase::database("record"));
    if (!query.exec("drop table if exists aip_record"))
        qDebug() << query.lastError();
    if (!query.exec("vacuum"))
        qDebug() << query.lastError();
}

void SqlDesktop::clickButton()
{
    QString sourceName = QObject::sender()->objectName();
    for (int i=0; i < buttons.size(); i++) {
        if (buttons.at(i)->objectName() != sourceName) {
            buttons.at(i)->setChecked(false);
        }
    }
    for (int i=0; i < stack->count(); i++) {
        if (stack->widget(i)->objectName() == sourceName) {
            stack->setCurrentIndex(i);
            break;
        }
    }
}

QString SqlDesktop::existsFlashDisk()
{
    QString path;
    QString name = QDateTime::currentDateTime().toString("yyMMddhhmmss");
#ifdef __arm__
    QProcess cmddf;
    cmddf.start("df -h");
    cmddf.waitForFinished();
    QByteArray bytedf = cmddf.readAll();
    cmddf.deleteLater();

    QStringList listdf = QString(bytedf).split("\n");
    for (int i=0; i < listdf.size(); i++) {
        if (listdf.at(i).startsWith("/dev/sd")) {
            QStringList tmp = listdf.at(i).split(" ");
            path = tmp.last();
        }
    }
    if (!path.isEmpty()) {
        path.append("/" + name + ".csv");
    }
#else
    path = QFileDialog::getSaveFileName(this,
                                        tr("Open Config"),
                                        name,
                                        tr("Config Files (*.csv)"));
    path += ".csv";
#endif
    return path;
}

void SqlDesktop::deleteFlashDisk()
{
#ifdef __arm__
    QProcess cmddf;
    cmddf.start("df -h");
    cmddf.waitForFinished();
    QByteArray bytedf = cmddf.readAll();

    QStringList listdf = QString(bytedf).split("\n");
    QString path;
    for (int i=0; i < listdf.size(); i++) {
        if (listdf.at(i).startsWith("/dev/sd")) {
            QStringList tmp = listdf.at(i).split(" ");
            if (tmp.last().startsWith("/mnt/usb")) {
                path = tmp.first();
                cmddf.start(tr("umount %1").arg(path));
                cmddf.waitForFinished();
            }
        }
    }
    cmddf.deleteLater();
#endif
}

void SqlDesktop::updateSqlExport()
{
    time--;
    time = qMax(time, 0);
#ifdef __arm__
    int t = quan*25/LENTH;
#else
    int t = quan/LENTH*0.5;
#endif
    int r = (t-time)*100/t -1;
    pBar->setLabelText(tr("共找到%1条数据,预计剩余%2s").arg(quan).arg(time));
    pBar->setValue(r);
}

void SqlDesktop::recvQuan(QVariantMap msg)
{
    quan = msg.value("quan").toInt();
#ifdef __arm__
    time = quan*25/LENTH;
#else
    time = quan/LENTH*0.5;
#endif
    pBar->setValue(1);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateSqlExport()));
    timer->start(1000);
}

void SqlDesktop::recvOver(QVariantMap msg)
{
    if (msg.value("fail").toString() == "fail") {
        text->setText("导出失败");
    }
    timer->stop();
    pBar->deleteLater();
    timer->deleteLater();
    deleteFlashDisk();
}

void SqlDesktop::recvSqlMap(QVariantMap msg)
{
    switch (msg.value("enum").toInt()) {
    case QMessageBox::Close:
        recvOver(msg);
        break;
    case QMessageBox::Abort:
        text->setText(tr("导出失败"));
        break;
    case QMessageBox::Question:
        recvQuan(msg);
        break;
    case QMessageBox::Information:
        qDebug() << msg.value("text").toString();
        text->setText(msg.value("text").toString());
        break;
    case QMessageBox::Apply:
        sendSqlExports(msg);
        break;
    default:
        break;
    }
}

void SqlDesktop::recvSqlDat(QVariantMap msg)
{
    QSqlQuery query(QSqlDatabase::database("record"));
    SqlSnowUid snow;
    quint64 uuid = snow.getId();
    items.clear();
    QStringList content = msg.value("data").toString().split("\n");
    for (int i=0; i < content.size(); i++) {
        QStringList temp = QString(content.at(i)).split("@");
        if (temp.size() >= 4) {
            int numb = getNumber(temp.at(0));
            query.prepare("insert into aip_record values(?,?,?,?,?,?,?)");
            query.addBindValue(snow.getId());
            query.addBindValue(uuid);
            query.addBindValue(QString::number(numb));
            query.addBindValue(temp.at(0));
            query.addBindValue(temp.at(1));
            query.addBindValue(temp.at(2));
            query.addBindValue(temp.at(3));
            if (!query.exec()) {
                qDebug() << "insert error" << query.lastError();
            }
        }
    }
    query.prepare("insert into aip_record values(?,?,?,?,?,?,?)");
    query.addBindValue(snow.getId());
    query.addBindValue(uuid);
    query.addBindValue(1);
    query.addBindValue(msg.value("user").toString());
    query.addBindValue(msg.value("temp").toString());
    query.addBindValue(msg.value("code").toString());
    query.addBindValue(msg.value("numb").toString());
    if (!query.exec()) {
        qDebug() << "insert error" << query.lastError();
    }
    query.prepare("insert into aip_record values(?,?,?,?,?,?,?)");
    query.addBindValue(uuid);
    query.addBindValue(uuid);
    query.addBindValue(0);
    query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
    query.addBindValue(QTime::currentTime().toString("hh:mm:ss"));
    query.addBindValue(msg.value("type").toString());
    query.addBindValue(msg.value("pass").toString());
    if (!query.exec()) {
        qDebug() << "insert error" << query.lastError();
    }
}

int SqlDesktop::getNumber(QString msg)
{
    int numb = 0;
    int item = 0;
    QStringList names;
    names << tr("电阻") << tr("反嵌") << tr("绝缘") << tr("交耐") << tr("直耐")
          << tr("匝间") << tr("电参") << tr("电感") << tr("堵转") << tr("低启")
          << tr("霍尔") << tr("负载") << tr("空载") << tr("反势");
    for (int i=0; i < names.size(); i++) {
        if (msg.startsWith(names.at(i))) {
            item = i+1;
        }
    }
    names.clear();
    names << tr("电阻")<< tr("反嵌") << tr("绝缘") << tr("交耐") << tr("直耐")
          << tr("匝间") << tr("电参") << tr("电感") << tr("堵转") << tr("低启")
          << tr("FG") << tr("负载") << tr("空载") << tr("BEMF");
    for (int i=0; i < names.size(); i++) {
        if (msg.startsWith(names.at(i))) {
            item = i+1;
        }
    }
    if (item == 0) {
        if (msg.startsWith(tr("-电阻-"))) {
            numb = 0x0180;
        } else if (msg.startsWith(tr("旋向"))) {
            numb = 0x0280;
        } else if (msg.startsWith(tr("转向"))) {
            numb = 0x0780;
        } else if (msg.startsWith(tr("-电感-"))) {
            numb = 0x0880;
        } else {
            numb = 0xFFFF;
        }
    } else {
        items[item]++;
        numb = 256*item + items[item];
    }
    return numb;
}
