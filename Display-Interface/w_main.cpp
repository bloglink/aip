/*******************************************************************************
 * Copyright (c) 2015,青岛艾普智能仪器有限公司
 * All rights reserved.
 *
 * version:     2.0
 * author:      SL
 * date:        2015.12.10
 * brief:       主界面
*******************************************************************************/
#include "w_main.h"
#include "ui_w_main.h"
/******************************************************************************
  * version:    1.0
  * author:     link
  * date:       2015.12.10
  * brief:      主界面构造/初始化
******************************************************************************/
w_Main::w_Main(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::w_Main)
{
    this->setWindowFlags(Qt::FramelessWindowHint);  //去掉标题栏
    ui->setupUi(this);

    wSyst = NULL;
    wData = NULL;
    wTest = NULL;
    wHelp = NULL;
    wConf = NULL;
    wColr = NULL;
    wDebug    = NULL;
    wPassWord = NULL;

    ui->label_net->hide();     //-网络图标隐藏
    ui->label_print->hide();   //-打印机图标隐藏
    ui->label_sqldata->hide(); //-数据库图标隐藏
    ui->label_Version->hide(); //-版本信息标签隐藏
    ui->label_hand->hide();    //-握手信息图标隐藏

    step_system = 0;
    window_old  = 0;   window_current = 0;

    can_number   = 0;
    can_bug_time = 0;

    imp_back_number = 0; imp_waveform_error = true;
    mag_back_number = 0; mag_waveform_error = true;  mag_set_rotation = 0;
    waveform_count  = 0;
    pg_waveform_error = true;
 //   fg_waveform_error = true;

    IRACW.updata = 0;
    IRACW.time   = 0;
    IRACW.volt   = 0;
    IRACW.voltc  = 0;
    IRACW.result = 0;
    IRACW.resultc = 0;
    IRACW.judge  = 0;
    IRACW.resultc_dou  = 0;
    IRACW.show_resultc = 0;

    for (int i = 0; i < 800; i++) {
        waveform_point[i] = 0;
        waveform_IMP_Compare[i] = 0;
    }

    out_station = 0x13;
    isTesting = false;
    PreCount  = 0xff;

    this->Button_Main_Group = new QButtonGroup;     //-配置按钮
    this->Button_Main_Group->addButton(ui->Key_A, Qt::Key_A);  ui->Key_A->hide();
    this->Button_Main_Group->addButton(ui->Key_B, Qt::Key_B);  ui->Key_B->hide();
    this->Button_Main_Group->addButton(ui->Key_C, Qt::Key_C);  ui->Key_C->hide();
    this->Button_Main_Group->addButton(ui->Key_D, Qt::Key_D);  ui->Key_D->hide();
    this->Button_Main_Group->addButton(ui->Key_E, Qt::Key_E);
    connect(this->Button_Main_Group, SIGNAL(buttonClicked(int)), \
            this, SLOT(Button_Main_Group_Judge(int)));

    if (QString(Version_System) == QString("Release")) {  // -系统(Release)隐藏退出按键
        ui->Key_E->hide();
    } else {
        ui->Key_E->show();
    }

    this->p_ipmethod = new IPMethod; // 启用输入法
    QWSServer::setCurrentInputMethod(this->p_ipmethod);
    connect(this, SIGNAL(Serial_SendString(QString)), this->p_ipmethod, SLOT(Preedit(QString)));
    connect(this, SIGNAL(Serial_SendEnter()), this->p_ipmethod, SLOT(Confirm()));
    connect(this, SIGNAL(Serial_SendBackspace()), this->p_ipmethod, SLOT(Backspace()));
    connect(this, SIGNAL(Serial_SendEscape()), this->p_ipmethod, SLOT(Escape()));
    connect(this, SIGNAL(Serial_Upspace()), this->p_ipmethod, SLOT(Upspace()));
    connect(this, SIGNAL(Serial_Downspace()), this->p_ipmethod, SLOT(Downspace()));
    connect(this, SIGNAL(Serial_Tabspace()), this->p_ipmethod, SLOT(Tabspace()));
    connect(this, SIGNAL(Serial_BackTabspace()), this->p_ipmethod, SLOT(BackTabspace()));

    Timer_system = new QTimer(this);
    connect(Timer_system, SIGNAL(timeout()), this, SLOT(System_Runing()));
    Timer_system->start(50);

    out_board = new QTimer(this);
    connect(out_board, SIGNAL(timeout()), this, SLOT(Out_Board_Stop()));
    dcr_board = new QTimer(this);
    connect(dcr_board, SIGNAL(timeout()), this, SLOT(Dcr_Board_Stop()));

    acw_board = new QTimer(this);
    connect(acw_board, SIGNAL(timeout()), this, SLOT(Acw_Board_Stop()));
    Mouse.Time = new QTimer(this);
    connect(Mouse.Time, SIGNAL(timeout()), this, SLOT(Mouse_Judge()));
    Mouse.Ms = 0;  Mouse.Us = 0;
    Mouse.Flag = false;

    QPalette palette = ui->Print_Text->palette(); // 设置输出文本信息
    palette.setBrush(QPalette::Base, QBrush(Qt::NoBrush));
    ui->Print_Text->setPalette(palette);
    ui->Print_Text->setReadOnly(true);
    ui->Print_Text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //-设置垂直滚动条不可见
    ui->Print_Text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); //-设置水平滚动条不可见
    QTextCursor text_cursor(ui->Print_Text->textCursor());                //-设置光标的位置
    text_cursor.movePosition(QTextCursor::End);
    ui->Print_Text->setTextCursor(text_cursor);

    myThread = new MyThread; // 开启一个线程，判断U盘的操作
    MYSQL_DB = new SQL_DB; // 开启一个线程，判断U盘的操作
    MYSQL_DB->start();
    myThread->start();

    QStringList system_conf_list;   system_conf_list.clear();
    system_conf_list.append("9");   //-蜂鸣器的档位
    system_conf_list.append("0.1"); //-合格时间
    system_conf_list.append("0.3"); //-不合格时间
    system_conf_list.append("9");   // -Led档位
    system_conf_list.append("0");   //-启动模式
    system_conf_list.append("1");   //-管理员

    QStringList Allocation_list; //-默认 Allocation/Item
    Allocation_list.clear();
    Allocation_list.append("13");
    Allocation_list.append("14");
    Allocation_list.append("0");
    Allocation_list.append("0");
    Allocation_list.append("0");
    Allocation_list.append("0");
    Allocation_list.append("1");

    QStringList Password_list; //-默认 Password/Text
    Password_list.clear();
    Password_list.append("6");
    Password_list.append("456");
    Password_list.append("789");

    QSettings *set_Test_File = new QSettings(Sys_path, QSettings::IniFormat);
    set_Test_File->setIniCodec("GB18030");
    set_Test_File->setValue("Version/text", Version_number); //-通过程序将版本号进行写入

    barcode_start = set_Test_File->value("Barcode/start", "0").toInt(); //-获取截取的字符串的起始位
    barcode_stop  = set_Test_File->value("Barcode/stop", "0").toInt();  //-获取截取的字符串的结束位
    out_stamp     = set_Test_File->value("outsingal/text", "0").toInt();
    signal_model  = set_Test_File->value("DCR_In/text", "1").toInt();
    nine_board    = set_Test_File->value("BoardNumber/text", 0).toInt();
    set_gnd_vlot  = set_Test_File->value("System/GND", "20").toInt();
    acsw_board    = set_Test_File->value("System/ACSW", "0").toInt();
    udp_enable    = set_Test_File->value("UDP/text", "0").toInt();
    system_conf   = set_Test_File->value("System/Text", system_conf_list).toStringList();

    Ini_Number    = set_Test_File->value("Allocation/All", "7").toString().toInt();
    Ini_Proj_Real = set_Test_File->value("Allocation/Item", Allocation_list).toStringList();
    Ini_Proj      = set_Test_File->value("Allocation/Item", \
                                         Allocation_list).toStringList().mid(5, Ini_Number);
    Ini_Password  = set_Test_File->value("Password/Text", Password_list).toStringList();
    Ini_IRACW_True = set_Test_File->value("Allocation/Set", "2").toInt(); //-设置绝缘耐压显示数值
    Ini_WD        = set_Test_File->value("System/WD", "Common").toString(); // -设置测试WD板
    Ini_IO        = set_Test_File->value("System/IO", "0").toInt();         // -设置测试IO板
    Ini_LR_Beep   = set_Test_File->value("System/LR_Beep", "0").toInt();         // -设置测试IO板
    Ini_IO_Model  = set_Test_File->value("System/IO_Model", "0").toInt();
    Ini_Motor     = set_Test_File->value("System/PG", "Common").toString();
    Ini_MAG_Device   = set_Test_File->value("System/MAG_Device", "0").toInt();   // -设置测试IO板
    Ini_Vacuum_Model = set_Test_File->value("System/Vacuum_Model", "0").toInt(); // -设置真空测试
    Ini_BLDC_PG      = set_Test_File->value("System/PLC_BLDC_PG", "0").toInt();
    // -设置 BLDC_PG 控制 PLC

    if (system_conf.size() != system_conf_list.size()) {
        system_conf = system_conf_list;
    }
    ui->label_Version->setText(ui->label_Version->text().append(Version_number));

    QString magtext;
    magtext = set_Test_File->value(QString("MAG/text").toUtf8().data()).toString();
    if (magtext == QString("")) {
        set_Test_File->setValue(QString("MAG/text").toUtf8().data(), "Positive");
    } else {
        if (magtext == QString(tr("Positive"))) {
            //
        } else if (magtext == QString(tr("negative"))) {
            //
        } else {
            //
        }
    }
    system("sync");
    delete set_Test_File;

    DCR_Beep_Ok = 0;
    DCR_Beep_Ng = 0;
    DCR_BEEP = 0;

    calibration_count = 0;
    Cheak_Ready_count = 0;

    GND_State = false;
    AMP_All_Count = 0;
    AMP_No_Count  = 0;

    Shake_hand_Success = false;
    Shake_hand_runcount = 0;
    WMessage_Text.clear();
    WMessage_Text.append(QString(tr("         输出板异常        ")));  // 0
    WMessage_Text.append(QString(tr("         电阻板异常        \n         B02A--异常      ")));
    WMessage_Text.append(QString(tr("         绝缘板异常        \n         B03A--异常      ")));
    WMessage_Text.append(QString(tr("         匝间板异常        \n         B04A--异常      ")));
    WMessage_Text.append(QString(tr("         电参板异常        \n         B07A--异常      ")));
    WMessage_Text.append(QString(tr("         电感板异常        \n         B06A--异常      ")));
    WMessage_Text.append(QString(tr("         左输出板异常        \n         B10A---异常      ")));
    WMessage_Text.append(QString(tr("         右输出板异常        \n         B10A---异常      ")));
    WMessage_Text.append(QString(tr("         系统接地异常！        \n         建议接地后使用     ")));
    WMessage_Text.append(QString(tr("         电源切换板异常！        \n         B09A 异常     ")));
    WMessage_Text.append(QString(tr("         内存空间不足  \n    建议导出数据，清除数据库       ")));
    WMessage_Text.append(QString(tr("         WD板异常        \n         B0BA---异常      ")));
    WMessage_Text.append(QString(tr("         PG板异常        \n         B0CA---异常      ")));
    WMessage_Text.append(QString(tr("         IO板异常        \n         B0DA---异常      ")));
    WMessage_Text.append(QString(tr("         相序异常        ")));     // 14
    WMessage_Text.append(QString(tr("         热继电器异常        ")));  // 15

    WD_I = 0.0;
    WD_V = 0.0;
    WD_P = 0.0;
    Fg_Direction = 0;
    Square_WD = 0.0;

    PG_Fre = 0;
    PG_Bb  = 0;
    PG_High = 0;
    PG_Low = 0;
    BEMF_HALL.clear();
    for (int i = 0; i < 200; i++) {
        BEMF_HALL.append("0");
    }
    BEMF_HIGH_Low.clear();
    for (int i = 0; i < 150; i++) {
        BEMF_HIGH_Low.append("0");
    }
    Shape_BEMF_Fre.clear();
    for (int i = 0; i < 20; i++) {
        Shape_BEMF_Fre.append("0");
    }
    Shape_Pg.clear();
    WD_PWR = false;
    
    RFID_H.clear();

}
/******************************************************************************
      * version:    1.0
      * author:     link
      * date:       2015.12.10
      * brief:      主界面析构
******************************************************************************/
w_Main::~w_Main() {
    delete this->p_ipmethod;
    delete this->Button_Main_Group;
    tcp->quit();
    tcp->wait();
    udp->quit();
    udp->wait();
    delete ui;
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.12.10
      * brief:      处理鼠标被按下事件
******************************************************************************/
void w_Main::mousePressEvent(QMouseEvent *event) {
    handleMousePressEvent(event);
    Mouse.Time->start(1);
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.12.10
      * brief:      处理鼠标被按下事件
******************************************************************************/
void w_Main::mouseReleaseEvent(QMouseEvent *event) {
    event = 0;
    Mouse.Time->stop();
    if ((Mouse.Flag) && (Mouse.Ms >= 2)) {
        Pubs_ChangeWindow(5);
    }
    Mouse.Ms = 0;
    Mouse.Us = 0;
    Mouse.Flag = false;
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.12.10
      * brief:      鼠标长按处理
******************************************************************************/
void w_Main::Mouse_Judge() {
    Mouse.Us++;
    if (Mouse.Us >= 1000) {
        Mouse.Us = 0;
        Mouse.Ms++;
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.12.10
      * brief:      鼠标进行位置选择
******************************************************************************/
bool w_Main::handleMousePressEvent(QMouseEvent *event) {
    QPoint topLeft(680, 440);
    QPoint rightBottom(800, 600);
    QRect  rect(topLeft, rightBottom);
    if (rect.contains(event->pos())) {
        Mouse.Flag = true;
    } else {
        Mouse.Flag = false;
    }
    return true;
}

/******************************************************************************
      * version:    1.0
      * author:     link
      * date:       2015.12.10
      * brief:      按钮功能
******************************************************************************/
void w_Main::Button_Main_Group_Judge(int id) {
    QString CurrentDateTime;
    QStringList Search;
    QStringList data_t;
    switch (id) {
    case Qt::Key_A: //-进入系统设置
        Pubs_ChangeWindow(10);
        break;
    case Qt::Key_B: //-进入型号管理
        Pubs_ChangeWindow(8);
        break;
    case Qt::Key_C: //-进入数据管理
        CurrentDateTime = QDateTime::currentDateTime().toString("yyyy/MM/dd"); //设置显示格式
        Search << wConf->Ini_ActiveFile << CurrentDateTime << CurrentDateTime;
        wData->Structure_Interface("wMain", wConf->Ini_ActiveFile);
        Pubs_ChangeWindow(3);
        qApp->processEvents(); // 立即显示生效

        data_t.append(QString::number(wConf->Conf_User));
        data_t.append(QString::number(wConf->Ini_ACW_And_IR));
        data_t.append(Ini_Motor);
        wData->Pub_Set_Data(data_t);
        wData->File = wConf->currentFile;
        break;
    case Qt::Key_D: //-进入测试
        Pubs_ChangeWindow(4);
        wConf->Pub_main_to_conf(QStringList(""), QString(""), 1, 7);
        break;
    case Qt::Key_E: //-退出
        Pubs_ChangeWindow(100);
        break;
    default:

        break;
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:
      * brief:      conf界面的接收函数
******************************************************************************/
void w_Main::Pubs_from_conf(QStringList list, int data, int value) {
    switch (value) {
    case 1:
        Pri_Sync_Data(list);
        Pubs_ChangeWindow(data);
        break;
    case 2:
        Pubs_ChangeWindow(data);
        break;
    case 3:
        ui->label_sqldata->show();
        break;
    default:

        break;
    }
}

void w_Main::Pri_Send_FG_Test_Enable() {
    wTest->FG_Conf_Item.clear(); //
    wTest->FG_Conf_Item.append("0"); // 空载
    wTest->FG_Conf_Item.append("0"); // 负载
    wTest->FG_Conf_Item.append("0"); // BEMF

    if (wTest->Conf_Item.contains(tr("11"))) { // FG
        if (wConf->WPG->wave_count/1000 == 0) {
            wTest->FG_Conf_Item.replace(0, QString::number(wConf->WPG->wave_count));
        } else if (wConf->WPG->wave_count/1000 == 1) {
            wTest->FG_Conf_Item.replace(1, QString::number(wConf->WPG->wave_count));
        } else if (wConf->WPG->wave_count/1000 == 2) {
            wTest->FG_Conf_Item.replace(2, QString::number(wConf->WPG->wave_count));
        }
        qDebug() << wTest->FG_Conf_Item;
    } else {
        //
    }
}

void w_Main::Pri_Send_PLC() {
    QStringList PLC_List;
    PLC_List.clear();
    if (wTest->Conf_Item.contains(tr("6"))) { // 匝间
        PLC_List.append("1");
    } else {
        PLC_List.append("0");
    }

    if (wTest->Conf_Item.contains(tr("3"))) { // 绝缘
        PLC_List.append("1");
    } else {
        PLC_List.append("0");
    }

    if (wTest->Conf_Item.contains(tr("4"))) { // 交耐
        PLC_List.append("1");
    } else {
        PLC_List.append("0");
    }

    if (wTest->Conf_Item.contains(tr("2"))) { // 反嵌
        PLC_List.append("1");
    } else {
        PLC_List.append("0");
    }

    if (Ini_MAG_Device == 1) { // 反嵌驱动-美芝项目
        PLC_List.append("1");
    } else {
        PLC_List.append("0");
    }
    qDebug() << "Main-2017-12-20 PIC" << PLC_List;
    Signal_main_to_pic(PLC_List, 0, 7);
}

/******************************************************************************
      * version:    1.0
      * author:     link
      * date:       2015.12.10
      * brief:      同步数据
******************************************************************************/
void w_Main::Pri_Sync_Data(QStringList Data) {
    int i;
    if (wTest == NULL) {
        return;
    }
    if (wConf == NULL) {
        return;
    }

    Main_ActiveFile = Data.at(0);
    Pri_Synchron_Result_Data(); // 结果参数
    wTest->Conf_Item = wConf->Conf_Test;
    Pri_Send_PLC();
    Pri_Send_FG_Test_Enable();
    wTest->COLOR_Id  = wConf->colorIdList_C; // -颜色Color

    for (i = 0; i < Ini_Proj.size(); i++) {
        switch (Ini_Proj.at(i).toInt()) {
        case  1:
            wTest->strRES = wConf->strRES;
            wTest->DLR_UpDown = wConf->strDLR_UpDown;
            break;
        case  2:
            wTest->strOPP = wConf->strOPP;
            break;
        case  3:
            wTest->strIR  = wConf->strIR;
            break;
        case  4:
            wTest->strACW = wConf->strACW;
            break;
        case  5:
            //
            break;
        case  6:
            wTest->strITT = wConf->strITT;
            wTest->IMP_Param = wConf->strIMP_Param;
            wTest->Pub_Main_to_test(QStringList(""), Main_ActiveFile, 1, 21);
            break;
        case  7:
            wTest->strPWR = wConf->strPWR;
            break;
        case  8:
            wTest->strINDL = wConf->strINDL;
            break;
        case  9:
            wTest->strBLOCK = wConf->strBLOCK;
            break;
        case 10:
            wTest->strLVS = wConf->strLVS;
            break;
        case 11:
            wTest->strPG = wConf->strPG;

            break;
        case 12:
            wTest->strLOAD = wConf->strLOAD;
            break;
        case 13:
            wTest->strNOLOAD = wConf->strNOLOAD;
            break;
        case 14:
            wTest->strBEMF = wConf->strBEMF;
            break;
        default:
            break;
        }
    }
    wTest->Pub_Main_to_test(Data, QString(""), 1, 8);
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      测试界面获取系统配置界面的参数
******************************************************************************/
void w_Main::Pri_Synchron_Syst_Data() {
    qDebug() << "system_conf" << system_conf;
    QStringList list;
    list.append(system_conf);
    list.append("");
    list.append("");
    list.append(QString::number(barcode_start));
    list.append(QString::number(barcode_stop));
    DCR_Beep_Ok = system_conf.at(1).toDouble();
    DCR_Beep_Ng = system_conf.at(2).toDouble();
    wTest->Pub_Main_to_test(list, QString(""), 1, 3);
}

/******************************************************************************
      * version:    1.0
      * author:     link
      * date:       2015.12.10
      * brief:      获取配置密码
******************************************************************************/
void w_Main::Pri_Synchron_PassWord_Data() {
    wPassWord->Password.Syst =  Ini_Password.at(0);   //  密码
    wPassWord->Password.Debug = Ini_Password.at(1);
    wPassWord->Password.Conf =  Ini_Password.at(2);
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.8.26
      * brief:      同步数据
******************************************************************************/
void w_Main::Pri_Synchron_Result_Data() {
    int i;

    for (i = 0; i < Ini_Proj.size(); i++) {
        switch (Ini_Proj.at(i).toInt()) {
        case 1:
            wTest->DCR_Total_Number = wConf->WDLR->DLR_TestNumber;
            wTest->DCR_TestItem_H   = wConf->WDLR->DLR_TestItemsH;
            wTest->DCR_TestItem_L   = wConf->WDLR->DLR_TestItemsL;
            break;
        case 2:
            wTest->MAG_Total_Number = wConf->WMAG->MAG_TestNumber;
            wTest->MAG_TestItem = wConf->WMAG->MAG_TestItems;
            break;
        case 3:
            wTest->IR_Test_Total = wConf->WIR->IR_Test_Total;
            wTest->IRgroup_num = wConf->WIR->IRgroup_num;
            break;
        case 4:
            wTest->ACW_Test_Total = wConf->WACW->ACW_Test_Total;    //测试总数
            wTest->ACWgroup_num = wConf->WACW->ACWgroup_num;        //当前测试项
            break;
        case 5:
            break;
        case 6:
            wTest->IMP_Total_Number = wConf->WIMP->IMP_TestNumber;
            wTest->IMP_TestItem  = wConf->WIMP->IMP_TestItems;
            break;
        case 7: // 电参
            wTest->PWR_Test_Data = wConf->WPWR->PWR_Test_Data;
            wTest->PG_Test_Order = wConf->WPWR->PG_Test_Order;
            break;
        case 8: // 电感
            wTest->INDL_Total_Number = wConf->WINDL->INDL_TestNumber;
            wTest->INDL_TestItem_H   = wConf->WINDL->INDL_TestItemsH;
            wTest->INDL_TestItem_L   = wConf->WINDL->INDL_TestItemsL;
            break;
        case 9: // 堵转
            //
            break;
        case 10: // 低启
            //
            break;
        case 11: // PG
            wTest->FG_Test_Data = wConf->WPG->FG_Test_Data;
            break;
        case 12: // LOAD
            //
            break;
        case 13: // NOLOAD

            break;
        case 14: // BEMF
            //
            break;
        default:
            //
            break;
        }
    }
}

/******************************************************************************
      * version:    1.0
      * author:     link
      * date:       2015.12.10
      * brief:      窗口切换
******************************************************************************/
void w_Main::Pubs_ChangeWindow(int win) {
    wTest->releaseKeyboard();
    window_current = win;
    if (window_old == wTest_Surface) { // -判断在离开或者进入测试界面时，关闭指示灯
        Pubs_Usart_Send(QString("LED1").toLatin1().data());
        Signal_main_to_pic(QStringList(""), out_station-19, 17);
        Signal_main_to_pic(QStringList(""), 0, 13);

//        frame.can_id  = 0x2D;  frame.can_dlc = 0x04;
//        frame.data[0] = 0xf1;
//        frame.data[1] = 0x00;
//        frame.data[2] = 0x00;
//        frame.data[3] = 0x00;
//        canSend(frame, 88);
    }
    switch (win) {
    case 0: // -主界面
        this->show();    this->raise();     this->activateWindow();      this->setFocus();
        break;
    case 1: // -系统设置
        if (wSyst == NULL) {
            return;
        }
        qDebug() << "wSyst";
        Sigmain_data_to_syst(QString(""), 2);
        wSyst->show();     wSyst->raise();    wSyst->activateWindow();    wSyst->setFocus();
        Sigmain_save_DayRecoord("System", ""); //-日志记录
        break;
    case 2: // -型号管理
        //
        break;
    case 3: // -数据管理
        if (wData == NULL) {
            return;
        }
        qDebug() << "wData";
        wData->show();    wData->raise();     wData->activateWindow();     wData->setFocus();
        break;
    case 4: // -进入测试
        if (wTest == NULL) {
            return;
        }
        qDebug() << "wTesting";
        Signal_main_to_pic(QStringList(""), 1, 13);
        wTest->Pub_Main_to_test(QStringList(""), QString(""), 1, 9);
        wTest->show();    wTest->raise();     wTest->activateWindow();     wTest->setFocus();
        wTest->grabKeyboard();
        // -2017.6.7,下发启动方式 更改为滑罩启动
        Pri_Can_Set_OUTModel(0x13, system_conf.at(4).toInt()); usleep(1800);
        Pri_Can_Set_OUTModel(0x14, system_conf.at(4).toInt()); usleep(1800);
        break;
    case 5: //-进入帮助
        if (wHelp == NULL) {
            return;
        }
        if (wTest->Back_Test_Status() != Test_Station_Free) {
            return;
        }
        qDebug() << "wHelp";
        wHelp->show();    wHelp->raise();     wHelp->activateWindow();     wHelp->setFocus();
        wHelp->updateText(window_old, wConf->index_c);
        break;
    case 6: //-进入后台
        //
        break;
    case 7: //-颜色设置
        if (wColr == NULL) {
            return;
        }
        qDebug() << "wColr";
        wColr->show();  wColr->raise();     wColr->activateWindow();        wColr->setFocus();
        break;
    case 8: //-设置界面
        if (wConf == NULL) {
            return;
        }
        qDebug() << "wConf";
        wConf->show();  wConf->raise();    wConf->activateWindow();      wConf->setFocus();
        Pubs_Usart_Send(QString("LED1").toLatin1().data());
        wConf->Pub_main_to_conf(QStringList(""), QString(""), wTest->JUMP_Chose*1000, 11);
        Pri_Can_Set_OUTModel(0x13, 2); usleep(1800); // -2017.6.7,下发启动方式 更改为滑罩启动
        Pri_Can_Set_OUTModel(0x14, 2); usleep(1800);
        break;
    case 9: //-调试界面
        if (wDebug == NULL) {
            return;
        }
        qDebug() << "wDebug";
        wDebug->show();    wDebug->raise();   wDebug->activateWindow();   wDebug->setFocus();
        Sigmain_data_to_debug(QStringList(""), QString(""), 5);
        Sigmain_save_DayRecoord("Debug", ""); // -日志记录
        break;
    case 10: //-密码界面
        if (wPassWord == NULL) {
            return;
        }
        qDebug() << "wPassWord";
        wPassWord->CheckDisplay(window_old);
        wPassWord->Password_Check = window_old;
        wPassWord->show();   wPassWord->raise();    wPassWord->activateWindow();
        wPassWord->setFocus();

        break;
    default: //-程序退出
        if (wSyst != NULL) wSyst->close();
        if (wData != NULL) wData->close();
        if (wTest != NULL) wTest->close();
        if (wHelp != NULL) wHelp->close();
        if (wConf != NULL) wConf->close();
        if (wColr != NULL) wColr->close();
        if (wDebug != NULL)    wDebug->close();
        if (wPassWord != NULL) wPassWord->close();

        myThread->quit();
        myCom->WritePort(QString("LED1").toLatin1().data(), 4);
        myCom->ClosePort();
        delete myCom;
        myCom = NULL;
        delete wpic;

        can.canClose();
        this->close();
        break;
    }
    window_old = win; // -保存界面ID备用
}

/******************************************************************************
      * version:    1.0
      * author:     link
      * date:       2015.12.10
      * brief:      同步颜色
******************************************************************************/
void w_Main::Pri_Sync_Color(QString colr) {
    if (wConf == NULL) {
        return;
    } else {
        //
    }
    wConf->Pub_main_to_conf(QStringList(""), colr, 1, 4);
}

/******************************************************************************
      * version:    1.0
      * author:     link
      * date:       2015.12.10
      * brief:      CAN总线发送数据
******************************************************************************/
void w_Main::canSend(can_frame frame, int Judge_Win) {
    int i;
    if ((Judge_Win == window_current) || (Judge_Win == 88) || (Judge_Win == 99)) {
        can.canSend(frame);
//        qDebug() << "send" << frame.can_id << Judge_Win;
//        for (i = 0; i < frame.can_dlc; i++) {
//            qDebug() << "i" << i << frame.data[i];
//        }
    } else {
        //
    }
    if (Judge_Win != 99) {
        can_bug_time = 0;
    } else {
        //
    }
}
/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.12.10
      * brief:      系统设置界面获取参数
******************************************************************************/
void w_Main::Pubs_from_syst(QStringList list, int number, int value) {
    switch (value) {
    case 1:
        Pubs_Cheak_Ready();
        break;
    case 2:
        Pri_SaveSystemData(list);
        break;
    case 3:
        Pubs_ChangeWindow(number);
        break;
    case 4:

        break;
    case 5:
         wTest->Pub_Main_to_test(QStringList(""),QString(""),number,118);
        break;
    case 6:
        frame.can_id = 0x2D;  frame.can_dlc = 0x03;
        frame.data[0] = 0x12;
        frame.data[1] = 0x02;
        frame.data[2] = number;
        canSend(frame,88);
        usleep(500);
        break;
    case 7:
        frame.can_id = 0x2D;  frame.can_dlc = 0x02;
        frame.data[0] = 0x12;
        frame.data[1] = 0x01;
        canSend(frame,88);
        usleep(500);
        break;
    case 8:
        Signal_main_to_Switchover(Bosch, number, 7);
        break;
    default:
        break;
    }
}
/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      数据管理界面获取参数
******************************************************************************/
void w_Main::Pubs_from_data (QStringList list, int number, int value)
{
    switch(value)
    {
    case 1:
        Pubs_ChangeWindow(number);
        break;
    case 2:
        Singal_to_SQL(QString(""),list,3);
        MYSQL_DB->start();
        break;
    case 3:
        Singal_to_SQL(QString(""),list,1);
        MYSQL_DB->start();
        break;
    case 4:
        Singal_to_SQL(QString(""),QStringList(""),2);
        MYSQL_DB->start();
        break;
    case 5:
        Singal_to_SQL(QString(""),QStringList(""),4);
        break;
    default:
        break;
    }
}
/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      密码界面获取参数
******************************************************************************/
void w_Main::Pubs_from_pass(int number, int value) {
    switch (value) {
    case 1:
        Pubs_ChangeWindow(number);
        break;
    case 2:
        Pri_Change_Password();
        break;
    default:
        break;
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      颜色界面获取参数
******************************************************************************/
void w_Main::Pubs_from_color(QString str, int number, int value) {
    switch (value) {
    case 1:
        Pubs_ChangeWindow(number);
        break;
    case 2:
        Pri_Sync_Color(str);
        break;
    default:
        break;
    }
}
/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      测试界面获取参数
******************************************************************************/
void w_Main::Pubs_from_test(int number, int value) {
    switch (value) {
    case 1:
        Pubs_ChangeWindow(number);
        break;
    case 2:

        break;
    case 3:

        break;
    case 5:

        break;
    default:
        break;
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      调试界面获取参数
******************************************************************************/
void w_Main::Pubs_from_debug(QStringList list, int number, int value) {
    switch (value) {
    case 1:
        Pubs_ChangeWindow(number);
        wTest->Pub_Main_to_test(list, QString(""), 1, 18);
        break;
    case 2:

        break;
    case 3:
        ui->label_net->show();
        break;
    case 4:
        ui->label_net->hide();
        break;
    default:
        break;
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.5.5
      * brief:      保存系统配置数据
******************************************************************************/
void w_Main::Pri_SaveSystemData(QStringList list)
{
    int i;
    if (system_conf != list.mid(0, 6)) {
        if (system_conf.at(4).toInt() != list.at(4).toInt()) { // 下发启动方式
            for (i = 0; i < 5; i++) {
                if (board_state[i*2] != 0) {
                    continue;
                } else {
                    //
                }
                Pri_Can_Set_OUTModel(0x13+i, list.at(4).toInt());
            }
        }
        system_conf = list;
        wConf->Pub_main_to_conf(list, QString(""), 2, 3);
        DCR_Beep_Ok = system_conf.at(1).toDouble();
        DCR_Beep_Ng = system_conf.at(2).toDouble();
    } else {
        qDebug() << "The system configuration data is the same";
    }
    wTest->Pub_Main_to_test(list, QString(""), 1, 3);
}

/******************************************************************************
      * version:    1.0
      * author:     link
      * date:       2016.9.26
      * brief:      网络初始化设置
******************************************************************************/
void w_Main::Pri_TcpInit()
{
    tcp = new QThread(this); // 启动tcp连接
    MyTcpThread *tcpThread = new MyTcpThread;
    tcpThread->moveToThread(tcp);
    connect(tcp, SIGNAL(started()), tcpThread, SLOT(TcpInit()));
    connect(tcpThread, SIGNAL(TransformCmd(quint16, quint16, QByteArray)), \
            wDebug, SLOT(Slot_TransformCmd(quint16, quint16, QByteArray)));
    tcp->start();

    udp = new QThread(this); // 启动udp连接
    UdpClient *udpThread = new UdpClient;
    udpThread->moveToThread(udp);
    connect(udp, SIGNAL(started()), udpThread, SLOT(Init()));
    connect(wConf, SIGNAL(WriteMessage(quint16, quint16, QByteArray)), \
            udpThread, SLOT(ReadMessage(quint16, quint16, QByteArray)));
    connect(wTest, SIGNAL(WriteMessage(quint16, quint16, QByteArray)), \
            udpThread, SLOT(ReadMessage(quint16, quint16, QByteArray)));
    connect(udpThread, SIGNAL(SendCommand(quint16, quint16, QByteArray)), \
            this, SLOT(ReadMessage(quint16, quint16, QByteArray)));
    connect(this, SIGNAL(WriteMessage(quint16, quint16, QByteArray)), \
            udpThread, SLOT(ReadMessage(quint16, quint16, QByteArray)));
    udp->start();

    if (wTest->START_Model == 4) { // 连接模式,进行延时等待结果
        QEventLoop Wait_Shake_hand;
        for (int x = 0; x < 600; x++) {
            if (x % 2) {
                ui->label_hand->show();
            } else {
                ui->label_hand->hide();
            }
            if (Shake_hand_Success) {
                x = 616;
                ui->label_hand->show();
            }
            QTimer::singleShot(1000, &Wait_Shake_hand, SLOT(quit()));
            Wait_Shake_hand.exec();
            qDebug() << "Wait PC shake hand....";
        }
    } else {
        qDebug() << "Not shake hand";
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      读取网络控制功能
******************************************************************************/
void w_Main::ReadMessage(quint16 addr, quint16 cmd, QByteArray msg)
{
    QString STATUSING;
    QSettings *set_Test_File;
    int Set_IO_Data;
    if (addr != 6000) {
        return;
    } else {
        //
    }
    QStringList n = QString(msg).split(" ");
    switch (cmd) {
    case CMD_START:
        if (window_current == wTest_Surface) {
            wTest->Pub_Main_to_test(QStringList(""), "3", n.at(0).toInt(), 11);
        } else {
            //
        }
        break;
    case CMD_STOP:
        wTest->Pub_Main_to_test(QStringList(""), "0", 1, 12);
        break;
    case CMD_STATUSING:
        if (wTest->Back_Test_Status() == Test_Station_Free) {
            STATUSING = "free";
        } else {
            STATUSING = "busy";
        }
        WriteMessage((6000), (0x18), STATUSING.toUtf8());
        break;
    case CMD_Param:
        if ((wTest->Back_Test_Status() != Test_Station_Free) || \
                (Main_ActiveFile == QString(msg))) {
            WriteMessage((6000), (0x18), QString(tr("Busy or Used")).toUtf8());
        } else {
            Pubs_ChangeWindow(8);     // 进入配置界面
            if (wConf->currentFile.contains(QString(msg))) {
                Pubs_ChangeWindow(8); // 进入配置界面
                wConf->Pub_main_to_conf(QStringList(""), QString(msg), 1, 18); // 修改型号
                Pubs_ChangeWindow(4); // 进入测试界面
                wConf->Pub_main_to_conf(QStringList(""), QString(""), 1, 7);
                WriteMessage((6000), (0x18), QString(tr("Ok")).toUtf8());
            } else {
                WriteMessage((6000), (0x18), QString(tr("No Model")).toUtf8());
            }
        }
        break;
    case CMD_ICO:
        if (udp_enable) {
            ui->label_print->show(); // -显示打印图标
        } else {
          //
        }
        if (wTest->START_Model == 4) {
            ui->label_hand->show();    // -显示握手标志
            Shake_hand_Success = true; // -进行握手标志
            WriteMessage((6000), (0x21), QString(tr("Login")).toUtf8());
        } else {
            //
        }
        break;
    case CMD_Reply:
        //
        break;
    case CMD_CONF_Data:
        wConf->Pub_main_to_conf(QStringList(""), QString(""), n.at(0).toInt(), 13);
        if (n.at(0).toInt() == 1) {
            Pubs_ChangeWindow(8);
        } else {
            //
        }
        break;
    case CMD_Start_Item:

        qDebug() << "555";
        wTest->Pub_Main_to_test(QStringList(""), n.at(1), 1, 10);
        break;
    case CMD_Init_Data:
        wConf->Pub_QbyteArray_conf(msg);
        break;
    case CMD_Judge:
        Button_Main_Group_Judge(Qt::Key_D);
        break;
    case CMD_Add_Model:
        wConf->Pub_main_to_conf(QStringList(""), n.at(1), 1, 10);
        break;
    case CMD_Del_Model:
        wConf->Pub_main_to_conf(QStringList(""), QString(""), 2, 10);
        break;
    case CMD_Oder_Model:
        wConf->Pub_main_to_conf(QStringList(""), QString(""), 3, 10);
        break;
    case CMD_Updae_Model:
        wConf->Pub_main_to_conf(QStringList(""), n.at(1), 5, 10);
        break;
    case CMD_All_Start:
        if (window_current == wTest_Surface) {
            if (Ini_Vacuum_Model) {
                Signal_main_to_pic(QStringList(""), n.at(1).toInt()-19, 15);
            } else {
                wTest->Pub_Main_to_test(QStringList(""), "3", n.at(1).toInt(), 11);
            }
        } else {
            //
        }
        break;
    case CMD_9a:
        if (window_current == wTest_Surface) {
            wTest->Pub_Main_to_test(QStringList(""), "2", n.at(1).toInt(), 11);
        } else {
            //
        }
        break;
    case CMD_All_Stop:
        qDebug() << "CMD_All_Stop*************************************>";
        wTest->Pub_Main_to_test(QStringList(""), "0", 1, 12);
        Signal_main_to_pic(QStringList(""), out_station-19, 17);
        break;
    case CMD_WMessage:
        WriteMessage((6000), CMD_WMessage, QString(WMessage_Record.join(" ")).toUtf8());
        break;
    case CMD_Time:
        wTest->Pub_Main_to_test(QStringList(""), QString(""), 6001, 1);
        break;
    case CMD_IO_Put:
        qDebug() << "n" << n;
        frame.can_id  = 0x2D;  frame.can_dlc = 0x04;
        frame.data[0] = 0xf1;
        frame.data[1] = n.at(1).toInt()%256; // (1<<(ui->IO_Bit1->text().toInt()));
        frame.data[2] = n.at(1).toInt()%65536/256; // (1<<(ui->IO_Bit2->text().toInt()));
        frame.data[3] = n.at(1).toInt()/65536; // (1<<(ui->IO_Bit2->text().toInt()));
        canSend(frame, 88);
        break;
    case CMD_MAG_Sample:
        if (Ini_MAG_Device) {
            Signal_main_to_pic(QStringList(""), 1, 9);
        } else {
            wConf->Pub_main_to_conf(QStringList(""), QString(""), n.at(1).toInt(), 19);
            wConf->Pub_main_to_conf(QStringList(""), QString(""), 24, 2);
        }
        break;
    case CMD_IMP_Sample:
        qDebug() << "n.at(2).toInt()" << n.at(2).toInt();
        if (n.at(2).toInt()) {
            Signal_main_to_pic(QStringList(""), n.at(1).toInt(), 6);
        } else {
            wConf->Pub_main_to_conf(QStringList(""), QString(""), n.at(1).toInt(), 19);
            wConf->Pub_main_to_conf(QStringList(""), QString(""), 24, 2);
        }
        break;
    case CMD_IMP_Ready:
        //
        break;
    case CMD_IMP_Finsh:
        Signal_main_to_pic(QStringList(""), 1, 6);
        break;
    case CMD_Vacuum_IMP:
        Signal_main_to_pic(QStringList(""), n.at(1).toInt(), 5);
        wTest->Pub_Main_to_test(QStringList(""), QString(""), n.at(1).toInt(), 26);
        break;
    case CMD_Vacuum_ACW:
        Signal_main_to_pic(QStringList(""), n.at(1).toInt()+10, 5); // 耐压真空测试
        wTest->Pub_Main_to_test(QStringList(""), QString(""), n.at(1).toInt()+10, 26);
        break;
    case CMD_IMP_PL:
        wConf->WIMP->Pub_Conf_Set_IMP(n.at(1), 15);
        break;
    case CMD_Start_Auto:
        Signal_main_to_pic(QStringList(""), n.at(1).toInt(), 19);
        break;
    case CMD_PIC:
        if (n.at(1).toInt()) {
            qDebug() << "--------------------disconnect-PIC";
            PIC_Connect = false;
            Signal_main_to_pic(QStringList(""), 0, 18);
            disconnect(wpic, SIGNAL(canSend(can_frame, int)), \
                       this, SLOT(canSend(can_frame, int)));
        } else {
            qDebug() << "--------------------connect-PIC";
            PIC_Connect = true;
            connect(wpic, SIGNAL(canSend(can_frame, int)), this, SLOT(canSend(can_frame, int)));
        }
        break;
    case CMD_Start_No:
        wTest->Pub_Main_to_test(QStringList(""), QString(""), n.at(1).toInt(), 27);
        break;
    case CMD_Beep:
        if (n.size() < 3) {
            break;
        } else {
            //
        }
        system_conf.replace(1, n.at(1));
        system_conf.replace(2, n.at(2));
        Pri_Synchron_Syst_Data();
        break;
    case CMD_A1:
        if (n.at(1).toInt()) {
            Set_IO_Data = 0x01;
        } else {
            Set_IO_Data = 0x00;
        }
        frame.can_id  = 0x2D;  frame.can_dlc = 0x04;
        frame.data[0] = 0xf1;
        frame.data[1] = Set_IO_Data;
        frame.data[2] = 0x00;
        frame.data[3] = 0x00;
        canSend(frame, 88);
        break;
    case CMD_A2:
        if (n.at(1).toInt()) {
            Set_IO_Data = 0x02;
        } else {
            Set_IO_Data = 0x03;
        }
        frame.can_id  = 0x2D;  frame.can_dlc = 0x04;
        frame.data[0] = 0xf1;
        frame.data[1] = Set_IO_Data;
        frame.data[2] = 0x00;
        frame.data[3] = 0x00;
        canSend(frame, 88);
        break;
    case CMD_A3:
        if (n.size() != 4) {
            break;
        } else {
            set_Test_File = new QSettings(Sys_path, QSettings::IniFormat);
            set_Test_File->setIniCodec("GB18030");
            set_Test_File->setValue("Selfstart/value", QStringList(n.mid(1,3))); // -自动启动
            wTest->Pub_Main_to_test(QStringList(n.mid(1,3)), QString(""), 1, 18);
        }
        break;
    default:
        //
        break;
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.5.6
      * brief:      修改密码
******************************************************************************/
void w_Main::Pri_Change_Password()
{
    Ini_Password.clear();
    Ini_Password.append(wPassWord->Password.Syst);
    Ini_Password.append(wPassWord->Password.Debug);
    Ini_Password.append(wPassWord->Password.Conf);

    Pri_Synchron_PassWord_Data();
    wConf->Pub_main_to_conf(system_conf, QString(""), 3, 3);
}

/***********************************************************
    * @name	 	1.0
    * @brief  	SL
    * @param  	2016.3.24
    * @retval 	初始进入测试界面
***********************************************************/
void w_Main::Pubs_Cheak_Ready()
{
    int i;
    int Check_CurrentProj = 0;
    Cheak_Ready_count++;
    if (Cheak_Ready_count%2 == 1) {
        qDebug() << "Clear Data";
        for (int j = 0; j < 220; j++) { // 初始化记录数值
            board_state[j] = 100;
        }
    }
    for (i = 0; i < Ini_Proj_Real.size(); i++) {
        if (i <= 5) {
            Version.OutV[i]="0";
            if (Ini_Proj_Real.at(i).toInt() != 0) {
                Pri_Can_CheckState(0x13+i);   usleep(1800);
                Pri_Can_CheckVersion(0x13+i); usleep(1800);
            } else {
                //
            }
        } else if (Ini_Proj_Real.at(i).toInt() <= 2) { // 电阻板
            if (Check_CurrentProj != 2) {
                Pri_Can_CheckState(0x22);   usleep(1800);
                Pri_Can_CheckVersion(0x22); usleep(1800);
                Check_CurrentProj = 2;
            } else {
                //
            }
            continue;
        } else if (Ini_Proj_Real.at(i).toInt() <= 5) { // 绝缘板
            if (Check_CurrentProj != 5) {
                Pri_Can_CheckState(0x23);   usleep(1800);
                Pri_Can_CheckVersion(0x23); usleep(1800);
                Check_CurrentProj = 5;
            } else {
                //
            }
            continue;
        } else if (Ini_Proj_Real.at(i).toInt() <= 6) { // 匝间板
            if (Check_CurrentProj != 6) {
                Pri_Can_CheckState(0x24);   usleep(1800);
                Pri_Can_CheckVersion(0x24); usleep(1800);
                Check_CurrentProj = 6;
            } else {
                //
            }
            continue;
        } else if ((Ini_Proj_Real.at(i).toInt() == 7) || (Ini_Proj_Real.at(i).toInt() == 9) || \
                   (Ini_Proj_Real.at(i).toInt() == 10)) { // 电参板
            if (Check_CurrentProj != 7) {
                Pri_Can_CheckState(0x27);   usleep(1800);
                Pri_Can_CheckVersion(0x27); usleep(1800);
                usleep(1000);
                Check_CurrentProj = 7;
            } else {
                //
            }
            continue;
        } else if (Ini_Proj_Real.at(i).toInt() <= 8) { // 电感板
            if (Check_CurrentProj != 8) {
                Pri_Can_CheckState(0x26);   usleep(1800);
                Pri_Can_CheckVersion(0x26); usleep(1800);

                frame.can_id = 0x26;  frame.can_dlc = 0x02;
                frame.data[0] = 0x07;
                frame.data[1] = 0x00;
                canSend(frame, 88); //  输出板发送
                usleep(1800);
                Check_CurrentProj = 8;
            } else {
                //
            }
            continue;
        } else if (Ini_Proj_Real.at(i).toInt() <= 11) { // 电感板
            if (Check_CurrentProj != 11) {
                qDebug() << "State PG";
                Pri_Can_CheckState(0x2C);   usleep(1800);
                Pri_Can_CheckVersion(0x2C); usleep(1800);
                canSend(frame, 88); //  输出板发送
                usleep(1800);
                Check_CurrentProj = 11;
            } else {
                //
            }
            continue;
        } else {
            //
        }
    }
    if (nine_board) { // 判断为9插板，进行功放板状态的查询
        Pri_Can_CheckState(0x2A);   usleep(1800);
        Pri_Can_CheckVersion(0x2A); usleep(1800); // 进行功放板的版本查询
    } else {
        //
    }
    if (acsw_board) { // 查询电源板
        Pri_Can_CheckState(0x29);   usleep(1800); // 进行电源切换板的查询
        Pri_Can_CheckVersion(0x29); usleep(1800); // 进行电源切换板的版本查询
    } else {
        //
    }
    if (Ini_WD == QString(tr("WD"))) {  // 查询WD板
        Pri_Can_CheckState(0x2B);   usleep(1800); // 进行WD的查询
        Pri_Can_CheckVersion(0x2B); usleep(1800); // 进行WD的版本查询
    } else {
        //
    }

    if (Ini_IO) { // 查询WD板
        qDebug() << "11111";
        Signal_main_to_pic(QStringList(""), Ini_IO_Model, 119); // 控制气缸
        Pri_Can_CheckState(0x2d);   usleep(1800); // 进行WD的查询
        Pri_Can_CheckVersion(0x2d); usleep(1800); // 进行WD的版本查询

        frame.can_id  = 0x2D;  frame.can_dlc = 0x03;
        frame.data[0] = 0xf5;
        frame.data[1] = 0x01;
        canSend(frame, 88);
        usleep(1800);

        frame.can_id  = 0x2D;  frame.can_dlc = 0x01;
        frame.data[0] = 0xf0;
        canSend(frame, 88);
        usleep(1800);
    } else {
        //
    }

    QTimer *Time_Cheak_Finsh = new QTimer(this);
    if ((window_current == wMain_Surface) && (Cheak_Ready_count%2 == 0)) {
        connect(Time_Cheak_Finsh, SIGNAL(timeout()), this, SLOT(Pubs_Cheak_Main_Finsh()));
        Time_Cheak_Finsh->setSingleShot(TRUE);
        Time_Cheak_Finsh->start(280);
    } else if ((window_current == wSyst_Surface) && (Cheak_Ready_count%2 == 0)) {
        connect(Time_Cheak_Finsh, SIGNAL(timeout()), this, SLOT(Pubs_Cheak_Syst_Finsh()));
        Time_Cheak_Finsh->setSingleShot(TRUE);
        Time_Cheak_Finsh->start(280);
    } else {
        connect(Time_Cheak_Finsh, SIGNAL(timeout()), this, SLOT(Pubs_Cheak_Ready()));
        Time_Cheak_Finsh->setSingleShot(TRUE);
        Time_Cheak_Finsh->start(50);
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      系统设置界面查询板子的状态
******************************************************************************/
void w_Main::Pubs_Cheak_Syst_Finsh()
{
    int i;
    int Error_Flag = 0;
    int All_OK = 0;
    int Check_CurrentProj = 0;

    for (i = 0; i < Ini_Proj_Real.size(); i++) {
        if (i<= 5) {
            if (Ini_Proj_Real.at(i).toInt() != 0) {
                if (board_state[i*2] == 0) { //  输出板
                    //
                } else if (board_state[i*2] == 100) {
                    Error_Flag = i+13;
                } else {
                    Error_Flag = i+13;
                }
            }
        }  else if (Ini_Proj_Real.at(i).toInt() <= 2) {
            if (Check_CurrentProj != 2) {
                if (board_state[20] == 0) { //  电阻板
                    //
                } else if (board_state[20] == 100) {
                    Error_Flag = 2;
                } else {
                    Error_Flag = 2;
                }
                Check_CurrentProj = 2;
            }
        } else if (Ini_Proj_Real.at(i).toInt() <= 5) {
            if (Check_CurrentProj != 3) {
                if (board_state[40] == 0) { //  绝缘板
                   //
                } else if (board_state[40] == 100) {
                    Error_Flag = 3;
                } else {
                    Error_Flag = 3;
                }
                Check_CurrentProj = 3;
            }
        } else if (Ini_Proj_Real.at(i).toInt() <= 6) {
            if (Check_CurrentProj != 4) {
                if (board_state[60] == 0) { //  匝间板
                   //
                } else if (board_state[60] == 100) {
                    Error_Flag = 4;
                } else {
                    Error_Flag = 4;
                }
                Check_CurrentProj = 4;
            } else {
                //
            }
        } else if ((Ini_Proj_Real.at(i).toInt() == 7) || (Ini_Proj_Real.at(i).toInt() == 9) || \
                   (Ini_Proj_Real.at(i).toInt() == 10)) { // 电参板
            if (Check_CurrentProj != 5) {
                if (board_state[80] == 0) {
                    ui->Print_Text->append("System PWR Ok");
                    ui->Print_Text->append("System PWR Version");
                    ui->Print_Text->append(Version.PWR_LV+"  ");
                } else if (board_state[80] == 100) {
                    Error_Flag = 5;
                    ui->Print_Text->append("System PWR Can communication Error");
                } else {
                    Error_Flag = 5;
                    ui->Print_Text->append("System PWR Error");
                    ui->Print_Text->append(QString::number(board_state[80]));
                    ui->Print_Text->append("System PWR Error Value");
                    ui->Print_Text->append(QString::number(board_state[90]));
                }
                Check_CurrentProj = 5;
            } else {
                //
            }
        } else if (Ini_Proj_Real.at(i).toInt() <= 8) {
            if (Check_CurrentProj != 7) {
                if ((board_state[100] == 0) || (board_state[100] == 3)) { //  电感板
                    //
                } else if (board_state[100] == 100) {
                    Error_Flag = 7;
                } else {
                    Error_Flag = 7;
                }
                Check_CurrentProj = 7;
            } else {
                //
            }
        } else if (Ini_Proj_Real.at(i).toInt() <= 11) {
            if (Check_CurrentProj != 8) {
                if ((board_state[170] == 0) || (board_state[170] == 10)) { // PG板
                    //
                } else if (board_state[170] == 100) {
                    Error_Flag = 8;
                } else {
                    Error_Flag = 8;
                }
                Check_CurrentProj = 8;
            } else {
                //
            }
        } else {
            //
        }
        if (Error_Flag != 0) {
            switch (Error_Flag) {
            case 1:
                Sigmain_data_to_syst(QString(tr("         输出板异常        ")), 1);
                break;
            case 2:
                Sigmain_data_to_syst(QString(tr("         电阻板异常        \n" \
                                                "B02A--异常      ")), 1);
                break;
            case 3:
                Sigmain_data_to_syst(QString(tr("         绝缘板异常        \n" \
                                                "B03A--异常      ")), 1);
                break;
            case 4:
                Sigmain_data_to_syst(QString(tr("         匝间板异常        \n" \
                                                "B04A--异常      ")), 1);
                break;
            case 5:
                Sigmain_data_to_syst(QString(tr("         电参板异常        \n" \
                                                "B07A--异常      ")), 1);
                break;
            case 6:
                Sigmain_data_to_syst(QString(tr("         电参板异常        \n" \
                                                "B07A--异常      ")), 1);
                break;
            case 7:
                Sigmain_data_to_syst(QString(tr("         电感板异常        \n" \
                                                "B06A--异常      ")), 1);
                break;
            case 8:
                Sigmain_data_to_syst(QString(tr("         PG板异常        \n" \
                                                "B0CA--异常      ")), 1);
                break;
            default:
                if (Error_Flag == 13) {
                    Sigmain_data_to_syst(QString(tr("         左输出板异常        \n" \
                                                    "B10A---异常      ")), 1);
                } else if (Error_Flag == 14) {
                    Sigmain_data_to_syst(QString(tr("         右输出板异常        \n" \
                                                    "B10A---异常      ")), 1);
                } else {
                    //
                }
                break;
            }
            All_OK++;
            Error_Flag = 0;
        }
    }
    if ((nine_board) && (!GND_State)) { // 判断为9插板进行，接地的检查
        All_OK++;
        Sigmain_data_to_syst(QString(tr("         系统接地异常！        \n" \
                                        "建议接地后使用     ")), 1);
    } else {
        //
    }
    if (acsw_board) {
        if (board_state[140] == 0) {
            //
        } else if (board_state[140] == 100) {
            All_OK++;
            Sigmain_data_to_syst(QString(tr("         电源切换板异常！        \n" \
                                            "B0BA 异常     ")), 1);
        } else {
            All_OK++;
        }
    } else {
        //
    }

    if (Ini_WD == QString(tr("WD"))) {
        if (board_state[190] == 0) {
            //
        } else if (board_state[190] == 100) {
            All_OK++;
            Sigmain_data_to_syst(QString(tr("         WD板异常！        \n:"\
                                            "B09A 异常     ")), 1);
        } else {
            All_OK++;
        }
    } else {
        //
    }
    if (Ini_IO) {
        if (board_state[210] == 17) {
            //
        } else if (board_state[210] == 100) {
            All_OK++;
            Sigmain_data_to_syst(QString(tr("         IO板异常！        \n" \
                                            "B0DA 异常     ")), 1);
        } else {
            //
        }
    } else {
        //
    }

    if (All_OK == 0) {
        Sigmain_data_to_syst(QString(tr("       全部正常      ")), 1);
    } else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      主页面查询板子的状态
******************************************************************************/
void w_Main::Pubs_Cheak_Main_Finsh()
{
    int i;
    int Error_Flag = 0;
    char str[30];
    QString string = "";
    int Check_CurrentProj = 0;
    int Check_PWR_L = 0;

    ui->Print_Text->hide();
    ui->label_4->hide();
    for (i = 0; i < Ini_Proj_Real.size(); i++) {
        if (i <= 5) {
            if (Ini_Proj_Real.at(i).toInt() != 0) {
                if (board_state[i*2] == 0) { //  输出板
                    sprintf(str, "System OUT %d Ok", 13 + i);
                    string = str;  ui->Print_Text->append(string); string.clear();
                    ui->Print_Text->append(QString(Version.OutV[i])+"  ");
                    Pri_Can_Set_OUTModel(0x13 + i, system_conf.at(4).toInt());
                } else if (board_state[i*2] == 100) {
                    Error_Flag = i+13;
                    sprintf(str, "System OUT %d Can communication Error", 13+i);
                    string = str;  ui->Print_Text->append(string);  string.clear();
                } else {
                    Error_Flag = i+13;
                    sprintf(str, "System OUT %d Error", 13+i);
                    string = str;  ui->Print_Text->append(string);  string.clear();
                }
            }
        } else if (Ini_Proj_Real.at(i).toInt() <= 2) {
            if (Check_CurrentProj != 2) {
                if (board_state[20] == 0) { //  电阻板
                    ui->Print_Text->append("System DLR Ok");
                    ui->Print_Text->append("System DLR Version");
                    ui->Print_Text->append(Version.DLRV+"  ");
                } else if (board_state[20] == 100) {
                    Error_Flag = 2;
                    ui->Print_Text->append("System DLR Can communication Error");
                } else {
                    Error_Flag = 2;
                    ui->Print_Text->append("System DLR Error");
                    ui->Print_Text->append(QString::number(board_state[20]));
                }
                Check_CurrentProj = 2;
            }
        } else if (Ini_Proj_Real.at(i).toInt() <= 5) {
            if (Check_CurrentProj != 3) {
                if (board_state[40] == 0) { //  绝缘板
                    ui->Print_Text->append("System IR Ok");
                    ui->Print_Text->append("System IR Version");
                    ui->Print_Text->append(Version.IWV+"  ");
                } else if (board_state[40] == 100) {
                    Error_Flag = 3;
                    ui->Print_Text->append("System IR Can communication Error");
                } else {
                    Error_Flag = 3;
                    ui->Print_Text->append("System IR Error");
                    ui->Print_Text->append(QString::number(board_state[40]));
                    ui->Print_Text->append("System IR Error Value");
                    ui->Print_Text->append(QString::number(board_state[50]));
                }
                Check_CurrentProj = 3;
            }
        } else if (Ini_Proj_Real.at(i).toInt() <= 6) {
            if (Check_CurrentProj != 4) {
                if (board_state[60] == 0) { //  匝间板
                    ui->Print_Text->append("System IMP Ok");
                    ui->Print_Text->append("System IMP Version");
                    ui->Print_Text->append(Version.IMPV+"  ");
                } else if (board_state[60] == 100) {
                    Error_Flag = 4;
                    ui->Print_Text->append("System IMP Can communication Error");
                } else {
                    Error_Flag = 4;
                    ui->Print_Text->append("System IMP Error");
                    ui->Print_Text->append(QString::number(board_state[60]));
                    ui->Print_Text->append("System IMP Error Value");
                    ui->Print_Text->append(QString::number(board_state[70]));
                }
                Check_CurrentProj = 4;
            }
        }  else if ((Ini_Proj_Real.at(i).toInt() == 7) || (Ini_Proj_Real.at(i).toInt() == 9) || \
                 (Ini_Proj_Real.at(i).toInt() == 10)) { // 电参板
            if (Check_PWR_L != 5) {
                if (board_state[80] == 0) {
                    ui->Print_Text->append("System PWR Ok");
                    ui->Print_Text->append("System PWR Version");
                    ui->Print_Text->append(Version.PWR_LV+"  ");
                } else if (board_state[80] == 100) {
                    Error_Flag = 5;
                    ui->Print_Text->append("System PWR Can communication Error");
                } else {
                    Error_Flag = 5;
                    ui->Print_Text->append("System PWR Error");
                    ui->Print_Text->append(QString::number(board_state[80]));
                    ui->Print_Text->append("System PWR Error Value");
                    ui->Print_Text->append(QString::number(board_state[90]));
                }
                Check_PWR_L = 5;
            } else {
                //
            }
        }  else if (Ini_Proj_Real.at(i).toInt() <= 8) { //  电感板
            if (Check_CurrentProj != 7) {
                if ((board_state[100] == 0) || (board_state[100] == 3)) {
                    ui->Print_Text->append("System INDL Ok");
                    ui->Print_Text->append("System INDL Version");
                    ui->Print_Text->append(Version.INDLV+"  ");
                } else if (board_state[100] == 100) {   Error_Flag=7;
                    ui->Print_Text->append("System INDL Can communication Error");
                } else {
                    Error_Flag = 7;
                    ui->Print_Text->append("System INDL Error");
                    ui->Print_Text->append(QString::number(board_state[100]));
                    ui->Print_Text->append("System INDL Error Value");
                    ui->Print_Text->append(QString::number(board_state[110]));
                }
                Check_CurrentProj = 7;
            }
        } else if (Ini_Proj_Real.at(i).toInt() <= 11) { //  PG板
            if (Check_CurrentProj != 8) {
                if ((board_state[170] == 0) || (board_state[170] == 10)) {
                    ui->Print_Text->append("System PG Ok");
                    ui->Print_Text->append("System PG Version");
                    ui->Print_Text->append(Version.VBLDCPG+"  ");
                } else if (board_state[170] == 100) {
                    Error_Flag = 8;
                    ui->Print_Text->append("System PG Can communication Error");
                } else {
                    Error_Flag = 8;
                    ui->Print_Text->append("System PG Error");
                    ui->Print_Text->append("System PG Error Value");
                }
                Check_CurrentProj = 8;
            }
        } else {
            //
        }
        if (Error_Flag != 0) {
            switch (Error_Flag) {
            case 1:
                Pri_Main_WMessage(0);
                break;
            case 2:
                Pri_Main_WMessage(1);
                break;
            case 3:
                Pri_Main_WMessage(2);
                break;
            case 4:
                Pri_Main_WMessage(3);
                break;
            case 5:
                if (board_state[80] != 0) {
                    Pri_Main_WMessage(4);
                } else {
                    //
                }
                break;
            case 7:
                Pri_Main_WMessage(5);
                break;
            case 8:
                Pri_Main_WMessage(12);
                break;
            default:
                if (Error_Flag == 13) {
                    Pri_Main_WMessage(6);
                } else if (Error_Flag == 14) {
                    Pri_Main_WMessage(7);
                } else {
                    //
                }
                break;
            }
            Error_Flag = 0;
        }
    }
    if (nine_board) {
        if (board_state[120] == 0) {
            ui->Print_Text->append("System AMP Ok");
            ui->Print_Text->append("System AMP Version");
            ui->Print_Text->append(Version.AMP+"  ");
        } else if (board_state[120] == 100) {
            ui->Print_Text->append("System AMP Can communication Error");
        } else {
            ui->Print_Text->append("System AMP Error");
            ui->Print_Text->append(QString::number(board_state[120]));
        }
    } else {
        //
    }
    if ((nine_board) && (!GND_State)) { // 判断为9插板进行，接地的检查
        Pri_Main_WMessage(8);
    } else {
        //
    }
    if (acsw_board) {
        if (board_state[140] == 0) {
            ui->Print_Text->append("System ACSW Ok");
            ui->Print_Text->append("System ACSW Version");
            ui->Print_Text->append(Version.ACSW+"  ");
        } else if (board_state[140] == 100) {
            ui->Print_Text->append("System ACSW Can communication Error");
            Pri_Main_WMessage(9);
        } else {
            //
        }
    } else {
        //
    }
    if (Ini_WD == QString(tr("WD"))) {
        if (board_state[190] == 0) {
            ui->Print_Text->append("System WD Ok");
            ui->Print_Text->append("System WD Version");
            ui->Print_Text->append(Version.BLDCWD+"  ");
        } else if (board_state[190] == 100) {
            ui->Print_Text->append("System WD Can communication Error");
            Pri_Main_WMessage(11);
        } else {
            ui->Print_Text->append("System WD Error");
            ui->Print_Text->append("System WD Error Value");
        }
    }
    if (Ini_IO) {
        if (board_state[210] ==  17) {
            ui->Print_Text->append("System IO Ok");
            ui->Print_Text->append("System IO Version");
            ui->Print_Text->append(Version.IO+"  ");
        } else if (board_state[210] == 100) {
            ui->Print_Text->append("System IO Can communication Error");
            Pri_Main_WMessage(13);
        } else {
            ui->Print_Text->append("System IO Error");
            ui->Print_Text->append("System IO Error Value");
        }
    } else {
        //
    }
    ui->Print_Text->append("System Ok");
    qApp->processEvents();
    Sigmain_save_DayRecoord("Check", \
                            QString(ui->Print_Text->toPlainText().mid(503)).replace \
                            (QRegExp("\\s{2,}"), "，").replace(QRegExp("\\s{1,}"), " "));
    Sigmain_data_to_debug(QStringList(ui->Print_Text->toPlainText()), QString(""), 4);

    ui->label_2->hide();
    ui->Key_A->show();
    ui->Key_B->show();
    ui->Key_C->show();
    ui->Key_D->show();
    ui->label_Version->show();

    WriteMessage((6000), (0x90), QString("").toUtf8());
    QTimer *Time_Jump = new QTimer(this);
    connect(Time_Jump, SIGNAL(timeout()), this, SLOT(Jump_wTest_Surface()));
    Time_Jump->setSingleShot(TRUE);
    Time_Jump->start(1000);
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.6.7
      * brief:      跳转到Test界面
******************************************************************************/
void w_Main::Jump_wTest_Surface()
{
    if ((Ini_Vacuum_Model) || (wTest->START_Model == Start_model_Net)) {
        return;
    } else {
        //
    }
    if (window_current != wMain_Surface) { //  判断当前页面为主页面
        return;
    } else {
        //
    }
    Pubs_ChangeWindow(4);
    wConf->Pub_main_to_conf(QStringList(""), QString(""), 1, 7);
}

void w_Main::Pubs_UDisk_Data(int a, QString USB) {
    Sigmain_save_DayRecoord("USB", " ");
    wData->UDisk_Test(a, USB);
}

/******************************************************************************
      * version:    1.0
      * author:     link
      * date:       2015.12.10
      * brief:      系统后台功能/CAN总线接收
******************************************************************************/
void w_Main::System_Runing()
{
    can_bug_time++;
    qint8 myComi; //初始化界面
    switch (step_system) {
    case 0:
        ui->Print_Text->append("0-Syetem-set:");
        ui->Print_Text->append("portnoo=tttyS3;");
        ui->Print_Text->append("m_baudRate=115200;");
        myCom  = new CSerial;
        myComi = myCom ->OpenPort(QString("ttyS3").toLatin1().data(), 115200, '8', '1', 'N');
        if (myComi < 0) {
            delete myCom;
            myCom = NULL;
            return;
        } else {
            ui->Print_Text->append("Open Usart success;");
        }

        ui->Print_Text->append("can.canOpen(500000);");
        can.canOpen(500000); // -运行Can设置，进行配置,打开CAN口

        wSyst = new w_Syst;
        connect(wSyst, SIGNAL(signal_to_main(QStringList, int, int)), \
                this, SLOT(Pubs_from_syst(QStringList, int, int)));
        connect(this, SIGNAL(Sigmain_data_to_syst(QString, int)), \
                wSyst, SLOT(slot_from_main(QString, int)));

        wDebug = new w_Debug;
        connect(wDebug, SIGNAL(Singal_debug_to_main(QStringList, int, int)), \
                this, SLOT(Pubs_from_debug(QStringList, int, int)));
        connect(wDebug, SIGNAL(canSend(can_frame, int)), \
                this, SLOT(canSend(can_frame, int)));
        connect(this, SIGNAL(Sigmain_data_to_debug(QStringList, QString, int)), \
                wDebug, SLOT(Slot_from_Main(QStringList, QString, int)));


        for (int j = 0; j < 220; j++) { //-初始化记录数值
            board_state[j] = 100;
        }
        wPassWord = new PassWord;
        connect(wPassWord, SIGNAL(Singal_pass_to_main(int, int)), \
                this, SLOT(Pubs_from_pass(int, int)));
        step_system++;
        break;
    case 1:
        ui->Print_Text->append("1-Syetem-set:");
        step_system++;
        break;
    case 2:
        ui->Print_Text->append("2-Syetem-set:");
        wData = new w_Data;
        connect(wData, SIGNAL(Signal_Data_to_Main(QStringList, int, int)), \
                this, SLOT(Pubs_from_data(QStringList, int, int)));
        connect(myThread, SIGNAL(Singal_U_Test(int, QString)), \
                this, SLOT(Pubs_UDisk_Data(int, QString)));
        connect(MYSQL_DB, SIGNAL(Singal_Data_Scale(QStringList, int, int)), \
                wData, SLOT(pubslot_display_scale(QStringList, int, int)));
        connect(this, SIGNAL(Singal_to_SQL(QString, QStringList, int)), \
                MYSQL_DB, SLOT(Slot_Data_to_SQLDB(QString, QStringList, int)), \
                Qt::QueuedConnection);

        step_system++;
        break;
    case 3:
        ui->Print_Text->append("3-Syetem-set:");
        ui->Print_Text->append("System Initialization Test!");
        wTest = new w_Test;
        connect(wTest, SIGNAL(canSend(can_frame, int)), \
                this, SLOT(canSend(can_frame, int)));
        connect(wTest, SIGNAL(Signal_Test_to_Main(int, int)), \
                this, SLOT(Pubs_from_test(int, int)));
        connect(wTest, SIGNAL(Sigtest_Usart_Send(char *)), \
                this, SLOT(Pubs_Usart_Send(char *)));
        connect(this, SIGNAL(Sigmain_data_to_test(QStringList)), \
                wTest, SLOT(join_updatedata(QStringList)));

        step_system++;
        break;
    case 4:
        ui->Print_Text->append("4-Syetem-set:");
        ui->Print_Text->append("System Initialization Test!");
        wHelp = new w_Help;
        connect(wHelp, SIGNAL(sendData(int)), \
                this, SLOT(Pubs_ChangeWindow(int)));
        step_system++;
        break;
    case 5:
        ui->Print_Text->append("5-Syetem-set:");
        ui->Print_Text->append("System Initialization Test!");
        step_system++;
        break;
    case 6:
        ui->Print_Text->append("6-Syetem-set:");
        ui->Print_Text->append("System Initialization Test!");
        ui->Print_Text->append("w_Conf Sql Connecting");
        wConf = new w_Conf;
        connect(this, SIGNAL(Sigmain_save_DayRecoord(QString, QString)), \
                wConf, SLOT(Slot_Save_DayRecoord(QString, QString)));
        connect(wDebug, SIGNAL(Singal_Save_Debug(QString, QString)), \
                wConf, SLOT(Slot_Save_Debug(QString, QString)));
        connect(wTest, SIGNAL(Sigtest_Save_Data(QStringList, QString, int)), \
                wConf, SLOT(Slot_test_to_conf(QStringList, QString, int)));
        connect(wConf, SIGNAL(Singal_Conf_to_Main(QStringList, int, int)), \
                this, SLOT(Pubs_from_conf(QStringList, int, int)));
        connect(wConf, SIGNAL(canSend(can_frame, int)), \
                this, SLOT(canSend(can_frame, int)));
        connect(this, SIGNAL(Sigmain_can_to_conf(can_frame, QStringList, int)), \
                wConf, SLOT(SlOT_can_to_conf(can_frame, QStringList, int)));

        wConf->Pub_main_to_conf(QStringList(""), QString(""), 1, 5);
        step_system++;
        ui->Print_Text->append("w_Conf Sql Connect Success");
        break;
    case 7:
        ui->Print_Text->append("7-Syetem-set:");
        ui->Print_Text->append("System Initialization Test!");
        wColr = new w_Colr;
        connect(wColr, SIGNAL(Signal_color_to_Main(QString, int, int)), \
                this, SLOT(Pubs_from_color(QString, int, int)));
        step_system++;
        break;
    case 8:
        ui->Print_Text->append("8-Syetem-set:");
        ui->Print_Text->append("System Initialization Test!");
        step_system++;
        break;
    case 9:
    {
        ui->Print_Text->append("9-Syetem-set:");
        ui->Print_Text->append("System Initialization Complete!");

        step_system++;
        Pri_Synchron_Syst_Data();
        Pri_Synchron_PassWord_Data();
        Pri_TcpInit();
        Pri_availableDisk();

        wpic = new pic;
        PIC_Connect = true;
        connect(this, SIGNAL(Signal_main_to_pic(QStringList, int, int)), \
                wpic, SLOT(Pubs_from_main(QStringList, int, int)));
        connect(wTest, SIGNAL(Signal_test_to_pic(QStringList, int, int)), \
                wpic, SLOT(Pubs_from_main(QStringList, int, int)));
        connect(this, SIGNAL(Signal_main_to_Switchover(int, int, int)), \
                wpic, SLOT(Pubs_from_Switchover(int, int, int)));
        connect(wTest, SIGNAL(Signal_test_to_Switchover(int, int, int)), \
                wpic, SLOT(Pubs_from_Switchover(int, int, int)));

        connect(wpic, SIGNAL(canSend(can_frame, int)), \
                this, SLOT(canSend(can_frame, int)));
        connect(wpic, SIGNAL(Signal_pic_to_Test(QString, int, int)), \
                wTest, SLOT(Pubs_from_pic(QString, int, int)));
        connect(wpic, SIGNAL(Signal_pic_to_Main(QString, int, int)), \
                this, SLOT(Pubs_from_pic(QString, int, int)));
        Signal_main_to_pic(QStringList(""), Ini_IO_Model, 119); // 控制气缸
        Signal_main_to_pic(QStringList(""), Ini_Vacuum_Model, 0);

        QTimer *Time_Cheak_Ready = new QTimer(this);
        connect(Time_Cheak_Ready, SIGNAL(timeout()), \
                this, SLOT(Pubs_Cheak_Ready()));
        Time_Cheak_Ready->setSingleShot(TRUE);
        Time_Cheak_Ready->start(50);
        break;
    }
    default:
        wTest->Pub_Main_to_test(QStringList(""), QString(""), can_bug_time, 1);
        while (can.canRecv()) { // -Can接收数据
            Pri_can_data_analytical();
        }
        while (myCom->ReceiveThreadFunc()) {
            Pri_usart_analytical();
        }
        break;
    }
    if (Shake_hand_Success) {
        Shake_hand_runcount++;
        if (Shake_hand_runcount <= 60) { // 计时3s
            return;
        } else {
            //
        }
        Shake_hand_runcount = 0;
        WriteMessage((6000), (0x22), QString(tr("Reply")).toUtf8()); // 进行心跳响应
    } else {
        //
    }
}


void w_Main::Pubs_from_pic(QString string, int value, int flag) {
    switch (flag) {
    case 1:
        if (window_current == wTest_Surface) {
            qDebug() << "Pubs_from_pic---1";
            out_station = value;
            wTest->Pub_Main_to_test(QStringList(""), "0", value, 11);
        } else {
            //
        }
        break;
    case 2:
        if (window_current == wTest_Surface) {
            qDebug() << "1111";
            wTest->Pub_Main_to_test(QStringList(""), "0", 1, 12);
            Beep_PWM_Stop();
        } else {
            //
        }
        break;
    case 3:
        if (window_current == wConf_Surface) {
            wConf->Pub_main_to_conf(QStringList(""), QString(""), 19, 19);
            wConf->Pub_main_to_conf(QStringList(""), QString(""), 24, 2);
        } else {
            //
        }
        break;
    case 5:
        if (window_current == wConf_Surface) {
            wConf->Pub_main_to_conf(QStringList(""), QString(""), 20, 19);
            wConf->Pub_main_to_conf(QStringList(""), QString(""), 24, 2);
        } else {
            //
        }
        break;
    case 6:
        WriteMessage(6000, 0x96, QString::number(value).toUtf8());
        break;
    case 7:
        Pri_Main_WMessage(value);
        break;
    case 8:
        Out_Board_Stop();
        usleep(500);
        frame.can_id = 0x22;  frame.can_dlc = 0x02; //-发送给电阻板
        frame.data[0] = 0x09;
        frame.data[1] = 0x00;
        canSend(frame, 88);
        usleep(500);
        dcr_board->stop();
        break;
    case 9:

        break;
    default:

        break;
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      系统接收的can信息进行解析
******************************************************************************/
void w_Main::Pri_can_data_analytical()
{
    int i, Id;
    can_bug_time = 0;
    Id = can.frame.can_id & 0xffff; can.frame.can_id = 0; // -ID
 // Pri_Can_Receive(Id);
    switch (Id) {
    case CAN_DLR:       // -电阻板测试系统
    //    Pri_Can_Receive(Id);
        Pri_Board_DLR();
        break;
    case CAN_DLR_SHAPE: // -反嵌波形
    //    Pri_Can_Receive(Id);
        mag_waveform_error = true;           // -初始设置为错误，在出现错误时显示一条横线；
        for (i = 0; i < can.frame.can_dlc; i++) { // -反嵌測試項目
            waveform_point[waveform_count] = can.frame.data[i];
            waveform_count++;
            if (waveform_count == 400) {
                mag_waveform_error = false;
                waveform_count = 399;
            } else {
                //
            }
        }
        break;
    case CAN_IR:       // -绝缘交耐板测试系统
        Pri_Can_Receive(Id);
        Pri_Board_IR_ACW();
        break;
    case CAN_ANP:      // -功放板测试系统
        Pri_Board_AMP();
        break;
    case CAN_IW:       // -匝间板测试系统
   //     Pri_Can_Receive(Id);
        Pri_Board_IMP();
        break;
    case CAN_IW_SHAPE: // -匝间板波形
        if (window_current == wDebug_Surface) {
            return;
        } else {
            //
        }
        imp_waveform_error = true;               // -初始设置为错误，在出现错误时显示一条横线；
        for (i = 0; i < (can.frame.can_dlc/2); i++) { // -匝间測試項目
            waveform_point[waveform_count] = can.frame.data[i*2]*256 + can.frame.data[i*2 + 1];
            waveform_count++;
            if (waveform_count == 400) {
                imp_waveform_error = false;
                waveform_count = 399;
            } else {
                //
            }
        }
        break;
    case CAN_PWR1:      // -电参测试系统
        Pri_Board_PWR(Id);
        break;
    case CAN_PWR2:      // -电参测试系统
        Pri_Board_PWR(Id);
        break;
    case CAN_PWR_SHAPE: // -PG波形的接收
        pg_waveform_error = true;
        for (i = 0; i < can.frame.can_dlc; i++) { // -PG波形进行接收
            waveform_point[waveform_count] = can.frame.data[i];
            waveform_count++;
            if (waveform_count == 400) {
                pg_waveform_error = false;
                waveform_count = 399;
            } else {
                //
            }
        }
        break;
    case CAN_INDL: //-电感测试系统
        Pri_Board_INDL();
        break;
    case CAN_ACSW: //-电源切换板
        Pri_Board_ACSW();
        break;
    case CAN_WD:
        Pri_Can_Receive(Id);
        Pri_Board_WD();
        break;
    case CAN_PG:
        Pri_Can_Receive(Id);
        Pri_Board_PG();
        break;
    case CAN_IO:
        Pri_Can_Receive(Id);
        Pri_Board_IO();
        break;
    case CAN_PG_SHAPE:
        Pri_Board_PG_Wave();
     /*   fg_waveform_error = true;
        for (i = 0; i < can.frame.can_dlc; i++) { // -PG波形进行接收
            waveform_point[waveform_count] = can.frame.data[i];
            waveform_count++;
            if (waveform_count == 400) {
                fg_waveform_error = false;
                waveform_count = 399;
            } else {
                //
            }
        }*/
        break;
    default: //-输出板或者其他
        if ((Id >= CAN_13OUT) && (Id <= CAN_17OUT)) { // -输出板（13-17）测试系统
            Pri_Board_Out(((Id-CAN_13OUT)/32), 0X13 + ((Id - CAN_13OUT)/32));
        } else {
            //
        }
        break;
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      系统接收的Usart信息进行解析
******************************************************************************/
void w_Main::Pri_usart_analytical()
{
    QString USART = QString(myCom->DatBuf).mid(1, myCom->m_DatLen-3); //  接收的字符串
    if (USART == "1") {
        if (window_current == wTest_Surface) {
//                    frame.can_id  = 0x2D;  frame.can_dlc = 0x04;
//                    frame.data[0] = 0xf1;
//                    frame.data[1] = 0x00;
//                    frame.data[2] = 0x00;
//                    frame.data[3] = 0x00;
//                    canSend(frame, 88);
            wTest->Pub_Main_to_test(QStringList(""), "0", 1, 12);
            Beep_PWM_Stop();
        } else if (window_current == wConf_Surface) {
            Button_Main_Group_Judge(Qt::Key_D);
        } else {
            //
        }
    } else if ((USART == "2")&&(window_current == wTest_Surface)) {
        out_station = 0x13;
        wTest->Pub_Main_to_test(QStringList(""), "0", 0x13, 11);
    } else if ((USART == "18") && (wTest->Back_Test_Status() == Test_Station_Free)) { // 空闲状态
        if (window_old != wPassWord_Surface) { // 密码禁止进入帮助
            Pubs_ChangeWindow(5);
        } else {
            //
        }
    } else if (USART == "17") {
        calibration_count++;
        if (calibration_count == 10) {
            system("./ts_calibrate -qws&");
        } else {
            //
        }
    } else if (window_current == wConf_Surface) { //  配置界面
        Pri_KEY_ConfSurface(USART);
    } else if (window_current == wTest_Surface) {  //  测试界面
        wTest->Pub_Main_to_test(QStringList(""), USART, 1, 2);
    } else if (window_current == wMain_Surface) {  //  测试界面
        Pri_KEY_MainSurface(USART);
    } else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      主页面按键操作
******************************************************************************/
void w_Main::Pri_KEY_MainSurface(QString USART)
{
    switch (USART.toInt()) {
    case 26:  // F1
        Button_Main_Group_Judge(Qt::Key_A);
        break;
    case 27:  // F2
        Button_Main_Group_Judge(Qt::Key_B);
        break;
    case 24:  // F3
        Button_Main_Group_Judge(Qt::Key_C);
        break;
    case 25:  // F4
        Button_Main_Group_Judge(Qt::Key_D);
        break;
    default:
        break;
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      配置界面按键操作
******************************************************************************/
void w_Main::Pri_KEY_ConfSurface(QString USART)
{
    int  key_table[] = {10, 10, 10, 1, 2, 3, 10, 4, 5, 6, 10, 7, 8, 9, 10, 0, 10};
    //-机器界面按键的数值
    if (USART == "6") {        // 退格
        Serial_SendBackspace();
    } else if (USART == "14") {  // 确定
        Serial_SendEnter();
        Serial_SendEscape();
    } else if (USART == "16") {
        Serial_SendString(".");
    } else if (USART == "23") {
        Serial_Upspace();
    } else if (USART == "22") {
        Serial_Downspace();
    } else if (USART == "20") {
        Serial_Tabspace();
    } else if (USART == "19") {
        Serial_BackTabspace();
    } else if (USART == "21") {
        wConf->Pub_main_to_conf(QStringList(""), QString(""), 1, 1);
    } else if ((USART.toInt() >= 24) && (USART.toInt() <= 27)) {
        wConf->Pub_main_to_conf(QStringList(""), QString(""), USART.toInt(), 2);
    } else if (key_table[USART.toInt()] < 10) {
        Serial_SendString(QString::number(key_table[USART.toInt()]));
    } else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.5.20
      * brief:      输出板 状态检测和启动测试和版本号
******************************************************************************/
void w_Main::Pri_Board_Out(int count, int Station)
{
    if (can.frame.data[0] == CAN_UPLOAD_STATE) {          //-上传状态
        board_state[count*2] = can.frame.data[1];
    } else if (can.frame.data[0] == CAN_UPLOAD_RESULT_ONE) { //-上传结果
        if (can.frame.data[1] == Receive_Stop) {
            if (window_current == wTest_Surface) {        //-测试界面
                Signal_main_to_pic(QStringList(""), Station*10 + 0, 11);
                WriteMessage((6000), (0x98), QString::number(out_station).toUtf8());
                wTest->Pub_Main_to_test(QStringList(""), "0", 2, 12);

                if ((wTest->START_Model == 2) && (((PreCount == 0xff) || (PreCount == count)))) {
                    // -滑罩启动
                    isTesting = false;
                } else {
                    //
                }
            } else if (window_current == wConf_Surface) {
                wConf->Pub_main_to_conf(QStringList(""), QString(""), 1, 19);
            } else {
                //
            }
        } else if (can.frame.data[1] == Receive_Start) {             // -拉上滑罩
            if ((window_current == wTest_Surface) && (!isTesting)) { // -测试界面,已经拉开滑罩
                Signal_main_to_pic(QStringList(""), Station*10+1, 11);
                out_station = Station;
                // -更改顺序，在大疆无人机出现的问题-2017.5.16

                wTest->Pub_Main_to_test(QStringList(""), \
                                        QString::number(wTest->START_Model), Station, 11);
                WriteMessage((6000), (0x97), QString::number(out_station).toUtf8());
                if (wTest->START_Model == 2) { //-滑罩启动
                    isTesting = true;
                    PreCount  = count;
                } else {
                    //
                }
            } else if (window_current == wConf_Surface) {
                wConf->Pub_main_to_conf(QStringList(""), QString(""), Station, 19);
            } else {
                //
            }
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_Version) { // 上传版本号
        Version.OutV[count] = Pri_Board_Version();
    } else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.6.27
      * brief:      DLR板获取结果参数
******************************************************************************/
QStringList w_Main::Pri_Board_DLR_TestData(can_frame canframe)
{
    QStringList Back_QStringList; Back_QStringList.clear();
    if ((canframe.data[2] == 1) || (canframe.data[2] == 2)) { // -档位（1，2）-"mΩ"
        Back_QStringList.append(QString::number((canframe.data[3] * 256 + canframe.data[4]) \
                * (qPow(10, -(6-canframe.data[2]))) * 1000));  //
        Back_QStringList.append("mΩ");
    } else if ((canframe.data[2] == 3) || (canframe.data[2] == 4) || (canframe.data[2] == 5)) {
        // -档位（3，4，5）-"Ω"
        Back_QStringList.append(QString::number((canframe.data[3] * 256 + canframe.data[4]) \
                * (qPow(10, -(6-canframe.data[2])))));  //
        Back_QStringList.append("Ω");
    } else if ((canframe.data[2] == 6) || (canframe.data[2] == 7) || (canframe.data[2] == 8)) {
        // -档位（6，7，8）-"KΩ" 其中（7，8）暂时未用到
        Back_QStringList.append(QString::number((canframe.data[3] * 256 + canframe.data[4]) \
                * (qPow(10, (canframe.data[2]-6))) / 1000));  //
        Back_QStringList.append("KΩ");
    } else {
        //
    }
    Back_QStringList.append(QString::number(canframe.data[1]));
    if (canframe.data[2] < 6) {
        Back_QStringList.append(QString::number((canframe.data[3] * 256 + canframe.data[4]) \
                * (qPow(10, -(6-canframe.data[2])))));
    } else {
        Back_QStringList.append(QString::number((canframe.data[3]*256+canframe.data[4]) \
                * (qPow(10, (canframe.data[2]-6)))));
    }
    Back_QStringList.append(QString::number(canframe.data[2]));
    return Back_QStringList;
}
void  w_Main::Pri_Mag_Trend(int *waveform_point) {
    /*int i;
    QStringList data;
    data.append("0");
    for (i = 0; i < 399; i++) {
        if (waveform_point[i+1] > waveform_point[i]) {
            data.append("1");
        } else if(waveform_point[i+1] < waveform_point[i]) {
            data.append("0");
        } else {
            data.append(data.at(i));
        }
    }
    qDebug() << "data" << data.size() << data;
    for (i = 1; i < 398; i++) {
        if (data.at(i) != data.at(i + 1)) {
            data.replace(i + 1, data.at(i));
            i = i + 1;
        } else {

        }
    }*/ //測試趨勢
    int i;

    int H_data,L_data;
    H_data = 0;
    L_data = 0;

    int zero_count;
    zero_count = 0;

    QStringList zero_list;
    zero_list.clear();

    for (i = 0; i < 400; i++) {
        if ((waveform_point[i]) >= 125) {
            if (H_data == 0) {
                zero_list.append(QString::number(2000 + L_data));
            } else {
                //
            }
            H_data++;
            L_data = 0;
        } else {
            if (L_data == 0) {
                zero_list.append(QString::number(1000 + H_data));
            } else {
                //
            }
            L_data++;
            H_data = 0;
        }
    }
    zero_list.append(QString::number(2000 + L_data));
    zero_list.append(QString::number(1000 + H_data));

    QStringList zero_list_update;
    for (i = 0; i < zero_list.size(); i++) {
        if (zero_list.at(i).toInt()%1000 <= 10) {
            //
        }
        else {
            zero_list_update.append(zero_list.at(i));
        }
    }

    if (zero_list_update.size() == 1) {
        qDebug() << "zero_count" << "0";
        return;
    } else {
        //
    }

    for (i = 0; i < zero_list_update.size() - 1; i++) {
        if (zero_list_update.at(i).toInt()/1000 != \
                zero_list_update.at(i + 1).toInt()/1000) {
            zero_count++;
        } else {
            //
        }
    }
    qDebug() << "zero_count" << zero_count;
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      DLR板
******************************************************************************/
void w_Main::Pri_Board_DLR()
{
    int i;
    QStringList Net_Shape_List;
    if (can.frame.data[0] == CAN_UPLOAD_STATE) {
        double Temp_Get = ((double)(can.frame.data[2]*256 + can.frame.data[3]))/10 - 50.0;
        wTest->Pub_Main_to_test(QStringList(""), QString::number(Temp_Get), 1, 5);
        board_state[20] = can.frame.data[1];
        if (window_current == wTest_Surface) {
            wTest->Pub_Main_to_test(QStringList(""), QString(""), 1 , 19);
        } else if (window_current == wConf_Surface) {
            wConf->Board_DLR = 1;     //  可以进行测试
            if ((Ini_MAG_Device) && (can.frame.data[1] == 0)) {
                qDebug() << "测试状态 OK";
                Signal_main_to_pic(QStringList(""), 0, 9);
            } else {
                //
            }
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_RESULT_ONE) { //-电阻测试结果
        QStringList DLR_Can_Data;
        if (window_current == wTest_Surface) {
            Sigmain_data_to_test(wConf->WDLR->DCR_Test_Result(can.frame, Back_out_station));
        } else if (window_current == wConf_Surface) {
            Sigmain_can_to_conf(can.frame, QStringList(""), 6);
        } else if (window_current == wDebug_Surface) {
            DLR_Can_Data = Pri_Board_DLR_TestData(can.frame);
            Sigmain_data_to_debug(QStringList(""), \
                                  QString::number(can.frame.data[3]*256+can.frame.data[4]), 2);
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_RESULT_TWO) {
        mag_back_number = can.frame.data[1];
        if (window_current == wTest_Surface) { // 显示在ABC显示区
            mag_str = QString::number(wConf->WMAG->Test_MagData(2, Back_out_station, can.frame));
        } else if (window_current == wConf_Surface) { // 显示在显示区
            wConf->WMAG->Test_MagData(1, 1, can.frame);
            Net_Shape_List.clear();
            Net_Shape_List.append(QString::number(mag_back_number));
            for (i = 0; i < 5; i++) {
                Net_Shape_List.append(QString::number(can.frame.data[i]));
            }
            emit WriteMessage(6000, 0x87, QString(Net_Shape_List.join(" ")).toUtf8());
        } else {
            //
        }
        waveform_count = 0;
    } else if (can.frame.data[0] == CAN_UPLOAD_SHAPE) {
        if (can.frame.data[1] != 255) {
            return;
        } else {
            //
        }
        if (window_current == wConf_Surface) {
            wConf->WMAG->MagDir_Conf(mag_back_number, mag_set_rotation, waveform_point);
            waveform_count = 0;
            Net_Shape_List.clear();
            Net_Shape_List.append(QString::number(mag_back_number));
            for (i = 0; i < 400; i++) {
                Net_Shape_List.append(QString::number(waveform_point[i]));
            }

            emit WriteMessage(6000, 0x81, QString(Net_Shape_List.join(" ")).toUtf8());
        } else {
            //
        }
        if (window_current == wTest_Surface) {
            QStringList mag_data; mag_data.clear();
            mag_data.append(QString::number(mag_waveform_error));
            mag_data.append(QString::number(mag_back_number));
            mag_data.append(QString::number(mag_set_rotation));
            Net_Shape_List.clear();
            Net_Shape_List.append(QString::number(mag_back_number));
            for (i = 0; i < 400; i++) {
                Net_Shape_List.append(QString::number(waveform_point[i]));
            }
            Pri_Mag_Trend(waveform_point); // 判斷趨勢
            emit WriteMessage(6000, 0x82, QString(Net_Shape_List.join(" ")).toUtf8());
            wTest->Pub_MAG_Param(mag_data, waveform_point);
            QStringList mag_result;
            mag_result.clear();
            mag_result.append(mag_str);
            Sigmain_data_to_test(mag_result);
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_Version) {
        Version.DLRV = Pri_Board_Version();
    } else if (can.frame.data[0] == CAN_UPLOAD_Debug) {
        if (window_current != wDebug_Surface) {
            return;
        } else {
            //
        }
        QStringList list;
        list.append(QString::number(can.frame.data[1]));
        list.append(QString::number(can.frame.data[2]));
        list.append(QString::number(can.frame.data[3]));
        Sigmain_data_to_debug(list, QString(""), 7);
    } else if ((can.frame.data[0] == CAN_UPLOAD_In) && ((window_current == wTest_Surface))) {
        if (signal_model == 1) { //-重测和继续
            if (can.frame.data[1] == 1) { //-重测
                wTest->Pub_Main_to_test(QStringList(""), "0", 1, 6);
            } else if (can.frame.data[2] == 1) { //-继续
                wTest->Pub_Main_to_test(QStringList(""), "0", 1, 15);
            } else {
                //
            }
        } else if (signal_model == 2) {
            if (can.frame.data[2] == 1) { //-继续
                wTest->Pub_Main_to_test(QStringList(""), "0", 1 , 15);
            }
        } else {
            //
        }
    } else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.6.27
      * brief:      IR板
-******************************************************************************/
void w_Main::Pri_Board_IR_ACW()
{
    if (can.frame.data[0] == CAN_UPLOAD_RESULT_ONE) {
        IRACW.time++;
        IRACW.volt += can.frame.data[1]*256+can.frame.data[2];
        IRACW.result += can.frame.data[3]*256+can.frame.data[4];
        IRACW.judge = can.frame.data[6]; //-判定是否合格
        if (IRACW.judge == 1) { // -NG-数据
            IRACW.show_resultc = (can.frame.data[3]*256 + can.frame.data[4] + 4) \
                    * (qPow(10, -can.frame.data[5])); //-不合格数据
        } else {
            //
        }
        if (IRACW.time != IRACW_Time) { //-计时两次
            return;
        } else {
            //
        }

        IRACW.time = 0;
        IRACW.volt = IRACW.volt/IRACW_Time;
        if (((IRACW.volt + 3) <= (IRACW.voltc)) || (((IRACW.volt - 3) >= (IRACW.voltc)))) {
            IRACW.voltc = IRACW.volt; qDebug() << "print IRACW.voltc" << IRACW.voltc;
            IRACW.updata = 1;
        } else {
            //
        }

        IRACW.result = IRACW.result/IRACW_Time; //-平均测试结果 绝缘电阻和耐压电流
        if (((IRACW.result + 2) <= (IRACW.resultc)) || (((IRACW.result - 2) >= (IRACW.resultc)))) {
            IRACW.resultc = IRACW.result;  qDebug() << "IRACW.resultc" << IRACW.resultc;
            IRACW.updata = 1;
            IRACW.resultc_dou = IRACW.resultc*(qPow(10, -can.frame.data[5]));
            if (IRACW.judge == 0) { //- 判定合格将数据 到 显示的最终数据
                IRACW.show_resultc = IRACW.resultc_dou;
            } else {
                //
            }
        }
        IRACW.volt = 0;
        IRACW.result = 0;
        if (window_current == wTest_Surface) {
            if (IRACW.updata != 1) { //-需要更新数据
                return;
            } else {
                //
            }
            IRACW.updata = 0;
            if (wTest->TestIRACW == TestIW_ACW) {
                QStringList ACW_Data; ACW_Data.clear();
                ACW_Data.append(QString::number(Ini_IRACW_True));
                ACW_Data.append(QString::number(IRACW.volt));
                ACW_Data.append(QString::number(wTest->ACW_Test_Count));
                ACW_Data.append(QString::number(IRACW.show_resultc));
                ACW_Data.append(QString::number(IRACW.judge));
                ACW_IR_Test_Data = wConf->WACW->ACW_Test_Result(ACW_Data);
                qDebug() << "ACW---" << ACW_IR_Test_Data;
            } else if (wTest->TestIRACW == TestIW_IR) {
                QStringList IR_Data; IR_Data.clear();
                IR_Data.append(QString::number(Ini_IRACW_True));
                IR_Data.append(QString::number(IRACW.volt));
                IR_Data.append(QString::number(wTest->IR_Test_Count));
                IR_Data.append(QString::number(IRACW.resultc_dou));
                IR_Data.append(QString::number(IRACW.judge));
                ACW_IR_Test_Data = wConf->WIR->IR_Test_Result(IR_Data);
                qDebug() << "IR---" << ACW_IR_Test_Data;
            } else {
                //
            }
            Sigmain_data_to_test(ACW_IR_Test_Data);
        } else if (window_current == wConf_Surface) {
            if (IRACW.updata == 1) {
                IRACW.updata = 0;
                wConf->Pub_main_to_conf(QStringList(""), QString::number(IRACW.resultc_dou), 0, 20);
            } else {
                //
            }
        } else if (window_current == wDebug_Surface) {
            Sigmain_data_to_debug(QStringList(""), QString::number(IRACW.resultc), 3);
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_STATE) {
        if (window_current == wTest_Surface) { //-绝缘数据解析
            wTest->IRFinsh = true;
            if (wTest->TestIRACW == TestIW_ACW) {
                QStringList ACW_Data; ACW_Data.clear();
                ACW_Data.append(QString::number(Ini_IRACW_True));
                ACW_Data.append(QString::number(IRACW.volt));
                ACW_Data.append(QString::number(wTest->ACW_Test_Count));
                ACW_Data.append(QString::number(IRACW.show_resultc));
                ACW_Data.append(QString::number(IRACW.judge));
                ACW_IR_Test_Data = wConf->WACW->ACW_Test_Result(ACW_Data);
                qDebug() << "ACW Finsh---" << ACW_IR_Test_Data;
            } else if (wTest->TestIRACW == TestIW_IR) {
                QStringList IR_Data; IR_Data.clear();
                IR_Data.append(QString::number(Ini_IRACW_True));
                IR_Data.append(QString::number(IRACW.volt));
                IR_Data.append(QString::number(wTest->IR_Test_Count));
                IR_Data.append(QString::number(IRACW.resultc_dou));
                IR_Data.append(QString::number(IRACW.judge));
                ACW_IR_Test_Data = wConf->WIR->IR_Test_Result(IR_Data);
                qDebug() << "IR Finsh---" << ACW_IR_Test_Data;
            } else {
                //
            }
            IRACW.time = 0;
            IRACW.volt = 0;  IRACW.voltc = 0;  IRACW.result = 0;  IRACW.resultc = 0;
            IRACW.resultc_dou = 0;
            IRACW.show_resultc = 0;
            Sigmain_data_to_test(ACW_IR_Test_Data); //-更新数据
        } else if (window_current == wConf_Surface) {
            wConf->Pub_main_to_conf(QStringList(""), QString::number(IRACW.resultc_dou), 1, 20);
        } else {
            board_state[40] = can.frame.data[1];
            board_state[50] = can.frame.data[2]*256 + can.frame.data[3];
        }
        wTest->State.IR = true;
    } else if (can.frame.data[0] == CAN_UPLOAD_Debug) {
        QStringList list;
        list.append(QString::number(can.frame.data[1]));
        list.append(QString::number(can.frame.data[2]));
        list.append(QString::number(can.frame.data[3]));
        list.append(QString::number(can.frame.data[4]));
        list.append(QString::number(can.frame.data[5]));
        Sigmain_data_to_debug(list, QString(""), 6);
    } else if (can.frame.data[0] == CAN_UPLOAD_Version)  {
        Version.IWV = Pri_Board_Version();
    } else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      功放板
******************************************************************************/
void w_Main::Pri_Board_AMP()
{
    int min_Volt;
    if (can.frame.data[0] == CAN_UPLOAD_Version) {
        Version.AMP = Pri_Board_Version();
    }

    if (can.frame.data[0] != CAN_UPLOAD_STATE) {
        return;
    } else {
        //
    }
    if ((window_current == wMain_Surface) || (window_current == wSyst_Surface)) {
        board_state[120] = can.frame.data[1];
    } else {
        //
    }
    min_Volt = (can.frame.data[5] > can.frame.data[6]) ? can.frame.data[6] : can.frame.data[5];
    //-获得火线零线对地线的电压
    AMP_All_Count++;
    if (min_Volt > set_gnd_vlot) {
        AMP_No_Count++;
        System_Display_Volt = min_Volt;
    } else {
        //
    }
    if ((window_current == wMain_Surface) || (window_current == wSyst_Surface)) {
        if (min_Volt > set_gnd_vlot) { // 接地不良
            GND_State = false;
        } else { // 接地良好
            GND_State = true;
        }
    } else if (window_current == wTest_Surface) {
        if (AMP_All_Count >= 10) {
            if (AMP_No_Count <= 3) {
                GND_State = true;
            } else {
                GND_State = false;
            }
            AMP_All_Count = 0;  AMP_No_Count = 0;
        } else {
            //
        }
        wTest->Pub_Main_to_test(QStringList(""), \
                                QString::number(System_Display_Volt+GND_State*1000), \
                                (can.frame.data[3]*256 + can.frame.data[4]-1000)/10, 16);
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.6.27
      * brief:      IMP板
******************************************************************************/
void w_Main::Pri_Board_IMP()
{
    int i;
    QStringList Net_Shape_List;
    if (can.frame.data[0] == CAN_UPLOAD_STATE) {
        qDebug() << "------Pri_Board_IMP()------";
        board_state[60] = can.frame.data[1];
        board_state[70] = can.frame.data[2]*256 + can.frame.data[3];
        wTest->State.IMP = true;
        if (window_current == wTest_Surface) {
            wTest->Pub_Main_to_test(QStringList(""), QString(""), 1, 19);
        } else if (window_current == wConf_Surface) {
            wConf->WIMP->Pub_Main_IMP_FinshSample();
        } else if (window_current == wDebug_Surface) {
            Sigmain_data_to_debug(QStringList(), QString(), 10); //-可以进行测试
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_RESULT_TWO) { //-采样结果上传
        if (window_current == wConf_Surface) {
            Sigmain_can_to_conf(can.frame, QStringList(""), 1);
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_SHAPE) { //-采样结束
        if (window_current != wDebug_Surface) {
            if (can.frame.data[1] == 255) {

                if ((imp_back_number == 0) && (wTest->strITT.at(16).toInt() == 2)) { //- 比较模式
                    for (int i = 0; i < 400; i++) {
                        waveform_IMP_Compare[i] = waveform_point[i];
                    }
                } else {
                    //
                }
                if (window_current == wTest_Surface) {
                    QStringList imp_data;
                    imp_data.clear();
                    imp_data.append(QString::number(imp_waveform_error));
                    imp_data.append(QString::number(imp_back_number));
                    imp_data.append(QString::number(waveform_count));
                    Net_Shape_List.clear();
                    Net_Shape_List.append(QString::number(imp_back_number));
                    for (i = 0; i < 400; i++) {
                        Net_Shape_List.append(QString::number(waveform_point[i]));
                    }
                    emit WriteMessage(6000, 0x86, QString(Net_Shape_List.join(" ")).toUtf8());
                    wTest->Pub_IMP_Param(imp_data, waveform_point);
                    QStringList imp_result;
                    if (wTest->strITT.at(16).toInt() == 2) {
                        imp_result = wConf->WIMP->Pub_Main_IMP_Result_Compare(imp_back_number, \
                                                         waveform_point, waveform_IMP_Compare);
                    } else {
                        imp_result = wConf->WIMP->Pub_Main_IMP_Result(imp_back_number, \
                                                     waveform_point, Back_out_station);
                    }
                    Sigmain_data_to_test(imp_result);
                } else if (window_current == wConf_Surface) { // 显示在显示区
                    qDebug() << "写入";
                    wConf->WIMP->Pub_Main_IMP_Get_date(imp_back_number, waveform_point);
                    Net_Shape_List.clear();
                    Net_Shape_List.append(QString::number(imp_back_number));
                    for (i = 0; i < 400; i++) {
                        Net_Shape_List.append(QString::number(waveform_point[i]));
                    }
                    emit WriteMessage(6000, 0x85, QString(Net_Shape_List.join(" ")).toUtf8());
                    waveform_count = 0;
                } else {
                    //
                }
                waveform_count = 0;
            } else {
                imp_back_number = can.frame.data[1];
                if (window_current == wTest_Surface) {
                    wTest->Pub_Main_to_test(QStringList(""), QString(""), imp_back_number, 20);
                } else {
                    //
                }
                if (window_current == wConf_Surface) {
                    wConf->WIMP->IMP_WaveList[imp_back_number]->IMP_PP = \
                            can.frame.data[3]*256 + can.frame.data[4];
                    wConf->WIMP->IMP_WaveList[imp_back_number]->IMP_Point = can.frame.data[2];
                } else {
                    //
                }
                waveform_count = 0;
            }
        } else if (window_current == wDebug_Surface) {
            if (can.frame.data[1] == 255) {
                //
            } else {
                Sigmain_data_to_debug(QStringList(), \
                                QString::number(can.frame.data[3]*256 + can.frame.data[4]), 8);
            }
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_Version) { //-版本号
        Version.IMPV = Pri_Board_Version();
        QString string = "";
        string.clear();
        for (int i = 0; i < 8; i++) {
            string += QString::number(can.frame.data[i]);
        }
        if (string.toInt() >= 60) {
            wConf->Pub_main_to_conf(QStringList(""), QString(""), 1, 17);
            Sigmain_data_to_syst(QString(""), 3);
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_Debug) { //-调试参数上传
        if ((window_current == wDebug_Surface) && (can.frame.data[1] <= 5)) {
            QStringList list;
            list.append(QString::number(can.frame.data[1]));
            list.append(QString::number(can.frame.data[2]));
            list.append(QString::number(can.frame.data[3]));
            Sigmain_data_to_debug(list, QString(""), 9);
        } else {
            //
        }
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.11.14
      * brief:      PWR 板
******************************************************************************/
void w_Main::Pri_Board_PWR(int Id)
{
    if (can.frame.data[0] == CAN_UPLOAD_STATE) {
        if ((window_current == wMain_Surface) || (window_current == wSyst_Surface)) {
            //-查询数据的状态
            board_state[80] = can.frame.data[1]; //-左工位
            board_state[90] = can.frame.data[2]*256+can.frame.data[3];
            return;
        }
        if (window_current == wTest_Surface) {
            if (can.frame.data[1] == 0) {
                QStringList pg_data; pg_data.clear();
                pg_data.append(QString::number(pg_waveform_error));
                wTest->Pub_PWR_Param(pg_data, waveform_point);
            } else if (can.frame.data[1] == 1) {
                wTest->Pub_Main_to_test(QStringList(""), QString(""), 1, 7);
            } else {
                //
            }
        } else if (window_current == wConf_Surface) {
            if (can.frame.data[1] == 0) {
                wConf->Pub_main_to_conf(QStringList(""), QString(""), 1, 16);
            } else if (can.frame.data[1] == 5) {
                wConf->Pub_main_to_conf(QStringList(""), QString(""), 2, 16);
            } else {
                //
            }
        } else {
            //-其他界面暂时不做处理
        }
    } else if ((can.frame.data[0] == 0x01) || (can.frame.data[0] == 0x02) || \
               (can.frame.data[0] == 0x03)) { // -A相上传 频率时间转向转速  B相上传
        if (window_current == wTest_Surface) {
            wTest->Pub_Main_can_to_test(can.frame, 1);
        } else {
            //
        }
        if (window_current == wConf_Surface) {
            if (can.frame.data[0] == 1) {
                Sigmain_can_to_conf(can.frame, QStringList(""), 5);
            } else {
                //
            }
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_Debug) { //-调试界面
        QStringList Aphase;     Aphase.clear();
        QStringList Bphase;     Bphase.clear();
        QStringList Cphase;     Cphase.clear();
        switch (can.frame.data[1]) {
        case 0:
            Aphase.append(QString::number((can.frame.data[2]*256 + can.frame.data[3])));
            Aphase.append(QString::number((can.frame.data[4]*256 + can.frame.data[5])));
            Aphase.append(QString::number((can.frame.data[6]*256 + can.frame.data[7])));
            Sigmain_data_to_debug(Aphase, QString::number(can.frame.data[1]), 12);
            break;
        case 1:
            Bphase.append(QString::number((can.frame.data[2]*256 + can.frame.data[3])));
            Bphase.append(QString::number((can.frame.data[4]*256 + can.frame.data[5])));
            Bphase.append(QString::number((can.frame.data[6]*256 + can.frame.data[7])));
            Sigmain_data_to_debug(Bphase, QString::number(can.frame.data[1]), 12);
            break;
        case 2:
            Cphase.append(QString::number((can.frame.data[2]*256 + can.frame.data[3])));
            Cphase.append(QString::number((can.frame.data[4]*256 + can.frame.data[5])));
            Cphase.append(QString::number((can.frame.data[6]*256 + can.frame.data[7])));
            Sigmain_data_to_debug(Cphase, QString::number(can.frame.data[1]), 12);
            break;
        default:
            break;
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_Version) {
        if (Id == 225) {
            Version.PWR_LV = Pri_Board_Version();
        } else {
            //
        }
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.11.14
      * brief:      INDL板
******************************************************************************/
void w_Main::Pri_Board_INDL()
{
    if (can.frame.data[0] == CAN_UPLOAD_STATE) {
        if ((window_current == wMain_Surface) || (window_current == wSyst_Surface)) {
            board_state[100] = can.frame.data[1];
        } else if (window_current == wDebug_Surface) {
            Sigmain_data_to_debug(QStringList(""), QString::number(can.frame.data[1]), 13);
        } else if (window_current == wConf_Surface) {
            wConf->Board_INDL = 1;
            wConf->Pub_main_to_conf(QStringList(""), QString(""), 1, 12);
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_RESULT_ONE) {
        if (window_current == wTest_Surface) {
            wTest->Pub_Main_can_to_test(can.frame, 2);
        } else if (window_current == wConf_Surface) {
            Sigmain_can_to_conf(can.frame, QStringList(""), 3);
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_Version) {
        QString string = "";
        string.clear();
        for (int i = 1; i < 6; i++) {
            string += (QChar)(can.frame.data[i]);
        }
        string += QString::number(can.frame.data[6]);
        string += " ";
        string += QString::number(can.frame.data[7]);
        Version.INDLV = string;
        qDebug() << "Version.INDLV" << Version.INDLV;
    } else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.6.12
      * brief:      ACSW板
******************************************************************************/
void w_Main::Pri_Board_ACSW()
{
    if (can.frame.data[0] == CAN_UPLOAD_STATE) {
        if ((window_current == wMain_Surface) || (window_current == wSyst_Surface)) {
            board_state[140] = can.frame.data[1];
        } else {
            //
        }
    } else if (can.frame.data[0] == CAN_UPLOAD_Version) {
        Version.ACSW = Pri_Board_Version();
    } else if ((can.frame.data[0] == 1) || (can.frame.data[0] == 2) || (can.frame.data[0] == 3)) {
        if (window_current == wTest_Surface) {
            wTest->Pub_Main_can_to_test(can.frame, 3);
        } else {
            //
        }
    } else if (can.frame.data[0] == 4) {
        if (can.frame.data[1] == 0) {
            waveform_count = 0; qDebug() << "PG waveform start upload";
        } else if (can.frame.data[1] != 0) {
            qDebug() << "PG waveform start upload finsh";
        } else {
            //
        }
    } else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.8.18
      * brief:      PG板
******************************************************************************/
void  w_Main::Pri_Board_PG() {
    QStringList list_d, list_BEMF;
    if (can.frame.data[0] == CAN_UPLOAD_STATE) {
        board_state[170] = can.frame.data[1];
        if (window_current == wTest_Surface) {
            list_d.clear();
            list_d.append(QString::number(PG_Fre));
            list_d.append(QString::number(PG_Bb));
            list_d.append(QString::number(PG_High));
            list_d.append(QString::number(PG_Low));
            wTest->Pub_Main_to_test(list_d, QString(""), 0, 17);

            if (can.frame.data[1] == 0x09) {
                qDebug() << "w_Main_____________________Up finsh---Start test";
                Fg_Direction = 0;
                WD_PWR = true;
                wTest->Pub_Main_to_test(QStringList(""), "0", 1, 22);
            } else if (can.frame.data[1] == 0x08) {
                qDebug() << "w_Main_____________________Down finsh---Stop  test";
                WD_PWR = false;
                wTest->NoLoadFinsh = true;
                if (Ini_BLDC_PG) {
                    frame.can_id  = 0x2D;  frame.can_dlc = 0x03;
                    frame.data[0] = 0xf1;
                    frame.data[1] = 0x00;
                    frame.data[2] = 0x00;
                    frame.data[3] = 0x00;
                    canSend(frame, 88);   usleep(5000); usleep(5000); usleep(5000);
                }else {
                    //
                }

                list_d.clear();
                list_d.append(QString::number(WD_I));
                list_d.append(QString::number(WD_V));
                list_d.append(QString::number(WD_P));
                list_d.append(QString::number(Square_WD));
                list_d.append(QString::number(Fg_Direction));

                Sigmain_data_to_test(list_d);

                PG_Fre = 0;
                PG_Bb  = 0;
                PG_High = 0;
                PG_Low = 0;
            } else if (can.frame.data[1] == 0x03) {
                qDebug() << "w_Main_____________________Noload BEMF finsh-------------";

                list_BEMF.append(BEMF_HALL);
                list_BEMF.append(BEMF_HIGH_Low);
                list_BEMF.append(Shape_BEMF_Fre);
                wTest->Pub_Main_to_test(list_BEMF, "0", 1, 23);
                BEMF_HALL.clear();
                for (int i = 0; i < 200; i++) {
                    BEMF_HALL.append("0");
                }
                BEMF_HIGH_Low.clear();
                for (int i = 0; i < 150; i++) {
                    BEMF_HIGH_Low.append("0");
                }
                Shape_BEMF_Fre.clear();
                for (int i = 0; i < 20; i++) {
                    Shape_BEMF_Fre.append("0");
                }
            } else if (can.frame.data[1] == 0x0a) {
                qDebug() << "w_Main_____________________PG Reset ok";
                wTest->Pub_Main_to_test(QStringList(""), "0", 1, 25);
            } else {
                //
            }
        } else {
            //
        }
    } else if (can.frame.data[0] == 0x09) {
        if (can.frame.data[2] == 0x00) {
            PG_Fre = can.frame.data[5]*65536 + can.frame.data[4]*256 + can.frame.data[3];
            PG_Fre = PG_Fre*60; //
        } else if (can.frame.data[2] == 0x01) {
            PG_Bb = can.frame.data[4]*256 + can.frame.data[3];
        } else  {
            //
        }
        Shape_BEMF_Fre.replace(can.frame.data[2], \
          QString::number(can.frame.data[5]*65536 + can.frame.data[4]*256 + can.frame.data[3]));
    } else if (can.frame.data[0] == 0x0a) {
        BEMF_HIGH_Low.replace((can.frame.data[1]*10 + can.frame.data[2])*2, \
               QString::number(can.frame.data[4]*256 + can.frame.data[3]));
        BEMF_HIGH_Low.replace((can.frame.data[1]*10 + can.frame.data[2])*2+1, \
               QString::number(can.frame.data[6]*256 + can.frame.data[5]));

    } else if (can.frame.data[0] == 0x0c) {
        qDebug() << "Shape Start-------------";
        Shape_Pg.append(QString::number(can.frame.data[1]));
    } else if (can.frame.data[0] == 0xff) {
        qDebug() << "Shape Stop-------------";
        qDebug() << "Shape_Pg" << Shape_Pg.at(0);
        wTest->Pub_FG_Param(Shape_Pg);
        waveform_count = 0;
        WriteMessage((6000), (0x75), QString(Shape_Pg.join(" ")).toUtf8());
        Shape_Pg.clear();     
    } else if (can.frame.data[0] == 0x0e) {
        if (can.frame.data[1] == 0x03) {
            BEMF_HALL.replace(can.frame.data[2]*2, \
                    QString::number(can.frame.data[4]*256+can.frame.data[3]));
            BEMF_HALL.replace(can.frame.data[2]*2+1, \
                    QString::number(can.frame.data[6]*256+can.frame.data[5]));
        } else if (can.frame.data[1] == 0x00) {
            BEMF_HALL.replace(can.frame.data[2]*3+can.frame.data[1]+60, \
              QString::number(can.frame.data[4]*256+can.frame.data[3]));
        } else if (can.frame.data[1] == 0x01) {
            BEMF_HALL.replace(can.frame.data[2]*3+can.frame.data[1]+60, \
              QString::number(can.frame.data[4]*256+can.frame.data[3]));
        } else if (can.frame.data[1] == 0x02) {
            BEMF_HALL.replace(can.frame.data[2]*3+can.frame.data[1]+60, \
              QString::number(can.frame.data[4]*256+can.frame.data[3]));
        } else {
            //
        }
    } else if (can.frame.data[0] == 0x17) {
        Fg_Direction = can.frame.data[1];
    } else if (can.frame.data[0] == CAN_UPLOAD_Version) {
        Version.VBLDCPG = Pri_Board_Version();
        qDebug() << "Version.VBLDCPG" << Version.VBLDCPG;
    } else {
        //
    }
}

void  w_Main::Pri_Board_IO() {
    int i;
    int in_data;
    if (can.frame.data[0] == CAN_UPLOAD_STATE) {
        board_state[210] = can.frame.data[1];
    } else if (can.frame.data[0] == CAN_UPLOAD_Version) {
        Version.IO = Pri_Board_Version();
    } else if ((can.frame.data[0] == 0xf0)&&((can.frame.can_dlc == 3))) {
        in_data = can.frame.data[1] + can.frame.data[2]*256;
        WriteMessage((6000), (0x79), QString::number(in_data).toUtf8());
        if (PIC_Connect) {
            Signal_main_to_pic(QStringList(""), in_data, 1);
        } else {
            //
        }
        wTest->Pub_Main_to_test(QStringList(""), QString(""), in_data, 120);
    } else if ((can.frame.data[0] == 0xf0)&&((can.frame.can_dlc == 4))) {
        in_data = can.frame.data[1]+can.frame.data[2]*256 + can.frame.data[3]*65536;
        WriteMessage((6000), (0x79), QString::number(in_data).toUtf8());
        if (PIC_Connect) {
            Signal_main_to_pic(QStringList(""), in_data, 1);
        } else {
            //
        }
    //    wTest->Pub_Main_to_test(QStringList(""), QString(""), in_data, 120);
    } else if (can.frame.data[0] == 0x11) {
        RFID_H.clear();
        for (i = can.frame.can_dlc-1; i >= 1; i--) {
            RFID_H += QString("%1").arg(can.frame.data[i] & 0xFF, 2, 16, QLatin1Char('0'));
        }
        wTest->Pub_Main_to_test(QStringList(""), RFID_H, 1, 121);
    }  else if (can.frame.data[0] == 0x12) {
        Sigmain_data_to_syst(QString::number(can.frame.data[2]),4);
    }  else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.8.18
      * brief:      WD板
******************************************************************************/
void  w_Main::Pri_Board_WD() {
    int j;
    QStringList list_d;
    if (can.frame.data[0] == CAN_UPLOAD_STATE) {
        board_state[190] = can.frame.data[1];
        if (window_current == wTest_Surface) {
            list_d.clear();
            list_d.append(QString::number(WD_I));
            list_d.append(QString::number(WD_V));
            list_d.append(QString::number(WD_P));
            Sigmain_data_to_test(list_d);
        }
    } else if (can.frame.data[0] == 0x03) {
        if (!WD_PWR) {
            qDebug() << "System back";
            return;
        }
        qDebug() << "1-------------" << (can.frame.data[3]*256 + can.frame.data[2]);
        WD_I = ((double(can.frame.data[3]*256 + can.frame.data[2]))/10000)*3.277 - 0.204;
        if (WD_I < 0.00) {
            WD_I = 0.00;
        } else {
            //
        }
        WD_V = ((double(can.frame.data[5]*256 + can.frame.data[4]))/10000)*497.96 - 0.99;
        if (WD_V < 0.00) {
            WD_V = 0.00;
        } else {
            //
        }
        WD_P = WD_I*WD_V;

        list_d.clear();
        list_d.append(QString::number(WD_I));
        list_d.append(QString::number(WD_V));
        list_d.append(QString::number(WD_P));
        list_d.append(QString::number(Square_WD));
        list_d.append(QString::number(Fg_Direction));

        Sigmain_data_to_test(list_d);
    } else if (can.frame.data[0] == 0x06) {
        Square_WD = can.frame.data[4]*65536 + can.frame.data[3]*256 + can.frame.data[2];
    } else if (can.frame.data[0] == 0x07) {
        //
    } else if (can.frame.data[0] == CAN_UPLOAD_Version) {
        Version.BLDCWD = Pri_Board_Version();
    } else {
        //
    }
}


/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.8.18
      * brief:      PG板波形数据
******************************************************************************/
void  w_Main::Pri_Board_PG_Wave() {
    int i;
    for (i = 0; i < 8; i++) {
        Shape_Pg.append(QString::number(can.frame.data[i]));
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.5.20
      * brief:      输出板状态检测和启动测试
******************************************************************************/
QString w_Main::Pri_Board_Version()
{
    QString string = "";
    string.clear();

    for (int i = 1; i < 6; i++) {
        string += (QChar)(can.frame.data[i]);
    }
    string += QString::number(can.frame.data[6]);
    string += "-";
    string += QString::number(can.frame.data[7]);

    return string;
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.5.26
      * brief:      打印CAN接收信息
******************************************************************************/
void w_Main::Pri_Can_Receive(int Id)
{
    int i;
    QString string = "";
    can_number++;

    string.clear();
    string = QString::number(Id)+" "+ QString::number(can.frame.can_dlc)+" ";
    for (i = 0; i < can.frame.can_dlc; i++) {
        string += QString::number(can.frame.data[i]); string += " ";
    }
    string += "  ";  string += QString::number(can_number);
    qDebug() << string;
    string.clear();
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2015.5.5
      * brief:      打印串口信息 (2016.10.9 添加输出板输出量信号)
******************************************************************************/
void w_Main::Pubs_Usart_Send(char *Buf)
{
    myCom->WritePort(Buf, 4); usleep(10); myCom->WritePort(Buf, 4); usleep(00);

    qDebug() << "out_station" << out_station;
    Back_out_station = out_station; //-发送给输出板
    frame.can_id = Back_out_station;
    frame.can_dlc = 0x02; //-输出板输出量信号发送
    frame.data[0] = 0x02;
    if (QString(Buf) == "LEDY") {
        frame.data[1] = 0x01;
    } else if (QString(Buf) == "LEDG") {
        frame.data[1] = 0x02;
    } else if (QString(Buf) == "LEDR")  {
        frame.data[1] = 0x03;
    } else {
        frame.data[1] = 0x00;
    }
    canSend(frame, 88); usleep(500);

    if ((QString(Buf) == "LEDG")&&(out_stamp)) { //-判定合格之后，进行短暂显示
        out_board->setSingleShot(TRUE);
        out_board->start(system_conf.at(1).toDouble()*1000); //-印度客户启动定时器
    } else {
        //
    }

    qDebug() << "--------------------------------------&&--------------------";
    if (Ini_LR_Beep&&(Back_out_station == 20)) {
        frame.can_id = 0x22;  frame.can_dlc = 0x02; //-发送给电阻板
        frame.data[0] = 0x09;
        if (QString(Buf) == "LEDY") { //-黄灯
            frame.data[1] = 0x04;
        } else if (QString(Buf) == "LEDG") { //-绿灯
            if (DCR_Beep_Ok != 0) {
                DCR_BEEP = 0x02|0x08;
                dcr_board->setSingleShot(TRUE);
                dcr_board->start(DCR_Beep_Ok*1000);
            } else {
                //
            }
            frame.data[1] = DCR_BEEP;
        } else if (QString(Buf)== "LEDR") { //-红灯
            if (DCR_Beep_Ng != 0) {
                DCR_BEEP = 0x01|0x08;
                dcr_board->setSingleShot(TRUE);
                dcr_board->start(DCR_Beep_Ng*1000);
            } else {
                //
            }
            frame.data[1] = DCR_BEEP;
        } else {
            frame.data[1] = 0x00;
            dcr_board->stop();
        }
        canSend(frame, 88); usleep(500);
    } else if (Ini_LR_Beep&&(Back_out_station == 19)) {
        frame.can_id = 0x23;  frame.can_dlc = 0x02; //-发送给耐压板
        frame.data[0] = 0x09;
        if (QString(Buf) == "LEDY") { //-黄灯
            frame.data[1] = 0x04;
        } else if (QString(Buf) == "LEDG") { //-绿灯
            if (DCR_Beep_Ok != 0) {
                DCR_BEEP = 0x02|0x08;
                acw_board->setSingleShot(TRUE);
                acw_board->start(DCR_Beep_Ok*1000);
            } else {
                //
            }
            frame.data[1] = DCR_BEEP;
        } else if (QString(Buf) == "LEDR") { //-红灯
            if (DCR_Beep_Ng != 0) {
                DCR_BEEP = 0x01|0x08;
                acw_board->setSingleShot(TRUE);
                acw_board->start(DCR_Beep_Ok*1000);
            } else {
                //
            }
            frame.data[1] = DCR_BEEP;
        } else {
            frame.data[1] = 0x00;
            acw_board->stop();
        }
        canSend(frame, 88); usleep(500);
    } else {
        frame.can_id = 0x22;  frame.can_dlc = 0x02; //-发送给电阻板
        frame.data[0] = 0x09;
        if (QString(Buf) == "LEDY") { //-黄灯
            frame.data[1] = 0x04;
        } else if (QString(Buf) == "LEDG") { //-绿灯
            if (DCR_Beep_Ok != 0) {
                DCR_BEEP = 0x02|0x08;
                dcr_board->setSingleShot(TRUE);
                dcr_board->start(DCR_Beep_Ok*1000);
            } else {
                //
            }
            frame.data[1] = DCR_BEEP;
        } else if (QString(Buf) == "LEDR") { //-红灯
            if (DCR_Beep_Ng != 0) {
                DCR_BEEP = 0x01|0x08;
                dcr_board->setSingleShot(TRUE);
                dcr_board->start(DCR_Beep_Ng*1000);
            } else {
                //
            }
            frame.data[1] = DCR_BEEP;
        } else {
            frame.data[1] = 0x00;
            dcr_board->stop();
        }
        canSend(frame, 88); usleep(500);
    }

    if (QString(Buf) == "LED1") {
        frame.can_id = 0x22;  frame.can_dlc = 0x02; //-发送给电阻板
        frame.data[0] = 0x09;
        frame.data[1] = 0x00;
        dcr_board->stop();
        canSend(frame, 88); usleep(500);
        frame.can_id = 0x23;  frame.can_dlc = 0x02; //-发送给耐压板
        frame.data[0] = 0x09;
        frame.data[1] = 0x00;
        acw_board->stop();
        canSend(frame, 88); usleep(500);
    } else {
        //
    }
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      输出板停止
******************************************************************************/
void w_Main::Out_Board_Stop() {
    frame.can_id = Back_out_station; //-输出板输出量信号发送
    frame.can_dlc = 0x02;
    frame.data[0] = 0x02;
    frame.data[1] = 0x00;
    canSend(frame, 88); usleep(500);
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.7.20
      * brief:      电阻板停止
******************************************************************************/
void w_Main::Dcr_Board_Stop() {
    frame.can_id = 0x22; //-输出板输出量信号发送
    frame.can_dlc = 0x02;
    frame.data[0] = 0x09;
    frame.data[1] = (DCR_BEEP&0x07);
    canSend(frame, 88); usleep(500);
}

void w_Main::Acw_Board_Stop() {
    frame.can_id = 0x23; //-输出板输出量信号发送
    frame.can_dlc = 0x02;
    frame.data[0] = 0x09;
    frame.data[1] = (DCR_BEEP&0x07);
    canSend(frame, 88); usleep(500);
}

/******************************************************************************
       * version:    1.0
       * author:     sl
       * date:       2016.5.19
       * brief:      设置警告弹出框
******************************************************************************/
void  w_Main::Pri_Main_WMessage(int number) {
    if (wTest->START_Model == 3) {
        WMessage_Record.append(QString::number(number));
    } else {
        QMessageBox *message = new QMessageBox(this);
        message->setWindowFlags(Qt::FramelessWindowHint);
        message->setStyleSheet("QMessageBox{border: gray;" \
                       "border-radius: 10px;padding:2px 4px;" \
                       "background-color: gray;height:390px;width:375px;}");
        message->setText(WMessage_Text.at(number)+"\n");
        message->addButton(QMessageBox::Ok)->setStyleSheet("height:39px;width:75px;"\
                  "border:5px groove;border:none;border-radius:10px;padding:2px 4px;");
        message->setButtonText(QMessageBox::Ok, QString(tr("确 定")));
        message->setIcon(QMessageBox::Warning);
        message->exec();
    }
    /*  if (Shake_hand_Success) {
          WriteMessage((6000), (0x23),QString("%1").arg(QString::number(100+number)).toUtf8());
      } */
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.6.8
      * brief:      查询板子状态
******************************************************************************/
void  w_Main::Pri_Can_CheckState(int id) {
    frame.can_id = id;  frame.can_dlc = 0x01; //-输出板发送
    frame.data[0] = 0x00;
    canSend(frame, 88);
    usleep(500);
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.6.8
      * brief:      查询板子版本
******************************************************************************/
void  w_Main::Pri_Can_CheckVersion(int id)
{
    frame.can_id = id;  frame.can_dlc = 0x01; //-输出板发送
    frame.data[0] = 0x08;
    canSend(frame, 88);
    usleep(500);
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.6.8
      * brief:      设置输出板  启动方式
******************************************************************************/
void  w_Main::Pri_Can_Set_OUTModel(int id, int KeyModel)
{
    frame.can_id =  id;  frame.can_dlc = 0x02;
    frame.data[0] = 0x03;
    frame.data[1] = KeyModel;
    canSend(frame, 88);
    usleep(500);
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2016.9.28
      * brief:      设置内存空间的剩余空间报警
******************************************************************************/
void  w_Main::Pri_availableDisk()
{
    struct statfs diskInfo;
    statfs("/mnt/nandflash", &diskInfo);
    uint64_t blocksize = diskInfo.f_bsize;                  // -每个block里包含的字节数
    uint64_t freeDisk = diskInfo.f_bfree * blocksize;       // -剩余空间的大小
    uint64_t availableDisk = diskInfo.f_bavail * blocksize; // -可用空间大小

    qDebug() << "The System remaining space--" << (freeDisk >> 20) << (availableDisk >> 20);
    if ((availableDisk>>20) < Memory_surplus) { //-设置内存空间
        Pri_Main_WMessage(10);
    }
}

/******************************************************************************
                                        END

******************************************************************************/
