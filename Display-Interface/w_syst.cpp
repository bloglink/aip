#include "w_syst.h"
#include "ui_w_syst.h"


/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2015.11.11
  * brief:      显示时间和温度
******************************************************************************/
w_Syst::w_Syst(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::w_Syst)
{
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint); // -去掉标题栏
    ui->setupUi(this);

    Syst_Group = new QButtonGroup; // -配置按钮
    Syst_Group->addButton(ui->Key_A, Qt::Key_A);
    Syst_Group->addButton(ui->Key_H, Qt::Key_H);
    connect(Syst_Group, SIGNAL(buttonClicked(int)), this, SLOT(ButtonJudge(int)));

    dateList.append(ui->spinBox_1);
    dateList.append(ui->spinBox_2);
    dateList.append(ui->spinBox_3);
    dateList.append(ui->spinBox_4);
    dateList.append(ui->spinBox_5);
    dateList.append(ui->spinBox_6);
    ui->spinBox_6->hide();

    QPalette palette = this->palette(); // -设置背景颜色
    palette.setBrush(this->backgroundRole(), QBrush(QColor(133, 189, 229, 255), Qt::SolidPattern));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    net_Widget = new QWidget(this);
    net_Widget->setGeometry(150, 180, 500, 250);
    net_Widget->setWindowFlags(Qt::WindowStaysOnTopHint);
    net_Widget->setStyleSheet
            ("border-radius: 10px;padding:2px 4px;background-color: gray;"\
             "color: black;border-color: black;");
    QGridLayout *upside = new QGridLayout;
    QString net_table[3] = {"ip:", QString(tr("网关:")), "DNS:"};
    int i;
    for (i = 0; i < 3; i++) {
        PushHist_lable.append(new QLabel(this));
        PushHist_lable[i]->setText(net_table[i]);
        PushHist_lable[i]->setMaximumHeight(35); PushHist_lable[i]->setMaximumWidth(135);
        PushHist_lable[i]->setAlignment(Qt::AlignCenter);
        upside->addWidget(PushHist_lable[i], i, 0);
        if (i < 3) {
            line_Text.append(new QLineEdit(this));
            line_Text[i]->setMaximumHeight(35);      line_Text[i]->setMaximumWidth(255);
            upside->addWidget(line_Text[i], i, 1);
        }
    }
    QPushButton *button_quit = new QPushButton;
    button_quit->setText(tr("退出"));
    button_quit->setMinimumHeight(50);
    button_quit->setMinimumWidth(90);
    button_quit->setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"\
                               "stop: 0 #5B5F5F, stop: 0.5 #0C2436,stop: 1 #27405A);");
    upside->addWidget(button_quit, 0, 5);
    connect(button_quit, SIGNAL(clicked()), this, SLOT(net_Widget_autoquit()));
    QPushButton *button_set_net = new QPushButton;
    button_set_net->setText(tr("设置网络"));
    button_set_net->setMinimumHeight(50);
    button_set_net->setMinimumWidth(90);
    button_set_net->setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"\
                                  "stop: 0 #5B5F5F, stop: 0.5 #0C2436,stop: 1 #27405A);");
    upside->addWidget(button_set_net, 2, 5);
    connect(button_set_net, SIGNAL(clicked()), this, SLOT(net_Widget_set_net()));
    QVBoxLayout *Histogram_Widget_layout = new QVBoxLayout;
    Histogram_Widget_layout->addLayout(upside);
    net_Widget->setLayout(Histogram_Widget_layout);
    net_Widget->hide();



    IO_Widget = new QWidget(this);
    IO_Widget->setGeometry(150, 100, 500, 410);
    IO_Widget->setWindowFlags(Qt::WindowStaysOnTopHint);
    IO_Widget->setStyleSheet\
            ("border-radius: 10px;padding:2px 4px;background-color: gray;"\
             "color: black;border-color: black;");
    QGridLayout *IO_upside = new QGridLayout;
    QString IO_table[7] = {QString(tr("延时时间(Ms):")), QString(tr("线体:")), \
                           QString(tr("工位:")), QString(tr("RFID读取:")), \
                           QString(tr("RFID设置:")), QString(tr("BOSCH:")), QString(tr("防呆:"))};
    for (i = 0; i < 7; i++) {
        IO_Show_Lable.append(new QLabel(this));
        IO_Show_Lable[i]->setText(IO_table[i]);
        IO_Show_Lable[i]->setMaximumHeight(35);
        IO_Show_Lable[i]->setMaximumWidth(135);
        IO_Show_Lable[i]->setAlignment(Qt::AlignCenter);
        IO_upside->addWidget(IO_Show_Lable[i], i, 0);
        if (i < 5) {
            IO_Line_Text.append(new QLineEdit(this));
            IO_Line_Text[i]->setMaximumHeight(35);
            IO_Line_Text[i]->setMaximumWidth(95);
            IO_upside->addWidget(IO_Line_Text[i], i, 1);
        }
    }
    IO_Line_Text[3]->setDisabled(true);
    IO_Line_Text[4]->setValidator(new QIntValidator(1, 9, this));
    IO_Line_Text[4]->setText("1");

    stagnate = new QCheckBox;
    stagnate->setText(tr("取消"));
    stagnate->setStyleSheet\
            ("QCheckBox::indicator {image: url(:/image/053.png);width: 55px;"\
             "height: 45px;}QCheckBox::indicator:checked { image: url(:/image/051.png); }"\
             "QCheckBox  { color: rgb(0, 0, 0);}");
    IO_upside->addWidget(stagnate, 6, 1);

    QPushButton *IO_button_Left = new QPushButton;
    IO_button_Left->setText(tr("左工位"));
    IO_button_Left->setMinimumHeight(50);
    IO_button_Left->setMinimumWidth(90);
    IO_button_Left->setStyleSheet\
            ("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"\
             "stop: 0 #5B5F5F, stop: 0.5 #0C2436,stop: 1 #27405A);");
    IO_upside->addWidget(IO_button_Left, 5, 1);
    connect(IO_button_Left, SIGNAL(clicked()), this, SLOT(Left_Widget_autoquit()));

    QPushButton *IO_button_Right = new QPushButton;
    IO_button_Right->setText(tr("右工位"));
    IO_button_Right->setMinimumHeight(50);
    IO_button_Right->setMinimumWidth(90);
    IO_button_Right->setStyleSheet\
            ("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"\
             "stop: 0 #5B5F5F, stop: 0.5 #0C2436,stop: 1 #27405A);");
    IO_upside->addWidget(IO_button_Right, 5, 2);
    connect(IO_button_Right, SIGNAL(clicked()), this, SLOT(Right_Widget_autoquit()));

    QPushButton *IO_button_quit = new QPushButton;
    IO_button_quit->setText(tr("退出"));
    IO_button_quit->setMinimumHeight(50);
    IO_button_quit->setMinimumWidth(90);
    IO_button_quit->setStyleSheet\
            ("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"\
             "stop: 0 #5B5F5F, stop: 0.5 #0C2436,stop: 1 #27405A);");
    IO_upside->addWidget(IO_button_quit, 0, 5);
    connect(IO_button_quit, SIGNAL(clicked()), this, SLOT(IO_Widget_autoquit()));

    QPushButton *RFID_Read = new QPushButton;
    RFID_Read->setText(tr("读取"));
    RFID_Read->setMinimumHeight(50);
    RFID_Read->setMinimumWidth(90);
    RFID_Read->setStyleSheet\
            ("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"\
             "stop: 0 #5B5F5F, stop: 0.5 #0C2436,stop: 1 #27405A);");
    IO_upside->addWidget(RFID_Read, 3, 5);
    connect(RFID_Read, SIGNAL(clicked()), this, SLOT(IO_RFID_Read()));

    QPushButton *RFID_Write = new QPushButton;
    RFID_Write->setText(tr("设置"));
    RFID_Write->setMinimumHeight(50);
    RFID_Write->setMinimumWidth(90);
    RFID_Write->setStyleSheet\
            ("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"\
             "stop: 0 #5B5F5F, stop: 0.5 #0C2436,stop: 1 #27405A);");
    IO_upside->addWidget(RFID_Write, 4, 5);
    connect(RFID_Write, SIGNAL(clicked()), this, SLOT(IO_RFID_Write()));

    QVBoxLayout *Histogram_IO_Widget_layout = new QVBoxLayout;
    Histogram_IO_Widget_layout->addLayout(IO_upside);
    IO_Widget->setLayout(Histogram_IO_Widget_layout);
    IO_Widget->hide();

    SQL_Widget = new QWidget(this);
    SQL_Widget->setGeometry(150, 100, 500, 400);
    SQL_Widget->setWindowFlags(Qt::WindowStaysOnTopHint);
    SQL_Widget->setStyleSheet\
            ("border-radius: 10px;padding:2px 4px;background-color: gray;"\
             "color: black;border-color: black;");
    QGridLayout *SQL_upside = new QGridLayout;
    QString SQL_table[6] = {"SQL-ip:", "SQL-User:", \
                            "SQL-Pass:", "SQL-Database:", \
                            "SQL-Port:", QString(tr("任务表"))};
    for (i = 0; i < 6; i++) {
        SQL_lable.append(new QLabel(this));
        SQL_lable[i]->setText(SQL_table[i]);
        SQL_lable[i]->setMaximumHeight(35); SQL_lable[i]->setMaximumWidth(135);
        SQL_lable[i]->setAlignment(Qt::AlignCenter);
        SQL_upside->addWidget(SQL_lable[i], i, 0);
        if (i < 6) {
            line_Text.append(new QLineEdit(this));
            line_Text[3+i]->setMaximumHeight(35);
            line_Text[3+i]->setMaximumWidth(255);
            SQL_upside->addWidget(line_Text[3+i], i, 1);
        }
    }
    line_Text[5]->setEchoMode(QLineEdit::Password);
    QPushButton *button_quit_SQL = new QPushButton;
    button_quit_SQL->setText(tr("退出"));
    button_quit_SQL->setMinimumHeight(50);
    button_quit_SQL->setMinimumWidth(90);
    button_quit_SQL->setStyleSheet
            ("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: "\
             "0 #5B5F5F, stop: 0.5 #0C2436,stop: 1 #27405A);");
    SQL_upside->addWidget(button_quit_SQL, 0, 5);
    connect(button_quit_SQL, SIGNAL(clicked()), this, SLOT(SQL_Widget_autoquit()));
    net_sql_enable = new QCheckBox;
    net_sql_enable->setText(tr("启用   "));
    net_sql_enable->setStyleSheet
            ("QCheckBox::indicator {image: url(:/image/053.png);width: 55px;"\
             "height: 45px;}QCheckBox::indicator:checked { image: url(:/image/051.png); }"\
             "QCheckBox  { color: rgb(0, 0, 0);}");
    SQL_upside->addWidget(net_sql_enable, 4, 5);
    System.SaveNG = false;
    net_sql_NG = new QCheckBox;
    net_sql_NG->setText(tr("不保存-NG"));
    net_sql_NG->setStyleSheet
            ("QCheckBox::indicator {image: url(:/image/053.png);width: 55px;"\
             "height: 45px;}QCheckBox::indicator:checked { image: url(:/image/051.png); }"\
             "QCheckBox  { color: rgb(0, 0, 0);}");
    SQL_upside->addWidget(net_sql_NG, 3, 5);
    QPushButton *button_set_sql = new QPushButton;
    button_set_sql->setText(tr("设置数据库"));
    button_set_sql->setMinimumHeight(50);
    button_set_sql->setMinimumWidth(90);
    button_set_sql->setStyleSheet
            ("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop:"
             "0 #5B5F5F, stop: 0.5 #0C2436,stop: 1 #27405A);");
    SQL_upside->addWidget(button_set_sql, 5, 5);
    connect(button_set_sql, SIGNAL(clicked()), this, SLOT(net_Widget_set_SQL()));
    QVBoxLayout *Histogram_Widget_layout_SQL = new QVBoxLayout;
    Histogram_Widget_layout_SQL->addLayout(SQL_upside);
    SQL_Widget->setLayout(Histogram_Widget_layout_SQL);
    SQL_Widget->hide();

    ui->comb_power->setDisabled(true);

    QStringList System_list; //  默认 System/Text
    System_list.clear();
    System_list.append("9");   // -蜂鸣器的档位
    System_list.append("0.1"); // -合格时间
    System_list.append("0.3"); // -不合格时间
    System_list.append("9");   // -Led档位
    System_list.append("0");   // -启动模式
    System_list.append("1");   // -管理员

    QStringList Allocation_list; // -默认 Allocation/Item
    Allocation_list.clear();
    Allocation_list.append("13");
    Allocation_list.append("14");
    Allocation_list.append("0");
    Allocation_list.append("0");
    Allocation_list.append("0");
    Allocation_list.append("0");
    Allocation_list.append("1");

    int SQLData_number;
    QStringList SQLData_list;
    System_power      = "none";

    QSettings *set_Sys = new QSettings(Sys_path, QSettings::IniFormat);
    set_Sys->setIniCodec("GB18030");

    System.Sum        = set_Sys->value("System/Text", System_list).toStringList();
    System.SQL_enable = set_Sys->value("Sql/Text", "disable").toString();
    System.SaveNG     = set_Sys->value("Sql/SaveNG", "0").toInt();
    SQLData_number    = set_Sys->value("Allocation/All", "7").toString().toInt();
    SQLData_list      = set_Sys->value("Allocation/Item", Allocation_list).\
            toStringList().mid(5, SQLData_number); // -数据型号

    if (SQLData_list.contains("7") || \
            SQLData_list.contains("9") || \
            SQLData_list.contains("10")) { // -整机操作
        System_power = "All";
        ui->comb_power->setEnabled(true);
        ui->comb_power->setCurrentIndex(set_Sys->value("power/Text", "0")\
                                        .toString().toInt()); //  读取电源选择
    }
    if (System.SQL_enable == "enable") { // -读取数据库使能
        net_sql_enable->setChecked(true);
    }
    if (System.SaveNG) { // -读取数据库使能
        net_sql_NG->setChecked(true);
    }
    line_Text.at(8)->setText(set_Sys->value("Sql/Task_Table", "").toString());
    if (set_Sys->value("language/Text").toString() == "English") {  // -读取语言选择设置
        ui->comb_language->setCurrentIndex(1);
    }
    if (set_Sys->value("language/enabled").toString() == "Enable") { // -使能语言选择
        ui->comb_language->setEnabled(true);
    }
    ui->comb_imp_back->setDisabled(true);
    if (set_Sys->value("impvolt/text", QString(tr("back"))).toString() == \
            QString(tr("back"))) {
        ui->comb_imp_back->setCurrentIndex(0);
    } else {
        ui->comb_imp_back->setCurrentIndex(1);
    }

    ui->comb_Beep->setCurrentIndex(System.Sum[0].toInt());
    ui->Beep_OK->setValue(System.Sum[1].toDouble());
    ui->Beep_NG->setValue(System.Sum[2].toDouble());
    ui->comb_Led->setCurrentIndex(System.Sum[3].toInt());
    ui->comb_Start->setCurrentIndex(System.Sum[4].toInt());
    ui->comb_Conf->setCurrentIndex(System.Sum[5].toInt());
    ui->scan_start->setValue(set_Sys->value("Barcode/start").toString().toInt());
    ui->scan_stop->setValue(set_Sys->value("Barcode/stop").toString().toInt());
    on_Led_Button_clicked();
    system("sync");
    delete set_Sys;

    L_count = 0;
    R_count = 0;
}

