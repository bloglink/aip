#include "Widget_PWR.h"
#include "ui_Widget_PWR.h"

Widget_PWR::Widget_PWR(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_PWR)
{
    ui->setupUi(this);

    //  电参设置表头
    ui->pwrTab->horizontalHeader()->setStyleSheet\
            ("QHeaderView::section{background-color:#191919;color: white;"\
             "padding-left: 4px;border: 1px solid #447684;}");
    ui->pwrTab->horizontalHeader()->setFixedHeight(25);
    ui->pwrTab->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->pwrTab->setColumnWidth(0, 100);
    ui->pwrTab->setColumnWidth(1, 70);
    ui->pwrTab->setColumnWidth(2, 70);
    ui->pwrTab->setColumnWidth(3, 85);
    ui->pwrTab->setColumnWidth(4, 85);
    ui->pwrTab->setColumnWidth(5, 75);
    ui->pwrTab->setColumnWidth(6, 75);
    ui->pwrTab->setColumnWidth(7, 70);

    //  PG设置表头
    ui->pgTab->horizontalHeader()->setStyleSheet\
            ("QHeaderView::section{background-color:#191919;color: white;"\
             "padding-left: 4px;border: 1px solid #447684;}");
    ui->pgTab->horizontalHeader()->setFixedHeight(25);
    ui->pgTab->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    ui->pgTab->setColumnWidth(0, 72);
    ui->pgTab->setColumnWidth(1, 72);
    ui->pgTab->setColumnWidth(2, 72);
    ui->pgTab->setColumnWidth(3, 72);
    ui->pgTab->setColumnWidth(4, 72);
    ui->pgTab->setColumnWidth(5, 72);
    ui->pgTab->setColumnWidth(6, 72);
    ui->pwrTab->setFixedHeight(267);


    EPAR_Init_Flag = 0;
    ui->checkBox_pwm->setDisabled(true);
    //  ui->checkBox_pg->setDisabled(true);
    PWM_Row_Count = 6;

    PWR_Test_Data.clear();  PWR_Test_Data.append("0");
    PG_Test_Order.clear();

    ui->label_volt->hide();  ui->PWR_Volt->hide();
    ui->label_fre->hide();  ui->PWR_Fre->hide();
    ui->label_channel->hide(); ui->PWR_channel->hide();
    Turn_Shan = false;
    PWM_Model = "";
    Pwr_Usart_Count = 0;

    PWR_Labelhide = new QLabel(this);
    PWR_Labelhide->setGeometry(0, 0, 800, 600);
    PWR_Labelhide->hide();
    /*进行功率档位的选择*/
    Widget_Button = new QWidget(this);
    Widget_Button->setGeometry(125, 123, 219, 260);
    QGridLayout *Pwr_GridLayout = new QGridLayout;
    Button_Text.clear();
    Button_Text.append("123");
    Button_Text.append("124");
    Button_Text.append("125");
    Button_Text.append("126");
    Button_Text.append("127");
    Button_Text.append("128");
    Pwr_Button_Group = new QButtonGroup;
    for (int i = 0; i < 6; i++) {
        Pwr_Button.append(new QPushButton(this));
        Pwr_Button[i]->setText(Button_Text[i]);
        Pwr_Button[i]->setMaximumWidth(100);  Pwr_Button[i]->setMinimumHeight(36);
        Pwr_Button[i]->setStyleSheet
                ("background: qlineargradient(x1: 0,  y1: 0,  x2: 0,  y2: "\
                 "1, stop: 0 #5B5F5F,  stop: 0.5 #0C2436, stop: 1 #27405A);"\
                 "color:rgb(255,  255,  255);");
        Pwr_Button_Group->addButton(Pwr_Button[i], i);
        Pwr_GridLayout->addWidget(Pwr_Button[i], i, 0);
    }
    Pwr_Button.append(new QPushButton(this));
    Pwr_Button[6]->setText(QString(tr("空")));
    Pwr_Button[6]->setMaximumWidth(100);  Pwr_Button[6]->setMinimumHeight(36);
    Pwr_Button[6]->setStyleSheet\
            ("background: qlineargradient(x1: 0,  y1: 0,  x2: 0,  y2: "\
             "1, stop: 0 #5B5F5F,  stop: 0.5 #0C2436, stop: 1 #27405A);"\
             "color:rgb(255,  255,  255);");
    Pwr_Button_Group->addButton(Pwr_Button[6], 6);
    Pwr_GridLayout->addWidget(Pwr_Button[6], 0, 1);
    connect(Pwr_Button_Group, SIGNAL(buttonClicked(int)), this, SLOT(buttonJudge(int)));
    Widget_Button->setLayout(Pwr_GridLayout);
    Widget_Button->setStyleSheet("background-color:#191919");
    Widget_Button->hide();

    PWR_Row = 0;

    Pg_Button_Group = new QButtonGroup;
    Pg_Button_Group->addButton(ui->pushButton_1, 1);
    Pg_Button_Group->addButton(ui->pushButton_2, 2);
    Pg_Button_Group->addButton(ui->pushButton_3, 3);
    Pg_Button_Group->addButton(ui->pushButton_4, 4);
    Pg_Button_Group->addButton(ui->pushButton_5, 5);
    Pg_Button_Group->addButton(ui->pushButton_6, 6);
    connect(Pg_Button_Group, SIGNAL(buttonClicked(int)), this, SLOT(pg_buttonJudge(int)));
    for (int j = 1; j < 7; j++) {
        Pg_Button_Group->button(j)->hide();
    }
    Old_Id = 0;
}

