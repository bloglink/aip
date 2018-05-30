#include "Widget_DCW.h"
#include "ui_Widget_DCW.h"

Widget_DCW::Widget_DCW(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_DCW)
{
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // 去掉标题栏
    ui->setupUi(this);

    Line_DCW_List[0].append(ui->dcvEdit1);
}

Widget_DCW::~Widget_DCW()
{
    delete ui;
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2015.12.30
  * brief:      初始化直耐设置
******************************************************************************/
void Widget_DCW::Init_DCW()
{
    ui->dcvEdit1->setText(Copy_DCW_List.at(20));
    ui->dcvEdit2->setText(Copy_DCW_List.at(21));
    ui->dcvEdit3->setText(Copy_DCW_List.at(22));
    ui->dcvEdit4->setText(Copy_DCW_List.at(23));
    ui->dcvEdit5->setText(Copy_DCW_List.at(24));
}
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2015.12.30
  * brief:      保存直耐配置
******************************************************************************/
void Widget_DCW::Save_DCW_Data()
{
    //更新直耐数据
    Copy_DCW_List.replace(20, ui->dcvEdit1->text());
    Copy_DCW_List.replace(21, ui->dcvEdit2->text());
    Copy_DCW_List.replace(22, ui->dcvEdit3->text());
    Copy_DCW_List.replace(23, ui->dcvEdit4->text());
    Copy_DCW_List.replace(24, ui->dcvEdit5->text());
}
void Widget_DCW::Slot_DCW_Setpoint(QString number)
{
    qDebug() << "Widget_DCW::" << number;
}