w_Syst::~w_Syst()
{
    delete Syst_Group;
    delete ui;
}

void w_Syst::update_system()
{
    System.Sum.clear();
    System.Sum.append(QString::number(ui->comb_Beep->currentIndex())); // -蜂鸣器时间
    System.Sum.append(ui->Beep_OK->text());
    System.Sum.append(ui->Beep_NG->text());
    System.Sum.append(QString::number(ui->comb_Led->currentIndex()));  // -液晶屏亮度
    System.Sum.append(QString::number(ui->comb_Start->currentIndex())); // -启动方式
    System.Sum.append(QString::number(ui->comb_Conf->currentIndex())); // -管理员和操作员
    System.Sum.append(ui->comb_Start->currentText()); // -启动方式
    System.Sum.append(ui->comb_Conf->currentText());  // -用户
    System.Sum.append(ui->scan_start->text());        // -条码扫描起始
    System.Sum.append(ui->scan_stop->text());         // -条码扫描终止
}
void w_Syst::ButtonJudge(int id)
{
    qDebug() << "w_Syst::ButtonJudge" << id;
    switch (id) {
    case Qt::Key_A: {
        update_system();
        signal_to_main(QStringList(""), wMain_Surface, 3);
        signal_to_main(System.Sum, 2, 2);

        QString currentDateTime = QDateTime::currentDateTime()\
                .toString("yyyy-MM-dd---hh-mm-ss"); // -设置显示格式
        QSettings set_Test_File(Test_Day_Record_path,  QSettings::IniFormat);
        set_Test_File.setIniCodec(QTextCodec::codecForName("GB2312"));
        QDir *temp = new QDir;   bool exist = temp->exists(Test_Day_Record_path);
        if (exist) {
            //
        } else {
            system("mkdir /mnt/nandflash/AIP/dayrecord/");
            system("touch /mnt/nandflash/AIP/dayrecord/dayrecord.ini");
            system("chmod +x /mnt/nandflash/AIP/dayrecord/dayrecord.ini");
        }
        QString keyname;
        keyname = QString("Data/%1").arg(currentDateTime);
        set_Test_File.setValue(keyname, System.Sum.at(6)+System.Sum.at(7));

        QSettings *set_Sys = new QSettings(Sys_path, QSettings::IniFormat);
        set_Sys->setIniCodec("GB18030");
        set_Sys->setValue("Barcode/start", QString::number(ui->scan_start->value()));
        set_Sys->setValue("Barcode/stop", QString::number(ui->scan_stop->value()));

        if (ui->comb_language->currentIndex() == 0) {
            set_Sys->setValue("language/Text", "Chinese");
        } else {
            set_Sys->setValue("language/Text", "English");
        }

        if (System_power == "All") { //  整机
            set_Sys->setValue("power/Text", QString::number(ui->comb_power->currentIndex()));
        } else {
            //  定子
        }

        if (ui->comb_imp_back->currentIndex() == 0) {
            set_Sys->setValue("impvolt/text", QString(tr("back")));
        } else {
            set_Sys->setValue("impvolt/text", QString(tr("noback")));
        }

        system("sync");
        delete set_Sys;
        break;
    }
    default:
        //
        break;
    }
}