Widget_PWR::~Widget_PWR()
{
    delete ui;
}

void Widget_PWR::mousePressEvent(QMouseEvent *event) //处理鼠标被按下事件
{
    if (ui->pwrTab->currentColumn() == 0) {
        Widget_Button->hide();   PWR_Labelhide->hide();
    }
}

void Widget_PWR::pg_buttonJudge(int id)
{
    if (PWM_Model != QString("PG")) { // -非Pg退出测试
        return;
    }
    if (Old_Id == 0) {
        //
    } else {
        Pg_Button_Group->button(Old_Id)->setStyleSheet\
                ("background: qlineargradient(x1: 0,  y1: 0,  x2: 0,  y2: 1, "\
                 "stop: 0 #5B5F5F,  stop: 0.5 #0C2436, stop: 1 #27405A);"\
                 "color:rgb(255,  255,  255);");
        Copy_PWR_List.replace(155+Old_Id-1, QString::number(ui->PG_volt->value()));
        Copy_PWR_List.replace(161, QString::number(ui->pg_count->value()));
        Copy_PWR_List.replace(5+Old_Id-1, QString::number(ui->checkBox_pg->checkState()));
        for (int i = 0; i < 2; i++) {
            Copy_PWR_List.replace(165+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 1)->text());
            Copy_PWR_List.replace(166+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 2)->text());
            Copy_PWR_List.replace(167+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 3)->text());
            Copy_PWR_List.replace(168+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 4)->text());
            Copy_PWR_List.replace(169+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 5)->text());
            Copy_PWR_List.replace(170+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 6)->text());
        }
    }

    qDebug() << "id-1" << id-1;

    Pg_Button_Group->button(id)->setStyleSheet\
            ("background: qlineargradient(x1: 0,  y1: 0,  x2: 0,  y2: 1,"\
             "stop: 0 #00A600,  stop: 0.5 #00A600, stop: 1 #27405A);"\
             "color:rgb(255,  255,  255);");
    QTableWidgetItem *PG_Item = new QTableWidgetItem;  // -PG信息
    PG_Item->setTextAlignment(Qt::AlignCenter);
    PG_Item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    PG_Item->setText(ui->pwrTab->item(id-1, 0)->text());
    ui->pgTab->setHorizontalHeaderItem(0, PG_Item);
    dd(id-1);
    Old_Id = id;
}

void Widget_PWR::dd(int data)
{
    int i;
    EPAR_Init_Flag = 1;

    qDebug() << "data-----------------" << data;
    ui->pgTab->setRowCount(0);
    ui->pgTab->setRowCount(2);

    QStringList lisit;   lisit << QString(tr("上限")) << QString(tr("下限"));
    if (Copy_PWR_List.at(5+data).toInt() == 2) {
        ui->checkBox_pg->setChecked(true);
    } else {
        ui->checkBox_pg->setChecked(false);
    }
    ui->PG_volt->setValue(Copy_PWR_List.at(155+data).toDouble());
    data = data*30;
    qDebug() << "155+data" << 155+data << "Copy_PWR_List.at(155+data).toDouble()" \
             << Copy_PWR_List.at(155+data).toDouble();
    PG_QTableWidgetItem.clear();
    for (i = 0; i < 2; i++) {
        PG_QTableWidgetItem.append(new QTableWidgetItem);
        PG_QTableWidgetItem[i]->setTextAlignment(Qt::AlignCenter);
        PG_QTableWidgetItem[i]->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        PG_QTableWidgetItem[i]->setText(lisit.at(i));
        ui->pgTab->setItem(i, 0, PG_QTableWidgetItem[i]);
    }
    for (i = 0; i < 2; i++) {
        QTableWidgetItem *pg_high = new QTableWidgetItem;    // 高电平
        pg_high->setTextAlignment(Qt::AlignCenter);
        pg_high->setText(Copy_PWR_List.at(165+data+i*15));
        ui->pgTab->setItem(i, 1, pg_high);
        QTableWidgetItem *pg_low = new QTableWidgetItem;     // 低电平
        pg_low->setTextAlignment(Qt::AlignCenter);
        pg_low->setText(Copy_PWR_List.at(166+data+i*15));
        ui->pgTab->setItem(i, 2, pg_low);
        QTableWidgetItem *pg_fre = new QTableWidgetItem;     // 频率
        pg_fre->setTextAlignment(Qt::AlignCenter);
        pg_fre->setText(Copy_PWR_List.at(167+data+i*15));
        ui->pgTab->setItem(i, 3, pg_fre);
        QTableWidgetItem *pg_duty = new QTableWidgetItem;    // 占空比
        pg_duty->setTextAlignment(Qt::AlignCenter);
        pg_duty->setText(Copy_PWR_List.at(168+data+i*15));
        ui->pgTab->setItem(i, 4, pg_duty);
        QTableWidgetItem *pg_current = new QTableWidgetItem; // 电流
        pg_current->setTextAlignment(Qt::AlignCenter);
        pg_current->setText(Copy_PWR_List.at(169+data+i*15));
        ui->pgTab->setItem(i, 5, pg_current);
        QTableWidgetItem *pg_speed_widget = new QTableWidgetItem; // 转速
        pg_speed_widget->setTextAlignment(Qt::AlignCenter);
        pg_speed_widget->setText(Copy_PWR_List.at(170+data+i*15));
        ui->pgTab->setItem(i, 6, pg_speed_widget);
    }
    qApp->processEvents();
    EPAR_Init_Flag = 0;
}
void Widget_PWR::buttonJudge(int id)
{
    Widget_Button->hide();
    PWR_Labelhide->hide();

    if (id == 6) {
        Copy_PWR_List.replace(35 + PWR_Row*20, "0");
        QTableWidgetItem *Table_Grade = new QTableWidgetItem;  //-档位
        Table_Grade->setTextAlignment(Qt::AlignCenter);
        Table_Grade->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        Table_Grade->setText("---");
        ui->pwrTab->setItem(PWR_Row, 0, Table_Grade);
    } else {
        Copy_PWR_List.replace(35 + PWR_Row*20, QString::number(id+1));
        QTableWidgetItem *Table_Grade = new QTableWidgetItem;  //-档位
        Table_Grade->setTextAlignment(Qt::AlignCenter);
        Table_Grade->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        Table_Grade->setText(Button_Text.at(id));
        ui->pwrTab->setItem(PWR_Row, 0, Table_Grade);
    }
}

