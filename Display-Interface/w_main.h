#ifndef W_MAIN_H
#define W_MAIN_H

#include <QWidget>
#include <QTimer>
#include <QProgressBar>
#include <QEvent>
#include <QMouseEvent>
#include <QInputDialog>
#include <QDebug>
#include <QPointer>
#include <QtCore/qmath.h>
#include <QMessageBox>
#include <QThread>
#include <QTextCodec>

#include "define.h"
#include "w_syst.h"
#include "w_test.h"
#include "w_help.h"
#include "w_keyb.h"
#include "w_conf.h"
#include "w_colr.h"
#include "w_debug.h"
#include "w_data.h"
#include "wt_wave.h"
#include "password.h"
#include "linux_can.h"
#include "Linux_Usart.h"
#include "ipmethod.h"
#include "mytcpthread.h"
#include "UdpClient.h"
#include "pic.h"
#include "SQL_DB.h"

#include "sql/sqldesktop.h"

#define CAN_DLR        0x41   // 电阻板
#define CAN_DLR_SHAPE  0x441  // 电阻板反嵌波形
#define CAN_IR         0x61   // 耐压板
#define CAN_ANP        0x141  // 功放板
#define CAN_IW         0x81   // 匝间板
#define CAN_IW_SHAPE   0x481  // 匝间板波形
#define CAN_PWR1       0xE1   // 电参板 1
#define CAN_PWR2       0x101  // 电参板 2
#define CAN_PWR_SHAPE  0x521  // 电参板波形
#define CAN_INDL       0xC1   // 电感板
#define CAN_ACSW       0x121  // 电源切换板
#define CAN_PG         0x181  // PG板
#define CAN_WD         0x161  // WD板
#define CAN_IO         0x1A1  // IO板
#define CAN_PG_SHAPE   0x581  // PG板波形

#define CAN_13OUT      0x261 // 输出板13
#define CAN_14OUT      0x281 // 输出板14
#define CAN_15OUT      0x2A1 // 输出板15
#define CAN_16OUT      0x2C1 // 输出板16
#define CAN_17OUT      0x2E1 // 输出板17

#define CAN_UPLOAD_STATE        0x00 // 上传状态
#define CAN_UPLOAD_RESULT_ONE   0x01 // 上传结果1
#define CAN_UPLOAD_RESULT_TWO   0x02 // 上传结果2
#define CAN_UPLOAD_SHAPE        0x03 // 上传波形
#define CAN_UPLOAD_Debug        0x06 // 上传调试参数
#define CAN_UPLOAD_Version      0x08 // 上传版本
#define CAN_UPLOAD_In           0x09 // 上传输出信号
#define IRACW_Time              2    // 绝缘板更新的次数

#define Version_System      "Release" // Debug Release
#define Version_number   "V-2.0.8.1.1"

// V-2.0.8.9 V-2.0.8.10 // 电感单位变化

namespace Ui {
class w_Main;
}

class w_Main : public QWidget
{
    Q_OBJECT
public:
    explicit w_Main(QWidget *parent = 0);
    ~w_Main();
    w_Syst   *wSyst; // -系统界面
    SqlDesktop   *wData; // -数据管理
    w_Test   *wTest; // -测试界面
    w_Help   *wHelp; // -帮助界面
    w_Conf   *wConf; // -配置界面
    w_Colr   *wColr; // -颜色选择界面
    w_Debug  *wDebug;    // -调试界面
    CSerial  *myCom;     // -串口
    PassWord *wPassWord; // -密码
    pic      *wpic;      // -pic
    MyThread *myThread;
    SQL_DB   *MYSQL_DB;
private:
    Ui::w_Main *ui;

    QThread    *tcp; // -tcp进行数据的网络传输
    QThread    *udp; // -udp进行数据的网络传输
    LINUX_CAN   can; // -can通信
    can_frame frame; // -can数据信息

    QTimer *out_board;       // -输出板定时器控制信号戳
    QTimer *dcr_board;       // -电阻板定时器控制蜂鸣器的时间
    QTimer *acw_board;       // -电阻板定时器控制蜂鸣器的时间
    QTimer *Timer_system;            // -系统运行节拍
    QButtonGroup *Button_Main_Group; // -主页面按钮进行测试
    QWSInputMethod *p_ipmethod;      // -输入法信息

    int  step_system;                // -系统初始化步骤
    int  window_old, window_current;  // -系统上一个窗口，当前窗口

    int  can_number;          // -Can接收数量（测试使用）
    int  can_bug_time;        // -Can发送数据后进行计时，判断板子是否存在