void w_Syst::on_Key_H_clicked()
{
    signal_to_main(QStringList(""), wPassWord_Surface, 3);
}

void w_Syst::on_Timer_clicked()
{
    LINUX_RTC rtc;
    rtc_time t;

    if (!dateList.isEmpty()) {
        t.tm_year = dateList[0]->value();
        t.tm_mon  = dateList[1]->value();
        t.tm_mday = dateList[2]->value();
        t.tm_hour = dateList[3]->value();
        t.tm_min  = dateList[4]->value();
        t.tm_sec  = dateList[5]->value();
    }
    rtc.set_rtc(t);
}

void w_Syst::gettime()
{
    LINUX_RTC rtc;
    rtc_time t;
    t = rtc.get_rtc();
    dateList[0]->setValue(t.tm_year);
    dateList[1]->setValue(t.tm_mon);
    dateList[2]->setValue(t.tm_mday);
    dateList[3]->setValue(t.tm_hour);
    dateList[4]->setValue(t.tm_min);
}

void w_Syst::on_Beep_Button_clicked()
{
    if (ui->Beep_Button->text() == QString(tr("停止"))) {
        ui->Beep_Button->setText(tr("音量 设定查看"));
        Beep_PWM_Stop();
    } else if (ui->Beep_Button->text() == QString(tr("音量 设定查看"))) {
        ui->Beep_Button->setText(tr("停止"));
        Beep_PWM_Start(ui->comb_Beep->currentText().toInt()*10-1);
    } else {
        //
    }
}