/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2016.11.5
  * brief:      初始化电参设置
******************************************************************************/
void Widget_PWR::Pri_PWR_Init(QString Model) // #define  PWM_Model "COMMON"
// PG COMMON   // PG电机 普通电机
{
    int i;
    EPAR_Init_Flag = 1;
    pwrturn.clear();   pwrturn << QString(tr("不测")) \
                               << QString(tr("反转")) << QString(tr("正转"));
    PWM_Model = Model;


    ui->pwrTab->setRowCount(PWM_Row_Count);
    ui->pgTab->setRowCount(2);

    //    Box_PWR_Grade_List.clear();
    //    for (i=0; i<PWM_Row_Count; i++)
    //    {
    //        Box_PWR_Grade_List.append(new QCheckBox);
    //        Box_PWR_Grade_List[i]->setStyleSheet\
    //        ("QCheckBox::indicator {image: url(:/image/053.png);"\
    //        width: 45px;height: 45px;}QCheckBox::indicator:checked "\
    //        "{image: url(:/image/051.png);}");
    //        Box_PWR_Grade_List[i]->setText(Button_Text.at(i));
    //    //  ui->pwrTab->setCellWidget(i, 0, Box_PWR_Grade_List[i]);
    //    }


    // 总设置  0 电参    1 PG

    if (Copy_PWR_List.at(0).toInt() == 2) {
        ui->checkBox_pwm->setChecked(true);
    }
    if (Copy_PWR_List.at(2).toInt() == 2) {
        ui->checkBox_3->setChecked(true);
    }

    // 电参
    ui->PWR_Volt->setValue(Copy_PWR_List.at(25).toInt());
    ui->PWR_Fre->setValue(Copy_PWR_List.at(26).toInt());

    ui->PWR_Volt_up->setValue(Copy_PWR_List.at(27).toInt());
    ui->PWR_Volt_down->setValue(Copy_PWR_List.at(28).toInt());
    ui->PWR_Volume_value->setValue(Copy_PWR_List.at(29).toInt());
    ui->PWR_channel->setValue(Copy_PWR_List.at(30).toInt()); // 电源通道

    for (i = 0; i < PWM_Row_Count; i++) {
        if (Copy_PWR_List.at(3).toInt() == 168) {     //-是否进行了更正
            QTableWidgetItem *Table_Grade = new QTableWidgetItem;  //-档位
            Table_Grade->setTextAlignment(Qt::AlignCenter);
            Table_Grade->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            if (Copy_PWR_List.at(35+i*20).toInt() != 0) {
                Table_Grade->setText(Button_Text.at(Copy_PWR_List.at(35+i*20).toInt()-1));
                Pg_Button_Group->button(i+1)->show();
                Pg_Button_Group->button(i+1)->setText\
                        (Button_Text.at(Copy_PWR_List.at(35+i*20).toInt()-1));
            } else {
                Table_Grade->setText("---");
            }
            ui->pwrTab->setItem(i, 0, Table_Grade);
        } else {
            QTableWidgetItem *Table_Grade = new QTableWidgetItem;  //-档位
            Table_Grade->setTextAlignment(Qt::AlignCenter);
            Table_Grade->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            if (Copy_PWR_List.at(35+i*20).toInt() != 0) {
                Table_Grade->setText(Button_Text.at(i));
                Copy_PWR_List.replace(35+i*20, QString::number(i+1));
                Pg_Button_Group->button(i+1)->show();
                Pg_Button_Group->button(i+1)->setText(Button_Text.at(i));
            } else {
                Table_Grade->setText("---");
            }
            if (Copy_PWR_List.at(2).toInt() != 0) {
                Copy_PWR_List.replace(5 + i, "2");
            }
            ui->pwrTab->setItem(i, 0, Table_Grade);
        }
        //        QTableWidgetItem *Volt_down = new QTableWidgetItem; //-电压下限
        //        Volt_down->setTextAlignment(Qt::AlignCenter);
        //        Volt_down->setText(Copy_PWR_List.at(36+i*20));
        //        ui->pwrTab->setItem(i, 1, Volt_down);

        //        QTableWidgetItem *Volt_up = new QTableWidgetItem;  //-电压上限
        //        Volt_up->setTextAlignment(Qt::AlignCenter);
        //        Volt_up->setText(Copy_PWR_List.at(37+i*20));
        //        ui->pwrTab->setItem(i, 2, Volt_up);

        QTableWidgetItem *Limit_down = new QTableWidgetItem;  // -电流下限
        Limit_down->setTextAlignment(Qt::AlignCenter);
        Limit_down->setText(Copy_PWR_List.at(38 + i*20));
        ui->pwrTab->setItem(i, 1, Limit_down);

        QTableWidgetItem *Limit_up = new QTableWidgetItem;  // -电流上限
        Limit_up->setTextAlignment(Qt::AlignCenter);
        Limit_up->setText(Copy_PWR_List.at(39 + i*20));
        ui->pwrTab->setItem(i, 2, Limit_up);

        QTableWidgetItem *Pwr_down = new QTableWidgetItem;  // -功率下限
        Pwr_down->setTextAlignment(Qt::AlignCenter);
        Pwr_down->setText(Copy_PWR_List.at(40 + i*20));
        ui->pwrTab->setItem(i, 3, Pwr_down);

        QTableWidgetItem *Pwr_up = new QTableWidgetItem;  // -功率上限
        Pwr_up->setTextAlignment(Qt::AlignCenter);
        Pwr_up->setText(Copy_PWR_List.at(41+i*20));
        ui->pwrTab->setItem(i, 4, Pwr_up);

        QTableWidgetItem *Volume_pressure_down = new QTableWidgetItem;  // 容压下限
        Volume_pressure_down->setTextAlignment(Qt::AlignCenter);
        Volume_pressure_down->setText(Copy_PWR_List.at(42 + i*20));
        ui->pwrTab->setItem(i, 5, Volume_pressure_down);

        QTableWidgetItem *Volume_pressure_up = new QTableWidgetItem;    // 容压上限
        Volume_pressure_up->setTextAlignment(Qt::AlignCenter);
        Volume_pressure_up->setText(Copy_PWR_List.at(43 + i*20));
        ui->pwrTab->setItem(i, 6, Volume_pressure_up);

        QTableWidgetItem *Volume_value = new QTableWidgetItem;    // 转向
        Volume_value->setTextAlignment(Qt::AlignCenter);
        Volume_value->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        Volume_value->setText(pwrturn[Copy_PWR_List.at(44+i*20).toInt()]);
        ui->pwrTab->setItem(i, 7, Volume_value);

        QTableWidgetItem *time = new QTableWidgetItem;    // 时间
        time->setTextAlignment(Qt::AlignCenter);
        time->setText(Copy_PWR_List.at(45 + i*20));
        ui->pwrTab->setItem(i, 8, time);
    }

    // PG
    if (Copy_PWR_List.at(3).toInt() == 168) { //-是否进行了更正
        //
    } else {
        Copy_PWR_List.replace(3, "168");
    }

    if (Model == QString("PG")) {
        //
    } else {
        ui->pgTab->setRowCount(0);
        ui->checkBox_pg->hide();
        ui->label_pg_volt->hide();   ui->PG_volt->hide();
        ui->label_pg_conut->hide();  ui->pg_count->hide();
        ui->pgTab->hide();
        ui->frame->hide();
        for (int j = 1; j < 7; j++) {
            Pg_Button_Group->button(j)->hide();
        }
    }

    for (int i = 1; i < 7; i++) {
        if (!(Pg_Button_Group->button(i)->isHidden())) {
            pg_buttonJudge(i);
            break;
        }
    }

    EPAR_Init_Flag = 0;
}

