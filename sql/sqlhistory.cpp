#include "sqlhistory.h"

SqlHistory::SqlHistory(QWidget *parent) : QWidget(parent)
{
    initUI();
}

void SqlHistory::initUI()
{
    initView();
    initButton();
    initLayout();
}

void SqlHistory::initView()
{
    view = new QTableView(this);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
#if (QT_VERSION <= QT_VERSION_CHECK(5, 0, 0))
    view->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#else
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif
}

void SqlHistory::initButton()
{
    btnLayout = new QHBoxLayout;
    btnLayout->addStretch();

    QPushButton *btnDelete = new QPushButton(this);
    btnDelete->setFixedSize(97, 44);
    btnDelete->setText(tr("删除当前"));
    connect(btnDelete, SIGNAL(clicked(bool)), this, SLOT(recvDelete()));
    btnLayout->addWidget(btnDelete);

    QPushButton *btnExport = new QPushButton(this);
    btnExport->setFixedSize(97, 44);
    btnExport->setText(tr("导出当前"));
    connect(btnExport, SIGNAL(clicked(bool)), this, SLOT(recvExport()));
    btnLayout->addWidget(btnExport);
}

void SqlHistory::initLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(view);
    layout->addLayout(btnLayout);
    this->setLayout(layout);
}

void SqlHistory::updateFile()
{
    mView = new QFileSystemModel(this);
    mView->setFilter(QDir::Files | QDir::NoDotAndDotDot);
    view->clearSelection();
    view->setModel(mView);
#ifdef __arm__
    view->setRootIndex(mView->setRootPath("/mnt/sdcard"));
#else
    view->setRootIndex(mView->setRootPath("/home/link/"));
#endif
}

void SqlHistory::recvExport()
{
    int row = view->currentIndex().row();
    if (row < 0)
        return;
    QString path = mView->filePath(view->currentIndex());
    if (QSqlDatabase::database("history").isValid()) {
        QSqlDatabase::database("history").close();
        QSqlDatabase::removeDatabase("history");
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "history");
    db.setDatabaseName(path);
    if (db.open()) {
    } else {
        qDebug() << "record:" << db.lastError();
    }
    tmpMap.insert("enum", QMessageBox::Apply);
    tmpMap.insert("name", "history");
    emit sendSqlMap(tmpMap);
    tmpMap.clear();
}

void SqlHistory::recvDelete()
{
    int row = view->currentIndex().row();
    if (row < 0)
        return;
    QString path = mView->filePath(view->currentIndex());
    QFile file(path);
    file.remove();
}

void SqlHistory::showEvent(QShowEvent *e)
{
    updateFile();
    e->accept();
}

