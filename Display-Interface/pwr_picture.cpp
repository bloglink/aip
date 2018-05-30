#include "pwr_picture.h"
#include "ui_pwr_picture.h"

pwr_picture::pwr_picture(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pwr_picture)
{
    ui->setupUi(this);
}

pwr_picture::~pwr_picture()
{
    delete ui;
}