void w_Syst::on_Led_Button_clicked()
{
    Led_PWM_Start(ui->comb_Led->currentText().toInt()-1);
}

void w_Syst::on_Button_Check_clicked()
{
    signal_to_main(QStringList(""), 1, 1);
}

void w_Syst::slot_from_main(QString str, int value)
{
    switch (value) {
    case 1:
        check_finsh(str);
        break;
    case 2:
        gettime();
        break;
    case 3:
        ui->comb_imp_back->setEnabled(true);
        break;
    case 4:
        IO_Line_Text[3]->setText(str);
        break;
    default:
        break;
    }
}

void w_Syst::check_finsh(QString Waring_Text)
{
    QMessageBox *message = new QMessageBox(this);
    message->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    message->setStyleSheet("QMessageBox{border: gray;border-radius: 10px;"\
                           "padding:2px 4px;background-color: gray;height:390px;width:375px;}");
    message->setText(Waring_Text+"\n");
    message->addButton(QMessageBox::Ok)->setStyleSheet
            ("height:39px;width:75px;border:5px groove;"\
             "border:none;border-radius:10px;padding:2px 4px;");
    message->setButtonText(QMessageBox::Ok, QString(tr("确 定")));
    message->setIcon(QMessageBox::Warning);
    message->exec();
}