    int  imp_back_number;     // -匝间返回序列号
    bool imp_waveform_error;  // -匝间波形错误标志位
    int  mag_back_number;     // -反嵌返回序列号
    bool mag_waveform_error;  // -反嵌波形错误标志位
    bool mag_set_rotation;    // -反嵌设置顺时针为正转，逆时针为正转
    bool pg_waveform_error;   // -PG波形错误标志位
//    bool fg_waveform_error;   // -FG波形错误标志位

    QString mag_str;

    struct {
        bool updata;          // -数据更新标志位
        int  time;            // -采集数据计时
        int  volt, voltc;      // -采集电压和电压的复制
        int  result, resultc;  // -电阻或电流的返回结果和结果的复制
        double  resultc_dou;  // -电阻或电流的返回结果转换-(double)型数据
        double  show_resultc; // -显示在界面的结果
        int  judge;           // -结果的判定
    }   IRACW;                // 绝缘交耐


    int  waveform_point[800];          // -反嵌、匝间、PG---波形存储数据点
    int  waveform_IMP_Compare[800];    // 匝间波形比较模式
    int  waveform_count;               // 反嵌和匝间 波形数量累加，达到400点
    int  out_station, Back_out_station; // -输出板工位信息，增加(Back_out_station),
                                        // 防止测试过快测试匝间时，突然滑到另外一侧，显示到另一边
    bool isTesting;  // -防止在滑罩启动模式下，左工位启动后滑罩未移开，右工位可以用螺丝刀启动
    quint8 PreCount; // -防止在滑罩启动模式下，左工位启动后滑罩未移开，右工位可以用螺丝刀启动

    struct {
        QTimer *Time;
        int Ms;
        int Us;
        bool Flag;
    }   Mouse;  //长按

    int  barcode_start;       // -条形码截取开始
    int  barcode_stop;        // -条形码截取结束
    bool out_stamp;           // -输出板信号戳
    int  signal_model;        // -电阻板输入信号的选择模式
    bool nine_board;          // -8插板和9插板的区分 2017.5.18
    int  set_gnd_vlot;        // -系统设置接地电压
    bool acsw_board;          // -电源切换板
    bool udp_enable;          // -启用UDP传输
    QStringList system_conf;  // -读取系统配置
    int  Ini_Number;          // -读取当前系统的数量
    QStringList Ini_Proj_Real;  // -当前系统的全部
    QStringList Ini_Proj;     // -当前系统
    QStringList Ini_Password; // -读取密码
    bool  Ini_IRACW_True;     // -绝缘耐压真实电压
    QString Ini_WD;           // -WD板子
    bool  Ini_IO;             // -IO板子
    bool  Ini_LR_Beep;        // -左右工位Beep分离
    int   Ini_IO_Model;
    QString  Ini_Motor;
    int   Ini_MAG_Device;     // -反嵌驱动装置
    bool  Ini_Vacuum_Model;   // -真空测试
    int  Ini_BLDC_PG;

    double DCR_Beep_Ok; // -电阻板蜂鸣器OK的时间
    double DCR_Beep_Ng; // -电阻板蜂鸣器NG的时间
    int    DCR_BEEP;    // -电阻板下发的报警灯的信息（蜂鸣器，红，绿，黄）
    int    calibration_count; // -校准按键计数，在连续按十次以上进行校准
    int    Cheak_Ready_count; // -系统初始化进入时，查询板子的状态设置为双数时，进行清除

    bool GND_State;     // -判断接地状态
    int  AMP_All_Count; // -记录检查系统是否进行接地的次数
    int  AMP_No_Count;  // -记录系统接地不合格的次数
    int  System_Display_Volt;

    bool Shake_hand_Success;
    int  Shake_hand_runcount;

    double WD_I, WD_V, WD_P;
    double Square_WD;
    int Fg_Direction;
    uint32_t PG_Fre,  PG_Bb,  PG_High,  PG_Low;
    int  board_state[220];   // -board_state+ErrorValue 20*6  \
                             // 输出板 电阻  绝缘 匝间板 电参 电感 \
                             // 功放（2017.5.18-140） 电源切换（2017.6.12-160）
                             // PG板 WD板 IO板
    QString Main_ActiveFile; // -2017.6.7 主函数里面的当前文件
    QStringList ACW_IR_Test_Data; // -绝缘耐压测试数据
    QStringList WMessage_Text;    // -警告文本信息
    QStringList WMessage_Record;  // -警告记录
    bool  WD_PWR;
    bool  PIC_Connect;
    QString RFID_H;
    
