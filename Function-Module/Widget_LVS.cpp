#include "Widget_LVS.h"
#include "ui_Widget_LVS.h"

Widget_LVS::Widget_LVS(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_LVS)
{
    ui->setupUi(this);

    //  电参设置表头
    ui->lvsTab->horizontalHeader()->setStyleSheet\
            ("QHeaderView::section{background-color:#191919;color: white;"\
             "padding-left: 4px;border: 1px solid #447684;}");
    ui->lvsTab->horizontalHeader()->setFixedHeight(25);
    ui->lvsTab->setColumnWidth(0, 100);
    ui->lvsTab->setColumnWidth(1, 100);
    ui->lvsTab->setColumnWidth(2, 100);
    ui->lvsTab->setColumnWidth(3, 100);
    ui->lvsTab->setColumnWidth(4, 100);

    LVS_Init_Flag = 0;
    ui->label_channel->hide(); ui->LVS_channel->hide();
    ui->lable_volt->hide();    ui->LVS_Volt->hide();
    ui->label_fre->hide();     ui->LVS_Fre->hide();
}

Widget_LVS::~Widget_LVS()
{
    delete ui;
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2016.11.5
  * brief:      初始化低启设置
******************************************************************************/
void Widget_LVS::Pri_LVS_Init()
{
    LVS_Init_Flag = 1;
    ui->lvsTab->setRowCount(1);
    ui->LVS_Volt->setValue(Copy_LVS_List.at(0).toInt());
    ui->LVS_Fre->setValue(Copy_LVS_List.at(1).toInt());
    ui->LVS_channel->setValue(Copy_LVS_List.at(2).toInt());
    if (Copy_LVS_List.at(5).toInt() == 2) {
        ui->radio_Turn->setChecked(true);
    } else {
        ui->radio_Pwr->setChecked(true);
    }
    QTableWidgetItem *Limit_down = new QTableWidgetItem; // 电流下限
    Limit_down->setTextAlignment(Qt::AlignCenter);
    Limit_down->setText(Copy_LVS_List.at(30));
    ui->lvsTab->setItem(0, 0, Limit_down);

    QTableWidgetItem *Limit_up = new QTableWidgetItem;   // 电流上限
    Limit_up->setTextAlignment(Qt::AlignCenter);
    Limit_up->setText(Copy_LVS_List.at(31));
    ui->lvsTab->setItem(0, 1, Limit_up);

    QTableWidgetItem *Pwr_down = new QTableWidgetItem;   // 功率下限
    Pwr_down->setTextAlignment(Qt::AlignCenter);
    Pwr_down->setText(Copy_LVS_List.at(32));
    ui->lvsTab->setItem(0, 2, Pwr_down);

    QTableWidgetItem *Pwr_up = new QTableWidgetItem;     // 功率上限
    Pwr_up->setTextAlignment(Qt::AlignCenter);
    Pwr_up->setText(Copy_LVS_List.at(33));
    ui->lvsTab->setItem(0, 3, Pwr_up);

    QTableWidgetItem *time = new QTableWidgetItem;       // 时间
    time->setTextAlignment(Qt::AlignCenter);
    time->setText(Copy_LVS_List.at(34));
    ui->lvsTab->setItem(0, 4, time);

    ui->combConf_Turn->setCurrentIndex(Copy_LVS_List.at(3).toInt()); // 转向
    //    if (LVS_Judge) // 功率和电流判定
    //    {
    //        ui->lvsTab->setColumnWidth(0, 0);
    //        ui->lvsTab->setColumnWidth(1, 0);
    //        ui->lvsTab->setColumnWidth(2, 0);
    //        ui->lvsTab->setColumnWidth(3, 0);
    //    }
    //    else // 方向判定
    //    {
    //        ui->lable_Turn->hide();
    //        ui->combConf_Turn->hide();
    //    }

    LVS_Init_Flag = 0;
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2015.12.30
  * brief:      保存低启 配置
******************************************************************************/
void Widget_LVS::Pri_LVS_Data_Save()
{
    //  保存低启
    Copy_LVS_List.replace(0, QString::number(ui->LVS_Volt->value()));
    Copy_LVS_List.replace(1, QString::number(ui->LVS_Fre->value()));
    Copy_LVS_List.replace(2, QString::number(ui->LVS_channel->value()));
    Copy_LVS_List.replace(3, QString::number(ui->combConf_Turn->currentIndex()));

    if (ui->radio_Turn->isChecked()) {
        Copy_LVS_List.replace(5, "2");
    } else {
        Copy_LVS_List.replace(5, "0");
    }
    Copy_LVS_List.replace(30, ui->lvsTab->item(0, 0)->text());
    Copy_LVS_List.replace(31, ui->lvsTab->item(0, 1)->text());
    Copy_LVS_List.replace(32, ui->lvsTab->item(0, 2)->text());
    Copy_LVS_List.replace(33, ui->lvsTab->item(0, 3)->text());
    Copy_LVS_List.replace(34, ui->lvsTab->item(0, 4)->text());
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2016.11.5
  * brief:      Conf 配置 PWR 界面
******************************************************************************/
void  Widget_LVS::Pri_LVS_Test_Parameter(QString str)
{
    str.clear();
    /*  int time;
    frame.can_id = 0x27;  frame.can_dlc = 0x07;
    frame.data[0] = 0x03;   // 设置功率参数
    frame.data[1] = 7;      // 测试项序号  8
    frame.data[2] = 1;      // 档位1-6
    time = Copy_LVS_List.at(34).toDouble()*10;
    frame.data[3] = time/256; // 测试时间
    frame.data[4] = time%256;
    frame.data[5] = 0;    // 转向
    frame.data[6] = 0x00; // PG 供电电压
    canSend(frame);
    usleep(500);

    frame.can_id = 0x28;  frame.can_dlc = 0x07;
    frame.data[0] = 0x03; // 设置功率参数
    frame.data[1] = 7;    // 测试项序号  8
    frame.data[2] = 1;  // 档位1-6
    time = Copy_LVS_List.at(34).toDouble()*10;
    frame.data[3] = time/256; // 测试时间
    frame.data[4] = time%256;
    frame.data[5] = 0; // 转向
    frame.data[6] = 0x00; // PG 供电电压
    canSend(frame);
    usleep(500);   */
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2016.11.5
  * brief:      Conf 配置 LVS 界面
******************************************************************************/
void Widget_LVS::Pub_Conf_Set_LVS(QString str, int flag)
{
    switch (flag) {
    case 1:
        Pri_LVS_Init();
        break;
    case 2:
        Pri_LVS_Data_Save();
        break;
    case 3:
        Pri_LVS_Test_Parameter(str);
        break;
    case 4:
        Pub_Conf_power_chose(str.toInt());
        break;
    default:
        //
        break;
    }
}
void Widget_LVS::Pub_Conf_power_chose(int value)
{
    if (value == 0) {
        //
    } else if (value == 1) {
        ui->lable_volt->show();  ui->LVS_Volt->show();
        ui->label_fre->show();   ui->LVS_Fre->show();
    } else if (value == 2) {
        ui->label_channel->show(); ui->LVS_channel->show();
    } else if (value == 3) {
        ui->lable_volt->show();  ui->LVS_Volt->show();
    } else {
        //
    }
}
void Widget_LVS::on_lvsTab_cellChanged(int row,  int column)
{
    QString str;
    int i, After_Point = 0;
    if (LVS_Init_Flag == 0) {
        LVS_Init_Flag = 1;

        str = ui->lvsTab->item(row, column)->text();
        if (str == NULL) {
            ui->lvsTab->item(row, column)->setText("0");
        }
        for (i = 0; i < str.length(); i++) { //判断输入是" 0-9 . "
            if (((str[i] >= '0') && (str[i] <= '9')) || (str[i] == '.')) {
                //
            } else {
                ui->lvsTab->item(row, column)->setText("0");
                break;
            }
            if (str[i] == '.') { //  判断是否规范
                After_Point = str.length() - i - 1;
                if ((i == 0)) {
                    ui->lvsTab->item(row, column)->setText("0");
                }
                if (i == (str.length()-1)) {
                    ui->lvsTab->item(row, column)->setText\
                            (ui->lvsTab->item(row, column)->text().left(i));
                }
            }
        }
        if ((column == 0) || (column == 1)) { // 电流上下限制
            if (ui->lvsTab->item(row, column)->text().toDouble() > 5) {
                ui->lvsTab->item(row, column)->setText("5.00");
            }
            ui->lvsTab->item(row, column)->setText\
                    (QString::number(ui->lvsTab->item(row, column)->text().toDouble(), 'f', 2));
        }
        if ((column == 2) || (column == 3)) { // 功率上下限
            if (ui->lvsTab->item(row, column)->text().toDouble() > 3000) {
                ui->lvsTab->item(row, column)->setText("3000");
            }
            ui->lvsTab->item(row, column)->setText\
                    (QString::number(ui->lvsTab->item(row, column)->text().toDouble(), 'f', 1));
        }
        if (column == 4) { // 时间
            if (ui->lvsTab->item(row, column)->text().toDouble() > 1000) {
                ui->lvsTab->item(row, column)->setText("999.9");
            } else if (ui->lvsTab->item(row, column)->text().toDouble() < 0.5) {
                ui->lvsTab->item(row, column)->setText("0.5");
            }
            ui->lvsTab->item(row, column)->setText\
                    (QString::number(ui->lvsTab->item(row, column)->text().toDouble(), 'f', 1));
        }

        if ((column == 0) || (column == 1)) {
            if ((ui->lvsTab->item(row, 0)->text().toDouble()) > \
                    (ui->lvsTab->item(row, 1)->text().toDouble())) {
                ui->lvsTab->item(row, 0)->setText("0");
            }
        }
        if ((column == 2) || (column == 3)) {
            if ((ui->lvsTab->item(row, 2)->text().toDouble()) > \
                    (ui->lvsTab->item(row, 3)->text().toDouble())) {
                ui->lvsTab->item(row, 2)->setText("0");
            }
        }
        LVS_Init_Flag = 0;
    }
}
QStringList Widget_LVS::LVS_Test_Param()
{
    QString str;
    QStringList strSql;
    QStringList strParam, strTest;
    strTest.clear();
    strParam.clear();
    QStringList  turn;      turn.clear();
    turn << QString(tr("不测")) << QString(tr("反转")) << QString(tr("正转"));

    str = QString(tr("低启")); strTest.append(str);

    if (ui->radio_Turn->isChecked()) {
        str = turn.at(Copy_LVS_List.at(3).toInt()+1);
    } else {
        str ="I:" + Copy_LVS_List.at(31) + "A," + "P:" + Copy_LVS_List.at(33) + "W";
    }
    strParam.append(str);
    strSql.append(tr("低启"));

    QStringList Back_List;
    Back_List.append(QString::number(strTest.size()));
    Back_List.append(QString::number(strParam.size()));
    Back_List.append(QString::number(strSql.size()));
    Back_List.append(strTest);
    Back_List.append(strParam);
    Back_List.append(strSql);
    return Back_List;
}