void w_Syst::on_Button_Check_2_clicked()
{
    system("./ts_calibrate -qws&");
}

void w_Syst::net_Widget_set_net()
{
    char copystr[256];
    char *whom;
    if (line_Text.at(0)->text() != "") {
        whom = QString(line_Text.at(0)->text()).toUtf8().data();
        sprintf(copystr, "ifconfig eth0 %s netmask 255.255.255.0", whom);
        system(copystr);
    }
    if (line_Text.at(1)->text() != "") {
        whom = QString(line_Text.at(1)->text()).toUtf8().data();
        sprintf(copystr, "echo servername %s >>/etc/resolv.conf", whom);
        system(copystr);
    }
    if (line_Text.at(2)->text() != "") {
        whom = QString(line_Text.at(2)->text()).toUtf8().data();
        sprintf(copystr, "route add default gw %s", whom);
        system(copystr);
    }
}

void w_Syst::net_Widget_set_SQL() //  /usr/local/arm/unixODBC/etc
{
    QSettings set_odbc("/usr/local/arm/unixODBC/etc/odbc.ini",  QSettings::IniFormat);
    if (line_Text.at(3)->text() != "") { //  Server
        set_odbc.setValue("testodbc/Server", line_Text.at(3)->text());
    }
    if (line_Text.at(4)->text() != "") { //  UserName
        set_odbc.setValue("testodbc/UserName", line_Text.at(4)->text());
    }
    if (line_Text.at(5)->text() != "") { //  Password
        set_odbc.setValue("testodbc/Password", line_Text.at(5)->text());
    }
    if (line_Text.at(6)->text() != "") { //  Database
        set_odbc.setValue("testodbc/Database", line_Text.at(6)->text());
    }
    if (line_Text.at(7)->text() != "") { //  Port
        set_odbc.setValue("testodbc/Port", line_Text.at(7)->text());
    }

    QSettings *set_Sys = new QSettings(Sys_path,  QSettings::IniFormat);
    if (net_sql_enable->checkState() == 2) {
        set_Sys->setValue("Sql/Text", "enable");
    } else {
        set_Sys->setValue("Sql/Text", "disable");
    }

    if (net_sql_NG->checkState() == 2) {
        set_Sys->setValue("Sql/SaveNG", "1");
    } else {
        set_Sys->setValue("Sql/SaveNG", "0");
    }

    if (line_Text.at(8)->text() != "") {
        set_Sys->setValue("Sql/Task_Table", line_Text.at(8)->text());
    }

    delete set_Sys;
    system("sync");
}