/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2015.12.30
  * brief:      保存 PWR 配置
******************************************************************************/
void Widget_PWR::Pri_PWR_Data_Save()
{
    int i;
    Copy_PWR_List.replace(0, QString::number(ui->checkBox_pwm->checkState()));
    Copy_PWR_List.replace(2, QString::number(ui->checkBox_3->checkState()));

    /*保存空载*/
    Copy_PWR_List.replace(25, QString::number(ui->PWR_Volt->value()));
    Copy_PWR_List.replace(26, QString::number(ui->PWR_Fre->value()));
    Copy_PWR_List.replace(27, QString::number(ui->PWR_Volt_up->value()));
    Copy_PWR_List.replace(28, QString::number(ui->PWR_Volt_down->value()));
    Copy_PWR_List.replace(29, QString::number(ui->PWR_Volume_value->value()));
    Copy_PWR_List.replace(30, QString::number(ui->PWR_channel->value()));
    for (i = 0; i < PWM_Row_Count; i++) {
        Copy_PWR_List.replace(38+i*20, ui->pwrTab->item(i, 1)->text());
        Copy_PWR_List.replace(39+i*20, ui->pwrTab->item(i, 2)->text());
        Copy_PWR_List.replace(40+i*20, ui->pwrTab->item(i, 3)->text());
        Copy_PWR_List.replace(41+i*20, ui->pwrTab->item(i, 4)->text());
        Copy_PWR_List.replace(42+i*20, ui->pwrTab->item(i, 5)->text());
        Copy_PWR_List.replace(43+i*20, ui->pwrTab->item(i, 6)->text());
        Copy_PWR_List.replace(45+i*20, ui->pwrTab->item(i, 8)->text());
    }

    /*保存PG*/
    if (ui->pgTab->rowCount() == 0) {
        return;
    }

    Copy_PWR_List.replace(155 + Old_Id - 1, QString::number(ui->PG_volt->value()));
    Copy_PWR_List.replace(161, QString::number(ui->pg_count->value()));
    Copy_PWR_List.replace(5 + Old_Id - 1, QString::number(ui->checkBox_pg->checkState()));
    for (int  i = 0; i < 2; i++) {
        Copy_PWR_List.replace(165+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 1)->text());
        Copy_PWR_List.replace(166+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 2)->text());
        Copy_PWR_List.replace(167+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 3)->text());
        Copy_PWR_List.replace(168+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 4)->text());
        Copy_PWR_List.replace(169+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 5)->text());
        Copy_PWR_List.replace(170+(Old_Id-1)*30+i*15, ui->pgTab->item(i, 6)->text());
    }
}

