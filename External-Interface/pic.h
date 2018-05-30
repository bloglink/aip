#ifndef PIC_H
#define PIC_H

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include "define.h"
#include "linux_can.h"

#define OUT_R_Station       0x01 // 右工位信号---滑罩
#define OUT_R_Inhale_Air    0x02 // 右工位抽气
#define OUT_R_Exhaust_Air   0x04 // 右工位排气
#define OUT_L_Station       0x08 // 左工位信号---滑罩
#define OUT_L_Inhale_Air    0x10 // 左工位抽气
#define OUT_L_Exhaust_Air   0x20 // 左工位排气
#define OUT_R_Cylinder_Down 0x80 // 右工位气缸上升
#define OUT_R_Cylinder_Up   0x40 // 右工位气缸下降

#define OUT_1               0x01 //
#define OUT_L_Cylinder_Down 0x04 // 左工位气缸下降
#define OUT_L_Cylinder_Up   0x02 // 左工位气缸上升
#define OUT_2               0x08 //
#define OUT_Pumb            0x10 // 真空泵
#define OUT_Horizontal      0x20 // 横向气缸
#define OUT_Vertical        0x80 // 纵向气缸
#define OUT_5               0x40 //

#define OUT_ADD_1           0x01 //
#define OUT_ADD_2           0x02 //
#define OUT_ADD_3           0x04 //
#define OUT_ADD_4           0x08 //
#define OUT_ADD_5           0x10 //
#define OUT_ADD_6           0x20 //

#define IN_R_Start         0x01 // 右工位启动信号
#define IN_R_Stop          0x02 // 右工位停止
#define IN_R_Light_Curtain 0x04 // 右工位光幕
#define IN_L_Start         0x08 // 左工位启动信号
#define IN_L_Stop          0x10 // 左工位停止
#define IN_L_Light_Curtain 0x20 // 左工位光幕
#define IN_R_Inhale_Volt   0x40 // 右工位气压到位
#define IN_R_Exhaust_Volt  0x80 // 右工位气压到位 放气

#define IN_R_Cylinder      0x01 // 右工位气缸到位
#define IN_L_Inhale_Volt   0x02 // 左工位气压到位
#define IN_L_Exhaust_Volt  0x04 // 左工位气压到位 放气
#define IN_L_Cylinder      0x08 // 左工位气缸到位
#define IN_Error_1         0x10 // 相序错误
#define IN_Error_2         0x20 // 热继电器信号
#define IN_Motor_Ready     0x40 // 电机到位信号
#define IN_Vertical_GO     0x80 // 反嵌装置-垂直到位

#define IN_Vertical_Reset   0x01 // 反嵌装置-垂直复位
#define IN_Horizontal_Go    0x02 // 反嵌装置-横向到位
#define IN_Horizontal_Reset 0x04 // 反嵌装置-横向复位
#define IN_ADD_4            0x08 //

#define X00                0x01 //
#define X01                0x02 //
#define X02                0x04 //
#define X03                0x08 //
#define X04                0x10 //
#define X05                0x20 //
#define X10                0x40 //
#define X11                0x80 //

#define X12                0x01 //
#define X13                0x02 //
#define X14                0x04 //
#define X15                0x08 //
#define X20                0x10 //
#define X21                0x20 //
#define X22                0x40 //
#define X23                0x80 //
class pic : public QObject
{
    Q_OBJECT
public:
    pic();
    virtual ~pic();

private:
    LINUX_CAN   can; // -can通信
    can_frame frame; // -can数据信息

    bool Test_IMP;
    bool Test_IR;   bool Test_IR_Finsh;
    bool Test_ACW;  bool Test_ACW_Finsh;
    bool Test_ACW_Finsh_Default;
    bool Test_MAG;

    bool MAG_Drive;
    bool Test_Model_Vacuum;
    bool IO_Int;
    bool System_ing;
    bool Current_Test_Window;
    bool L_Air_Press_Hold;
    bool R_Air_Press_Hold;
    bool IMP_Sample_ing;
    bool MAG_Sample_ing;

    bool L_Ready_Inhale_Air;
    bool R_Ready_Inhale_Air;
    int  IO_in_Change;
    int  IO_in_H, IO_in_L; int IO_in_H_ADD;
    int  IO_in_H_State, IO_in_L_State;  int IO_in_H_State_ADD;

    int  Out_Singal_H, Out_Singal_L;
    int  Out_Singal_Add; // 添加信号

    bool L_Stop;
    bool R_Stop;
    bool L_Judge_OK;
    bool R_Judge_OK;
    bool L_Stop_Push;
    bool R_Stop_Push;
    bool Test_Product;
    bool Test_Start_Auto;

    bool Test_Start_Auto_Disnable;
    bool Test_Back_Horizontal;
    bool L_Station_Ok;    bool R_Station_Ok;
    bool L_Station_Order; bool R_Station_Order;
    QTimer *Timer_Order;       //-预约定时
    QTimer *Timer_Cylinder_Up; //-气缸上升复位？
    QTimer *Timer_Real;
    QTimer *IO_Reset;
    bool L_Cylinder_Uping;   bool R_Cylinder_Uping;
    bool L_Horizontal_Reset; bool R_Horizontal_Reset;
    bool Vacuum_Model;
    int  Switchover_Factory;

    void analytical_in(int);
    void Check_Init_State(int, int);
    void Get_H_In(int); void Get_H_In_ADD(int);
    void Get_L_In(int);
    void Get_H_All_In(); void Get_H_All_In_ADD();
    void Get_L_All_In();

    void Get_H_In_Commom(int); void Get_H_In_Commom_ADD(int);
    void Get_L_In_Commom(int);
    void Get_H_All_In_Commom(); void Get_H_All_In_Commom_ADD();
    void Get_L_All_In_Commom();

    void R_Station_Start();
    void R_Cylinder_Down();
    void R_Cylinder_Up();
    void R_Inhale_Air();
    void R_Exhaust_Air();

    void L_Station_Start();
    void L_Cylinder_Down(); void L_Cylinder_Down_True();
    void L_Cylinder_Up();
    void L_Inhale_Air();
    void L_Exhaust_Air();

    void L_Horizontal_Cylinder_Go();
    void L_Horizontal_Cylinder_Back();
    void L_Vertical_Cylinder_Go();
    void L_Vertical_Cylinder_Back();

    void analytical_Item_Enable(QStringList);
    void MAG_Drive_Enable();
    void MAG_Drive_Disable();
    void MAG_Drive_Ready();

    void PLC_CAN_Data(int Data_1, int Data_2, int Data_3);
signals:
    void canSend(can_frame frame, int);
    void Signal_pic_to_Test(QString, int, int);
    void Signal_pic_to_Main(QString, int, int);

public slots:
    void Pubs_from_main(QStringList, int, int);
    void Pubs_from_Switchover(int, int, int);

private slots:
    void Timer_Order_Go();
    void Cylinder_Up_Go();
    void Cylinder_Real_Back();
    void IO_Join_Reset();
};

#endif // PIC_H