void w_Syst::Left_Widget_autoquit() {
    L_count++;
    if (L_count%2) {
        signal_to_main(QStringList(""), 2, 8);
    } else {
        signal_to_main(QStringList(""), 0, 8);
    }
}

void w_Syst::Right_Widget_autoquit() {
    R_count++;
    if (R_count%2) {
        signal_to_main(QStringList(""), 4, 8);
    } else {
        signal_to_main(QStringList(""), 0, 8);
    }
}

void w_Syst::IO_Widget_autoquit() {
    signal_to_main(QStringList(""), IO_Line_Text[0]->text().toInt(), 5);
    IO_Widget->hide();
}

void w_Syst::IO_RFID_Read() {
    signal_to_main(QStringList(""), 7, 7);
}

void w_Syst::IO_RFID_Write() {
    signal_to_main(QStringList(""), IO_Line_Text[4]->text().toInt(), 6);

    QSettings *set_Sys = new QSettings(Sys_path, QSettings::IniFormat);
    if (IO_Line_Text.at(4)->text()!="") {
        set_Sys->setValue("RFID/Code", IO_Line_Text.at(4)->text());
    }
    delete set_Sys;
    system("sync");
}
void w_Syst::net_Widget_autoquit()
{
    net_Widget->hide();
}

void w_Syst::SQL_Widget_autoquit()
{
    SQL_Widget->hide();
}