/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2016.11.5
  * brief:      Conf 配置 PWR 界面
******************************************************************************/
void  Widget_PWR::Pri_PWR_Test_Parameter(QString str)
{
    int i;
    int PWR_Test_All_number;
    int PG_Test_All_Order;
    str.clear();
    PWR_Test_Data.clear();
    PG_Test_Order.clear();
    QStringList Number_data;  Number_data.clear();
    PWR_Test_All_number = 0;
    PG_Test_All_Order = 0;
    for (i = 0; i < PWM_Row_Count; i++) {
        if (Copy_PWR_List.at(35 + i*20).toInt() != 0) {
            PWR_Test_All_number++;
            Number_data.append(QString::number(i*10+Copy_PWR_List.at(35+i*20).toInt()));
            //  檔位
            if (Copy_PWR_List.at(5+i).toInt() == 2) {
                PG_Test_All_Order++;
                PG_Test_Order.append(QString::number(PG_Test_All_Order));
            } else {
                PG_Test_Order.append("0");
            }
        } else {
            //
        }
    }
    PWR_Test_Data << QString::number(PWR_Test_All_number) << Number_data;
}

void  Widget_PWR::Pri_PWR_Test_start(QString str)
{
    str.clear();
}

/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2016.11.5
  * brief:      Conf 配置 PWR 界面