    struct {
        QString OutV[5]; // -输出板版本
        QString DLRV;    // -电阻板版本
        QString IMPV;    // -匝间板版本
        QString IWV;     // -绝缘板版本
        QString PWR_LV;  // -功率板版本
        QString INDLV;   // -电感板版本
        QString ACSW;    // -电源切换板版本
        QString AMP;     // -功放板版本
        QString VBLDCPG; // -BLDC-PG
        QString BLDCWD;  // -BLDC-WD
        QString IO;      // -IO
    }   Version;

    QStringList BEMF_HALL, BEMF_HIGH_Low;
    QStringList Shape_Pg;
    QStringList Shape_BEMF_Fre;
    bool  handleMousePressEvent(QMouseEvent *event);
    void  mousePressEvent(QMouseEvent * event);
    void  mouseReleaseEvent(QMouseEvent * event);

    void  Pri_Main_WMessage(int);
    void  Pri_availableDisk();
    void  Pri_TcpInit();
    void  Pri_Board_Out(int, int);
    void  Pri_Board_DLR();
    void  Pri_Board_IR_ACW();  void  Pri_Board_AMP();
    void  Pri_Board_IMP();
    void  Pri_Board_PWR(int);
    void  Pri_Board_INDL();
    void  Pri_Board_ACSW();
    void  Pri_Board_PG();
    void  Pri_Board_WD();
    void  Pri_Board_IO();
    void  Pri_Board_PG_Wave();
    void  Pri_Can_Receive(int);
    void  Pri_Can_CheckState(int);
    void  Pri_Can_CheckVersion(int);
    void  Pri_Can_Set_OUTModel(int, int);
    void  Pri_Synchron_Syst_Data();
    void  Pri_Synchron_PassWord_Data();
    void  Pri_Synchron_Result_Data();
    void  Pri_KEY_ConfSurface(QString);
    void  Pri_KEY_MainSurface(QString);
    void  Pri_SaveSystemData(QStringList);
    void  Pri_Change_Password();
    void  Pri_Sync_Data(QStringList);
    void  Pri_Sync_Color(QString);
    void  Pri_usart_analytical();
    void  Pri_can_data_analytical();
    void  Pri_Board_PG_Getfre();
    QString Pri_Board_Version();
    QStringList Pri_Board_DLR_TestData(can_frame);
    void  Pri_Send_PLC();
    void  Pri_Send_FG_Test_Enable();
    void  Pri_Mag_Trend(int *waveform_point);
public slots: 
    void  Pubs_ChangeWindow(int);
    void  Pubs_Cheak_Ready();
    void  Pubs_Cheak_Main_Finsh();
    void  Pubs_Cheak_Syst_Finsh();
    void  Pubs_from_pass(int, int);
    void  Pubs_from_data(QStringList, int, int);
    void  Pubs_from_debug(QStringList, int, int);
    void  Pubs_from_syst(QStringList, int, int);
    void  Pubs_from_test(int, int);
    void  Pubs_from_color(QString, int, int);
    void  Pubs_from_conf(QStringList, int, int);
    void  Pubs_Usart_Send(char *Buf);
    void  ReadMessage(quint16 addr,  quint16 cmd,  QByteArray msg);
    void  Pubs_UDisk_Data(int, QString);
    void  Pubs_from_pic(QString, int, int);

private slots:
    void  canSend(can_frame frame, int);
    void  Button_Main_Group_Judge(int);
    void  Mouse_Judge();
    void  System_Runing();
    void  Out_Board_Stop();
    void  Dcr_Board_Stop();
    void  Acw_Board_Stop();
    void  Jump_wTest_Surface(); 

signals:
    void  Sigmain_data_to_test(QStringList);
    void  Sigmain_data_to_syst(QString, int);
    void  Sigmain_save_DayRecoord(QString, QString);
    void  Sigmain_data_to_debug(QStringList, QString, int);
    void  Sigmain_can_to_conf(can_frame frame, QStringList, int);

    void  Serial_SendString(QString); // -输入法输入信号, 进行输入法的输入信息
    void  Serial_SendEnter();
    void  Serial_SendBackspace();
    void  Serial_SendEscape();
    void  Serial_Upspace();
    void  Serial_Downspace();
    void  Serial_Tabspace();
    void  Serial_BackTabspace();
    void  WriteMessage(quint16,  quint16,  QByteArray);
    void  Signal_main_to_pic(QStringList, int, int);
    void  Signal_main_to_Switchover(int, int, int);
    void  Singal_to_SQL(QString, QStringList, int);
};

#endif // W_MAIN_H