QString w_Syst::GetIP()
{
    int sock_get_ip;
    char ipaddr[50];
    struct  sockaddr_in *sin;
    struct  ifreq ifr_ip;
    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        //
    }
    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);
    if (ioctl(sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0) {
        //
    }
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
    strcpy(ipaddr, inet_ntoa(sin->sin_addr));
    return QString(ipaddr);
}

QString w_Syst::GetGateWay()
{
    FILE *fp;
    char buf[512];
    char cmd[128];
    char gateway[30];
    char *tmp;

    strcpy(cmd, "ip route");
    fp = popen(cmd, "r");
    if (NULL == fp) {
        perror("popen error");
        return "";
    }
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        tmp = buf;
        while (*tmp && isspace(*tmp))
            ++tmp;
        if (strncmp(tmp, "default", strlen("default")) == 0)
            break;
    }
    sscanf(buf, "%*s%*s%s", gateway);

    pclose(fp);

    return QString(gateway);
}

void w_Syst::on_Button_Check_3_clicked()
{
    line_Text.at(0)->setText(GetIP()); //  ip
    line_Text.at(1)->setText(GetGateWay()); //  网关
    //   line_Text.at(2)->setText(); //  DNS
    net_Widget->show();
}

void w_Syst::on_lineEdit_editingFinished()
{
    if ((ui->scan_start->text().toInt() == 0) && (ui->scan_stop->text().toInt() == 0)) {
        //
    } else if ((ui->scan_start->text().toInt() < ui->lineEdit->text().size()) && \
             (ui->scan_stop->text().toInt() < ui->lineEdit->text().size())) {
        ui->lineEdit->setText(ui->lineEdit->text().mid(ui->scan_start->text().toInt(), \
                       ui->scan_stop->text().toInt()-ui->scan_start->text().toInt()));
    } else if ((ui->scan_start->text().toInt() < ui->lineEdit->text().size()) && \
               (ui->scan_stop->text().toInt() > ui->lineEdit->text().size())) {
        ui->lineEdit->setText(ui->lineEdit->text().mid(ui->scan_start->text().toInt(), \
                        ui->lineEdit->text().size()-ui->scan_start->text().toInt()));
    } else if (ui->scan_start->text().toInt() >= ui->lineEdit->text().size()) {
        ui->lineEdit->setText("");
    } else {
        //
    }
}

void w_Syst::on_scan_start_editingFinished()
{
    if (ui->scan_start->text().toInt() > ui->scan_stop->text().toInt()) {
        ui->scan_start->setValue(0);
    }
}

void w_Syst::on_scan_stop_editingFinished()
{
    if (ui->scan_start->text().toInt() > ui->scan_stop->text().toInt()) {
        ui->scan_start->setValue(0);
    }
}

void w_Syst::on_Button_SQL_clicked()
{
    QSettings set_odbc("/usr/local/arm/unixODBC/etc/odbc.ini", QSettings::IniFormat);
    line_Text.at(3)->setText(set_odbc.value("testodbc/Server").toString()); //  SQL-IP
    line_Text.at(4)->setText(set_odbc.value("testodbc/UserName").toString()); //  SQL-User
    line_Text.at(5)->setText(set_odbc.value("testodbc/Password").toString()); //  SQL-Pass
    line_Text.at(6)->setText(set_odbc.value("testodbc/Database").toString()); //  SQL-Database
    line_Text.at(7)->setText(set_odbc.value("testodbc/Port").toString()); //  SQL-Port
    SQL_Widget->show();
}
void w_Syst::on_Button_IO_clicked()
{
    IO_Widget->show();
}

void w_Syst::on_Button_IO_2_clicked()
{
    QSettings *set_Sys = new QSettings("/mnt/nandflash/userinfo.txt", QSettings::IniFormat);
    set_Sys->setIniCodec("GB18030");
    set_Sys->setValue("LOCAL_MACHINE/DHCP", QString::number(1));

    system("sync");
    delete set_Sys;
}