******************************************************************************/
void Widget_PWR::Pub_Conf_Set_PWR(QString str, int flag)
{
    switch (flag) {
    case 1:
        Pri_PWR_Init(str);
        break;
    case 2:
        Pri_PWR_Data_Save();
        break;
    case 3:
        Pri_PWR_Test_Parameter(str);
        break;
    case 4:
        Pri_PWR_power_chose(str.toInt());
        break;
    case 6:
        Turn_Shan = str.toInt();
        if (Turn_Shan) {
            ui->pwrTab->setColumnWidth(1, 0);
            ui->pwrTab->setColumnWidth(2, 0);
            ui->pwrTab->setColumnWidth(3, 0);
            ui->pwrTab->setColumnWidth(4, 0);
            ui->pwrTab->setColumnWidth(5, 0);
            ui->pwrTab->setColumnWidth(6, 0);
            ui->pwrTab->setColumnWidth(7, 300);
            ui->checkBox_3->setDisabled(true);
            ui->checkBox_3->setChecked(false);
        }
        break;
    default:
        break;
    }
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2017.2.23
  * brief:      配置PWR电源选择
******************************************************************************/
void Widget_PWR::Pri_PWR_power_chose(int value)
{
    if (value == 0) {
        //
    } else if (value == 1) {
        ui->label_volt->show();  ui->PWR_Volt->show();
        ui->label_fre->show();   ui->PWR_Fre->show();
    } else if (value == 2) {
        ui->label_channel->show(); ui->PWR_channel->show();
    } else if (value == 3) {
        ui->label_volt->show();  ui->PWR_Volt->show();
    } else {
        //
    }
}

/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2017.2.10
  * brief:      配置PWR，设置PG
******************************************************************************/
QStringList Widget_PWR::Pub_Set_PG(can_frame frame)
{
    if (frame.data[0] == 1) { // 电流 低电平 高电平
        pg_Cur = ((double)(frame.data[1]*256+frame.data[2]))/100;
        pg_floor = ((double)(frame.data[3]*256+frame.data[4]))/100;
        pg_up = ((double)(frame.data[5]*256+frame.data[6]))/100;
    } else if (frame.data[0] == 2) { // 频率 min avr max
        pg_freq_min = ((double)(frame.data[1]*256+frame.data[2]))/10;
        pg_freq_max = ((double)(frame.data[3]*256+frame.data[4]))/10;
        pg_freq_avr = ((double)(frame.data[5]*256+frame.data[6]))/10;
    } else if (frame.data[0] == 3) { // 占空比 min avr max
        pg_duty_min = ((double)(frame.data[1]*256+frame.data[2]))/10;
        pg_duty_max = ((double)(frame.data[3]*256+frame.data[4]))/10;
        pg_duty_avr = ((double)(frame.data[5]*256+frame.data[6]))/10;
    } else {
        //
    }

    QStringList data;
    if (frame.data[0] == 3) {
        data.append(QString::number(pg_Cur, 'f', 2));      // 电流
        data.append(QString::number(pg_floor, 'f', 2));    // 低电平
        data.append(QString::number(pg_up, 'f', 2));       // 高电平
        data.append(QString::number(pg_freq_avr, 'f', 1)); // 频率
        data.append(QString::number(pg_duty_avr, 'f', 1)); // 占空比
        pg_speed = 60*pg_freq_avr/(ui->pg_count->value());
        data.append(QString::number(pg_speed, 'f', 0));    // 速度
    }
    return data;
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2017.2.10
  * brief:      配置PWR界面，转向是否可以选择
******************************************************************************/
void Widget_PWR::on_pwrTab_cellClicked(int row,  int column)
{
    // 苏恒 司密达 if ((column == 7)&&(row == 0))
    if (column == 0) {
        PWR_Labelhide->show();
        Widget_Button->show();
        PWR_Row = row;
    } else if (column == 7) { // India
        int resId = (Copy_PWR_List[44 + row*20].toInt() + 1)%pwrturn.size();
        if ((resId == 0) && (Turn_Shan)) {
            resId++;
        } else {
            //
        }
        Copy_PWR_List.replace(44+row*20, QString::number(resId));
        QTableWidgetItem *UnitItem = new QTableWidgetItem(pwrturn[resId]);
        UnitItem->setTextAlignment(Qt::AlignCenter);
        UnitItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        ui->pwrTab->setItem(row, 7, UnitItem);
        ui->pwrTab->setEditTriggers(QAbstractItemView::AllEditTriggers);
    } else {
        //
    }
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2016.11.5
  * brief:      配置PWR界面，PWR参数设定后进行参数规范设定
******************************************************************************/
void Widget_PWR::on_pwrTab_cellChanged(int row,  int column)
{
    QString str;
    int i, After_Point = 0;
    if (EPAR_Init_Flag != 0) {
        return;
    } else {
        //
    }
    EPAR_Init_Flag = 1;
    if (column != 7) {
        str = ui->pwrTab->item(row, column)->text();
        if (column == 0) {
            if (PWM_Model != QString("PG")) {
                EPAR_Init_Flag = 0;
                return;
            } else {
                //
            }
            if (str == QString(tr("---"))) {
                Pg_Button_Group->button(PWR_Row+1)->hide();
            } else {
                Pg_Button_Group->button(PWR_Row+1)->show();
                Pg_Button_Group->button(PWR_Row+1)->setText(str);
            }
            EPAR_Init_Flag = 0;
            return;
        }
        if (str == NULL) {
            ui->pwrTab->item(row, column)->setText("0");
        }
        for (i = 0; i < str.length(); i++) { //判断输入是" 0-9 . "
            if (((str[i] >= '0') && (str[i] <= '9')) || (str[i] == '.')) {
                //
            } else {
                ui->pwrTab->item(row, column)->setText("0");
                break;
            }
            if (str[i] == '.') { //  判断是否规范
                After_Point = str.length() - i - 1;
                if ((i == 0)) {
                    ui->pwrTab->item(row, column)->setText("0");
                }
                if (i == (str.length()-1)) {
                    ui->pwrTab->item(row, column)->setText\
                            (ui->pwrTab->item(row, column)->text().left(i));
                }
            }
        }
        if ((column == 1) || (column == 2)) {
            if (ui->pwrTab->item(row, column)->text().toDouble() > 5) {
                ui->pwrTab->item(row, column)->setText("5.00");
            }
            ui->pwrTab->item(row, column)->setText\
                    (QString::number(ui->pwrTab->item(row, column)->text().toDouble(), 'f', 2));
        }
        if ((column == 3) || (column == 4)) {
            if (ui->pwrTab->item(row, column)->text().toDouble() > 3000) {
                ui->pwrTab->item(row, column)->setText("3000");
            }
            ui->pwrTab->item(row, column)->setText\
                    (QString::number(ui->pwrTab->item(row, column)->text().toDouble(), 'f', 1));
        }
        if ((column == 5) || (column == 6)) {
            if (ui->pwrTab->item(row, column)->text().toDouble() > 600) {
                ui->pwrTab->item(row, column)->setText("600");
            }
            ui->pwrTab->item(row, column)->setText\
                    (QString::number(ui->pwrTab->item(row, column)->text().toDouble(), 'f', 0));
        }
        if (column == 8) {
            if (ui->pwrTab->item(row, column)->text().toDouble() > 1000)  {
                ui->pwrTab->item(row, column)->setText("999.9");
            } else if (ui->pwrTab->item(row, column)->text().toDouble() < 0.5) {
                ui->pwrTab->item(row, column)->setText("0.5");
            }
            ui->pwrTab->item(row, column)->setText\
                    (QString::number(ui->pwrTab->item(row, column)->text().toDouble(), 'f', 1));
        }

        if ((column == 1) || (column == 2)) {
            if ((ui->pwrTab->item(row, 1)->text().toDouble()) > \
                    (ui->pwrTab->item(row, 2)->text().toDouble())) {
                ui->pwrTab->item(row, 1)->setText("0");
            }
        }
        if ((column == 3) || (column == 4)) {
            if ((ui->pwrTab->item(row, 3)->text().toDouble()) > \
                    (ui->pwrTab->item(row, 4)->text().toDouble())) {
                ui->pwrTab->item(row, 3)->setText("0");
            }
        }
        if ((column == 5) || (column == 6)) {
            if ((ui->pwrTab->item(row, 5)->text().toDouble()) > \
                    (ui->pwrTab->item(row, 6)->text().toDouble())) {
                ui->pwrTab->item(row, 5)->setText("0");
            }
        }
    }
    EPAR_Init_Flag = 0;
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2016.12.10
  * brief:      PWR界面，电压上限输入完成
******************************************************************************/
void Widget_PWR::on_PWR_Volt_up_editingFinished()
{
    if ((ui->PWR_Volt_down->text().toDouble()) >= \
            (ui->PWR_Volt_up->text().toDouble())) {
        ui->PWR_Volt_down->setValue(0);
    }
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2016.12.10
  * brief:      PWR界面，电压下限输入完成
******************************************************************************/
void Widget_PWR::on_PWR_Volt_down_editingFinished()
{
    if ((ui->PWR_Volt_down->text().toDouble()) >= \
            (ui->PWR_Volt_up->text().toDouble())) {
        ui->PWR_Volt_down->setValue(0);
    }
}
/******************************************************************************
  * version:    1.0
  * author:     sl
  * date:       2017.2.5
  * brief:      配置PWR界面，PG参数设定后进行参数规范设定
******************************************************************************/
void Widget_PWR::on_pgTab_cellChanged(int row,  int column)
{
    QString str;
    int i, After_Point = 0;
    if (EPAR_Init_Flag == 0) {
        EPAR_Init_Flag = 1;
        str = ui->pgTab->item(row, column)->text();
        if (str == NULL) {
            ui->pgTab->item(row, column)->setText("0");
        }
        for (i = 0; i < str.length(); i++) { // 判断输入是" 0-9 . "
            if (((str[i] >= '0') && (str[i] <= '9')) || (str[i] == '.')) {
                //
            } else {
                ui->pgTab->item(row, column)->setText("0");
                break;
            }
            if (str[i] == '.') { //  判断是否规范
                After_Point = str.length() - i - 1;
                if ((i == 0)) {
                    ui->pgTab->item(row, column)->setText("0");
                }
                if (i == (str.length() - 1)) {
                    ui->pgTab->item(row, column)->setText\
                            (ui->pgTab->item(row, column)->text().left(i));
                }
            }
        }

        if (column == 1) { // 高电平
            if (ui->pgTab->item(row, column)->text().toDouble() > 15) {
                ui->pgTab->item(row, column)->setText("15.00");
            }
            ui->pgTab->item(row, column)->setText\
                    (QString::number(ui->pgTab->item(row, column)->text().toDouble(), 'f', 2));
        }
        if (column == 2) { // 低电平
            if (ui->pgTab->item(row, column)->text().toDouble() > 15) {
                ui->pgTab->item(row, column)->setText("15.00");
            }
            ui->pgTab->item(row, column)->setText\
                    (QString::number(ui->pgTab->item(row, column)->text().toDouble(), 'f', 2));
        }
        if (column == 3) { //频率
            if (ui->pgTab->item(row, column)->text().toDouble() < 20) {
                ui->pgTab->item(row, column)->setText("20.0");
            } else if (ui->pgTab->item(row, column)->text().toDouble() > 500) {
                ui->pgTab->item(row, column)->setText("500.0");
            }
            ui->pgTab->item(row, column)->setText\
                    (QString::number(ui->pgTab->item(row, column)->text().toDouble(), 'f', 1));
        }
        if (column == 4) { //占空比
            if (ui->pgTab->item(row, column)->text().toDouble() < 20) {
                ui->pgTab->item(row, column)->setText("20.0");
            } else if (ui->pgTab->item(row, column)->text().toDouble() > 80) {
                ui->pgTab->item(row, column)->setText("80.0");
            }
            ui->pgTab->item(row, column)->setText\
                    (QString::number(ui->pgTab->item(row, column)->text().toDouble(), 'f', 1));
        }
        if (column == 5) { //电流
            ui->pgTab->item(row, column)->setText\
                    (QString::number(ui->pgTab->item(row, column)->text().toDouble(), 'f', 2));
        }
        if (column == 6) { //转速
            ui->pgTab->item(row, column)->setText\
                    (QString::number(ui->pgTab->item(row, column)->text().toDouble(), 'f', 0));
        }

        if (column == 1) { // 高电平
            if ((ui->pgTab->item(0, 1)->text().toDouble()) < \
                    (ui->pgTab->item(1, 1)->text().toDouble())) {
                ui->pgTab->item(1, 1)->setText("0.00");
            }
        }
        if (column == 2) { // 低电平
            if ((ui->pgTab->item(0, 2)->text().toDouble()) < \
                    (ui->pgTab->item(1, 2)->text().toDouble())) {
                ui->pgTab->item(1, 2)->setText("0.00");
            }
        }
        if (column == 3) { // 频率
            if ((ui->pgTab->item(0, 3)->text().toDouble()) < \
                    (ui->pgTab->item(1, 3)->text().toDouble())) {
                ui->pgTab->item(1, 3)->setText("20.0");
            }
        }
        if (column == 4) { // 占空比
            if ((ui->pgTab->item(0, 4)->text().toDouble()) < \
                    (ui->pgTab->item(1, 4)->text().toDouble())) {
                ui->pgTab->item(1, 4)->setText("20.0");
            }
        }
        if (column == 5) {  // 电流
            if ((ui->pgTab->item(0, 5)->text().toDouble()) < \
                    (ui->pgTab->item(1, 5)->text().toDouble())) {
                ui->pgTab->item(1, 5)->setText("0.00");
            }
        }
        if (column == 6) { //电流
            if ((ui->pgTab->item(0, 6)->text().toDouble()) < \
                    (ui->pgTab->item(1, 6)->text().toDouble())) {
                ui->pgTab->item(1, 6)->setText("0.00");
            }
        }
        EPAR_Init_Flag = 0;
    }
}

QStringList Widget_PWR::PWR_Test_Param()
{
    QString str;
    QStringList  strSql;
    QStringList strParam, strTest, Back_List;;
    strTest.clear();
    strParam.clear();
    Back_List << "0" << "0";
    QStringList direction; direction << QString(tr("正转")) << QString(tr("反转"));
    QStringList turn;
    turn << QString(tr("不转")) << QString(tr("反转")) << QString(tr("正转"));
    str.clear();
    if (Copy_PWR_List.at(0).toInt() != 2) { //  电参
        return Back_List;
    }

    for (int j = 0; j < 6; j++) {
        if (Copy_PWR_List.at(35+j*20).toInt() != 0) {
            if (!Turn_Shan) {
                str += QString(tr("电参")) + \
                        Button_Text.at(Copy_PWR_List.at(35 + j*20).toInt()-1); // 电参
                strTest.append(str);   str.clear();
                str += "I:"+Copy_PWR_List.at(39 + j*20) + "A";
                str += " P:"+Copy_PWR_List.at(41 + j*20) + "W";
                str += " CU:"+Copy_PWR_List.at(43 + j*20) + "V";
                strParam.append(str);  str.clear();
                strSql.append(tr("电参"));
            }

            if (Copy_PWR_List.at(44 + j*20).toInt() != 0) { // 转向
                str += QString(tr("转向"));
                strTest.append(str);   str.clear();
                str = turn.at(Copy_PWR_List.at(44+j*20).toInt());
                strParam.append(str);  str.clear();
                strSql.append(tr("转向"));
            }
            if ((PWM_Model == QString("PG")) && \
                    (Copy_PWR_List.at(5+j).toInt() == 2)) { // PG--测试数据
                str += QString(tr("PG"))+Button_Text.at(Copy_PWR_List.at(35+j*20).toInt()-1);
                strTest.append(str);   str.clear();
                str += Copy_PWR_List.at(166+j*15)+"V,"; // 165
                str += Copy_PWR_List.at(165+j*15)+"V,"; // 166
                str += Copy_PWR_List.at(167+j*15)+"Hz,"; // 167   ,
                str += Copy_PWR_List.at(168+j*15)+"%,"; // 168
                str += Copy_PWR_List.at(169+j*15)+"mA,"; // 169
                str += Copy_PWR_List.at(170+j*15)+"rpm"; // 170
                strParam.append(str);  str.clear();
                strSql.append(tr("PG"));
            }
        }
    }

    Back_List.clear();
    Back_List.append(QString::number(strTest.size()));
    Back_List.append(QString::number(strParam.size()));
    Back_List.append(QString::number(strSql.size()));
    Back_List.append(strTest);
    Back_List.append(strParam);
    Back_List.append(strSql);
    return Back_List;
}

void Widget_PWR::on_toolButton_4_clicked()
{
    Pwr_Usart_Count++;
    if (Pwr_Usart_Count >= 10) {
        Pwr_Usart_Count = 0;
        frame.can_id  = 0x27;  frame.can_dlc = 0x01; // 串口
        frame.data[0] = 0x0e; // 堵转
        canSend(frame);
    }
}
