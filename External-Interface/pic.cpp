#include "pic.h"

pic::pic()
{
    Test_Model_Vacuum = false;
    IO_Int = false;
    IO_in_Change = 0;
    IO_in_H = 0, IO_in_L = 0;   IO_in_H_ADD = 0;
    IO_in_H_State = 0, IO_in_L_State = 0; IO_in_H_State_ADD = 0;

    Out_Singal_H = 0;
    Out_Singal_L = 0; Out_Singal_Add = 0;

    L_Stop = false;
    R_Stop = false;

    Test_IMP = false;

    Test_IR_Finsh = true;
    Test_ACW_Finsh = true;

    L_Ready_Inhale_Air = false;
    R_Ready_Inhale_Air = false;

    System_ing = false;
    Current_Test_Window = false;

    L_Air_Press_Hold = false;
    R_Air_Press_Hold = false;

    IMP_Sample_ing = false;

    L_Judge_OK = false;
    R_Judge_OK = false;

    L_Stop_Push = false;
    R_Stop_Push = false;

    MAG_Drive = false;
    Test_MAG  = false;

    Test_Product = true;
    Test_ACW_Finsh_Default = false;
    Test_Start_Auto = false;

    MAG_Sample_ing = false;

    Test_Start_Auto_Disnable = false;
    Test_Back_Horizontal = false;

    L_Station_Ok = false;
    R_Station_Ok = false;
    L_Station_Order = false;
    R_Station_Order = false;

    Timer_Order = new QTimer(this);
    connect(Timer_Order, SIGNAL(timeout()), this, SLOT(Timer_Order_Go()));

    Timer_Cylinder_Up = new QTimer(this);
    connect(Timer_Cylinder_Up, SIGNAL(timeout()), this, SLOT(Cylinder_Up_Go()));

    Vacuum_Model = true;
    L_Cylinder_Uping = false;  R_Cylinder_Uping = false;
    L_Horizontal_Reset = true; R_Horizontal_Reset = false;

    Timer_Real = new QTimer(this);
    connect(Timer_Real, SIGNAL(timeout()), this, SLOT(Cylinder_Real_Back()));

    IO_Reset = new QTimer(this);
    connect(IO_Reset, SIGNAL(timeout()), this, SLOT(IO_Join_Reset()));
}
pic::~pic() {
    // -
}

void pic::IO_Join_Reset() {

}

void pic::Cylinder_Real_Back() {
    // qDebug() << "Cylinder_Real_Back()";
    if (IN_L_Cylinder & IO_in_L) {
        qDebug() << "Cylinder_Real_Back back";
        return;
    } else {
        //
    }
    L_Stop = false;
    if (R_Station_Order) { // 右工位预约
        Timer_Order->setSingleShot(TRUE);
        Timer_Order->start(200); // 定时器200ms, 自启动
    } else {
        //
    }
    if (Test_MAG && MAG_Drive) {
        MAG_Drive_Disable();
    } else {
        //
    }
}

void pic::Cylinder_Up_Go() {
    qDebug() << "**************************************";
    if (L_Cylinder_Uping) {
        L_Cylinder_Uping = false;
        if ((IN_Vertical_Reset) & (IO_in_H_ADD)) { // 垂直复位
            //
        } else {
            L_Vertical_Cylinder_Back();
            qDebug() << "2018-01-18---Return---L_Vertical_Cylinder_Back()";
            return;
        }
        if ((IN_Horizontal_Reset) & (IO_in_H_ADD)) { // 水平复位
            //
        } else {
            L_Horizontal_Cylinder_Back();
        }
    } else if (R_Cylinder_Uping) {
        R_Cylinder_Uping = false;
    } else {
        //
    }
}


void pic::Timer_Order_Go() {
    int Current_H;
    int Current_State;
    if (L_Station_Order) {
        Current_H = IO_in_H;
        Current_State = IO_in_H_State;

        IO_in_H_State = IN_L_Start; // 左启动
        IO_in_H = IN_L_Start;

        Get_H_In(IO_in_H_State);
        IO_in_H_State = Current_State;
        IO_in_H = Current_H;
    } else if (R_Station_Order) {
        Current_H = IO_in_H;
        Current_State = IO_in_H_State;

        IO_in_H_State = IN_R_Start; // 右启动
        IO_in_H = IN_R_Start;

        Get_H_In(IO_in_H_State);
        IO_in_H_State = Current_State;
        IO_in_H = Current_H;
    } else {
        //
    }
}

void pic::analytical_in(int value) {
    int value_H, value_L; int value_H_Add;
    value_L = value%256;
    value_H = value%65536/256;
    value_H_Add = value/65536;
    IO_in_Change = 0;
    if (IO_in_H != value_L) {
        IO_in_Change = 1;
        IO_in_H_State = ((IO_in_H)^(value_L));
        IO_in_H = value_L;
    } else {
        //
    }
    if (IO_in_H_ADD != value_H_Add) {
        IO_in_Change = 100 + IO_in_Change;
        IO_in_H_State_ADD = ((IO_in_H_ADD)^(value_H_Add));
        IO_in_H_ADD = value_H_Add;
    } else {
        //
    }
    if (IO_in_L != value_H) {
        IO_in_Change = 10 + IO_in_Change;
        IO_in_L_State = ((IO_in_L)^(value_H));
        IO_in_L = value_H;
    } else {
       //
    }

    if (!IO_Int) {
        qDebug() << "系统气缸初始化";
        IO_Int = true;
        Check_Init_State(IO_in_H, IO_in_L);
        qDebug() << "2018-01-18---Return---Check_Init_State(IO_in_H, IO_in_L)";
        return;
    } else {
        //
    }

    if (!IO_in_Change) {
        qDebug() << "2018-01-18---Return---!IO_in_Change";
        return;
    } else {
        //
    }

    if (Vacuum_Model) {
        if (IO_in_Change%10 == 1) {
            Get_H_In(IO_in_H_State);
        }
        if (IO_in_Change%100/10 == 1) {
            Get_L_In(IO_in_L_State);
        }
        if (IO_in_Change/100 == 1) {
            Get_H_In_ADD(IO_in_H_State_ADD);
        }
    }
    else {
        if (IO_in_Change%10 == 1) {
            Get_H_In_Commom(IO_in_H_State);
        }
        if (IO_in_Change%100/10 == 1) {
            Get_L_In_Commom(IO_in_L_State);
        }
        if (IO_in_Change/100 == 1) {
            Get_H_In_Commom_ADD(IO_in_H_State);
        }
    }
}

void pic::Check_Init_State(int State_H, int State_L) {
    int i;
    int State_H_Change, State_L_Change;
    if ((State_H == 146) && (State_L == 4)) {
        return;
    } else {
        //
    }
    if (State_H != 146) {
        State_H_Change = (State_H^146);
        for (i = 0; i < 8; i++) {
            if ((1 << i) & State_H_Change) {
                switch (i) {
                case 0:
                    //
                    break;
                case 1:
                    //
                    break;
                case 2: // 右光幕
                    //    F_R_Light_Curtain = true;
                    break;
                case 3:
                    //
                    break;
                case 4:
                    //
                    break;
                case 5: // 左光幕
                    //    F_L_Light_Curtain = true;
                    break;
                case 6:
                    //
                    break;
                case 7:
                    //
                    break;
                default:
                    //
                    break;
                }
            }
        }
    }
    if (State_L != 4) {
        State_L_Change = (State_L^4);
        for (i = 0; i < 8; i++) {
            if ((1 << i) & State_L_Change) {
                switch (i) {
                case 0: // 右气缸到位
                    //    R_Cylinder_Palce_OK = true; // 右真空罩到位
                    break;
                case 1:
                    //
                    break;
                case 2:
                    //
                    break;
                case 3: // 左气缸到位
                    //    L_Cylinder_Palce_OK = true; // 左真空罩到位
                    break;
                case 4:
                    Signal_pic_to_Main(QString(""), 14, 7);
                    break;
                case 5:
                    Signal_pic_to_Main(QString(""), 15, 7);
                    break;
                case 6:
                    //
                    break;
                case 7:
                    //
                    break;
                default:
                    //
                    break;
                }
            }
        }
    }
    PLC_CAN_Data(0x80, 0x02, 0x00); // 2018-01-12 真空罩上升
    usleep(1000);
    qDebug() << "IO Reset";
}

void pic::Pubs_from_main(QStringList io_List, int value, int flag) {
    if (Vacuum_Model) {
        qDebug() << "Vacuum_Model" << Vacuum_Model << "io_List" <<\
                    io_List << "value" << value << "flag" << flag;
    } else {
        qDebug() << "Return" << flag;
        if (flag == 1) {
            analytical_in(value);
        }
        return;
    }
    int Current_State;
    int Current_H;
    switch (flag) {
    case 0:
        Vacuum_Model = value; qDebug() << "0" << Vacuum_Model;
        break;
    case 1:
        analytical_in(value);
        break;
    case 2:
        L_Judge_OK = value;
        Out_Singal_H = Out_Singal_H & (~OUT_L_Station); // 断工位信号
        PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
        if (Test_Model_Vacuum) { // 放气
            if (L_Stop) {
                L_Cylinder_Up();
            } else {
                L_Exhaust_Air();
            }
        } else {
            if (Test_MAG && MAG_Drive) {
                qDebug() << "撤回反嵌装置";
                MAG_Drive_Disable();
            } else {
                //
            }
        }
        break;
    case 3:
        R_Judge_OK = value;

        Out_Singal_H = Out_Singal_H & (~OUT_R_Station);
        PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);

        if (!Test_IMP) {
            break;
        }
        if (Test_Model_Vacuum) { // 放气
            if (R_Stop) {
                R_Cylinder_Up();
            } else {
                R_Exhaust_Air();
            }
        }
        break;
    case 5:
        if (value/10) { // 耐压
            if (value%10) { // 耐压真空模式判定
                Test_ACW_Finsh_Default = true;
            } else {
                Test_ACW_Finsh_Default = false;
            }
        } else { // 匝间
            if (value%10) { // 匝间真空模式判定
                Out_Singal_L = Out_Singal_L | (OUT_Pumb); // 打开真空泵；
                PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
            } else {
                Out_Singal_L = Out_Singal_L & (~OUT_Pumb); // 关闭真空泵；
                PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
            }
        }
        break;
    case 6:
        if (value/10) {
            Test_IR_Finsh  = true;  Test_ACW_Finsh = true; // 测试绝缘完成 测试耐压完成
            System_ing = true; IMP_Sample_ing = true;
            if (value == 19) {
                L_Cylinder_Down(); L_Stop = false;
            } else if (value == 20) {
                R_Cylinder_Down(); R_Stop = false;
            } else {
                //
            }
        } else {
            System_ing = false; IMP_Sample_ing = false;
            L_Stop_Push = true; L_Exhaust_Air();
            R_Stop_Push = true; R_Exhaust_Air();
        }
        break;
    case 7:
        analytical_Item_Enable(io_List);
        break;
    case 8:
        qDebug() << "PIC----------Finsh" << value;
        if (System_ing) {
            // 测试中
        } else {
            L_Ready_Inhale_Air = false;
            R_Ready_Inhale_Air = false;
            break;
        }
        if (value == 3) {
            Test_IR_Finsh = true;
        } else if (value == 4) {
            Test_ACW_Finsh = true;
        } else {
            //
        }
        if ((Test_IR_Finsh) && (Test_ACW_Finsh)) {
            if (Test_Model_Vacuum) { // 测试匝间
                //
            } else {
                break;
            }
            if (L_Ready_Inhale_Air) {
                L_Ready_Inhale_Air = false;
                if (L_Stop) {
                    break;
                }
                L_Inhale_Air();
            } else if (R_Ready_Inhale_Air) {
                R_Ready_Inhale_Air = false;
                if (R_Stop) {
                    break;
                }
                R_Inhale_Air();
            } else {
                //
            }
        }
        break;
    case 9:
        System_ing = true;
        if (value) {
           MAG_Drive_Enable(); // 反嵌的驱动方式控制
        } else {
           MAG_Drive_Disable();
           System_ing = false;
        }
        break;
    case 10:
        //
        break;
    case 11:
        if (value/10 == 19) { // 左工位
            if (value%10 == 1) {
                L_Station_Ok = true;
            } else {
                L_Station_Ok = false;
            }
        } else if (value/10 == 20) { // 右工位
            if (value%10 == 1) {
                R_Station_Ok = true;
            } else {
                R_Station_Ok = false;
            }
        } else {
            //
        }
        if (value%10 == 1) {
            System_ing = true; // 测试中
            L_Judge_OK = false;
            R_Judge_OK = false;
        } else {
            System_ing = false; qDebug() << "System_ing  测试完成"; // 测试完成
        }
        break;
    case 12:
        qDebug() << "PIC-----------2017-12-15  ";
        if (value == 2) {
            Test_Model_Vacuum = false;
            Out_Singal_L = Out_Singal_L & (~OUT_Pumb); // 关闭真空泵；
            PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
            break;
        } else {
            //
        }
        if (value) {
            Test_Model_Vacuum = true;
            Out_Singal_L = Out_Singal_L | (OUT_Pumb); // 打开真空泵；
        } else {
            Test_Model_Vacuum = false;
            Out_Singal_L = Out_Singal_L & (~OUT_Pumb); // 关闭真空泵；
        }
        PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
        break;
    case 13:
        if (value == 1) {
            Current_Test_Window = true;
        } else {
            Current_Test_Window = false;
        }
        break;
    case 15:
        Current_H = IO_in_H;
        Current_State = IO_in_H_State;
        if (value == 1) {
            IO_in_H_State = IN_R_Start; // 右启动
            IO_in_H = IN_R_Start;
        } else {
            IO_in_H_State = IN_L_Start; // 左启动
            IO_in_H = IN_L_Start;
        }
        Get_H_In(IO_in_H_State);
        IO_in_H_State = Current_State;
        IO_in_H = Current_H;
        qDebug() << "启动完成";
        break;
    case 17:
        Current_H = IO_in_H;
        Current_State = IO_in_H_State;
        if (value == 1) {
            IO_in_H_State = IN_R_Stop; // 右停止
            IO_in_H = (IN_R_Stop);
        } else {
            IO_in_H_State = IN_L_Stop; // 左停止
            IO_in_H = (IN_L_Stop);
        }
        Get_H_In(IO_in_H_State);
        IO_in_H_State = Current_State;
        IO_in_H = Current_H;
        qDebug() << "进行完成";
        break;
    case 18:
        Signal_pic_to_Main(QString(""), Out_Singal_L*256+Out_Singal_H, 6);
        break;
    case 19:
        if (value) {
            Test_Start_Auto = true;
        } else {
            Test_Start_Auto = false;
        }
        break;
    case 20:
        //
        break;
    case 119:
        Switchover_Factory = value; // 初始获取IO配置
        if (Switchover_Factory==LvZhi) {
            qDebug()<<"绿智---初始气缸复位";
            frame.can_id  = 0x2D;  frame.can_dlc = 0x03;
            frame.data[0] = 0xf1 ;
            frame.data[1] = 0x00;
            frame.data[2] = 0x00;
            canSend(frame,88);
        }
        break;
    default:
        //
        break;
    }
}

void pic::analytical_Item_Enable(QStringList Item_List) {
    qDebug() << "2018-3-22 Item_List" << Item_List;
    if (Item_List.at(0).toInt() == 1) { // 匝间
        Test_IMP = true;
    } else {
        Test_IMP = false;
    }

    if (Item_List.at(1).toInt() == 1) { // 绝缘
        Test_IR = true;
    } else {
        Test_IR = false;
        Test_IR_Finsh = true;
    }

    if (Item_List.at(2).toInt() == 1) { // 交耐
        Test_ACW = true;
    } else {
        Test_ACW = false;
        Test_ACW_Finsh = true;
    }

    if (Item_List.at(3).toInt() == 1) { // 反嵌
        Test_MAG = true;
    } else {
        Test_MAG = false;
    }

    if (Item_List.at(4).toInt() == 1) { // 反嵌驱动-美芝项目
        MAG_Drive = true;
    } else {
        MAG_Drive = false;
    }
}

void pic::Pubs_from_Switchover(int Factory, int value, int chose) {
    qDebug() << "Switchover_Factory" << Switchover_Factory;
    qDebug() << "Factory" << Factory << "value" << value << "chose" << chose;
    if (Switchover_Factory != Factory) {
        return;
    }

    switch (chose) {
    case 0x01:
        frame.can_id  = 0x2D;  frame.can_dlc = 0x04;
        frame.data[0] = 0xf1 ;
        frame.data[1] = value % 256;
        frame.data[2] = value % 65536 / 256;
        frame.data[3] = value / 65536;
        qDebug() << "frame.data[1]" << frame.data[1] << "frame.data[2]" << frame.data[2];
        canSend(frame, 88);
        break;
    case 0x02:
        qDebug() << "绿智 初始气缸" << value - 18;
        frame.can_id  = 0x2D;  frame.can_dlc = 0x03; // 绿智 初始气缸复位
        frame.data[0] = 0xf1 ;
        frame.data[1] = value - 18;
        frame.data[2] = 0x00;
        canSend(frame, 88);
        break;
    case 0x03:
        qDebug() << "威灵" << value - 18; // +南新
        frame.can_id  = 0x2D;  frame.can_dlc = 0x03; // 绿智 初始气缸复位
        frame.data[0] = 0xf1 ;
        frame.data[1] = value - 18;
        frame.data[2] = 0x00;
        canSend(frame, 88);
        IO_Reset->setSingleShot(TRUE);
        IO_Reset->start(100); // 启动定时器,100Ms
        break;
    case 0x05:
        qDebug() << "南新" << value - 18;
        frame.can_id  = 0x2D;  frame.can_dlc = 0x03; // 南新 初始气缸复位
        frame.data[0] = 0xf1 ;
        frame.data[1] = (value - 18) * 8;
        frame.data[2] = 0x00;
        canSend(frame, 88);
        break;
    case 0x06:
        qDebug() << "日电产凯宇" << value;
        frame.can_id  = 0x2D;  frame.can_dlc = 0x03; // 日电产凯宇 下压气缸复位
        frame.data[0] = 0xf1 ;
        frame.data[1] = value;
        frame.data[2] = 0x00;
        canSend(frame, 88);
        break;
    case 0x07:
        frame.can_id  = 0x2D;  frame.can_dlc = 0x03; //
        frame.data[0] = 0xf1 ;
        frame.data[1] = value;
        frame.data[2] = 0x00;
        canSend(frame, 88);
        break;
    default:
        //
        break;
    }
}

void pic::Get_H_All_In_Commom() {
    int i;
    qDebug() << "Get_H_All_In_Commom" << "Get_H_All_In_Commom";
    for (i = 0; i < 8; i++) {
        if ((1 << i) & IO_in_H_State) {
            Get_H_In_Commom(1 << i);
        }
    }
}

void pic::Get_H_In_Commom(int in_h) {
     switch (in_h) {
     case X00:
         if (X00 & IO_in_H) {
             qDebug() << "X00 & IO_in_H)";
             Signal_pic_to_Test(QString(""), 1, 1);
         } else {
             //
         }
         break;
     case X01:
         if (X01 & IO_in_H) {
             Signal_pic_to_Test(QString(""), 2, 2);
         } else {
             //
         }
         break;
     case X02:
         if (X02 & IO_in_H) {
             Signal_pic_to_Test(QString(""), 3, 3);
         } else {
             //
         }
         break;
     case X03:
         if (X03 & IO_in_H) {
             Signal_pic_to_Test(QString(""), 4, 4);
         } else {
             //
         }
         break;
     case X04:
         if (X04 & IO_in_H) {
             Signal_pic_to_Test(QString(""), 5, 5);
         } else {
             //
         }
         break;
     case X05:
         if (X05 & IO_in_H) {
             Signal_pic_to_Test(QString(""), 1, 6);
         } else {
             Signal_pic_to_Test(QString(""), 0, 6);
         }
         break;
     case X10:
         if (X10 & IO_in_H) {
             Signal_pic_to_Test(QString(""), 1, 7);
         } else {
             Signal_pic_to_Test(QString(""), 0, 7);
         }
         break;
     default:
         Get_H_All_In_Commom();
         break;
     }
}

void pic::Get_L_In_Commom(int in_h) {
     //
}

void pic::Get_H_In_Commom_ADD(int in_h) {
    qDebug() << in_h << "in_h" << IO_in_H_ADD << "IO_in_H_ADD";
    switch (in_h) {
    case 0x08:
        if (0x08 & IO_in_H_ADD) {
            Signal_pic_to_Test(QString(""), 0, 8);
        } else {
            //
        }
        break;
    }
}

void pic::Get_H_All_In_ADD() {
    int i;
    for (i = 0; i < 8; i++) {
        if ((1 << i) & IO_in_H_State_ADD) {
            Get_H_In_ADD(1 << i);
        }
    }
}

void pic::Get_H_In_ADD(int in_h_add) {
    switch (in_h_add) {
    case IN_Vertical_Reset:
        if (IN_Vertical_Reset & IO_in_H_ADD) {
            qDebug() << "2017-12-13 IN_Vertical_Reset";
        } else {
            return;
        }

        if (System_ing) { // 防止在单个信号控制时，进行动作
            L_Horizontal_Cylinder_Back();
        } else if (Test_Back_Horizontal) {
            Test_Back_Horizontal = false;
            L_Horizontal_Cylinder_Back();
        } else if (MAG_Sample_ing) {
            MAG_Sample_ing = false;
            L_Horizontal_Cylinder_Back();
        } else {
            return;
        }
        break;
    case IN_Horizontal_Go:
        if (IN_Horizontal_Go & IO_in_H_ADD) {
            qDebug() << "-------------------------2017-12-13 IN_Horizontal_Go";
        } else {
            return;
        }
        if (System_ing) { // 防止在单个信号控制时，进行动作
            L_Vertical_Cylinder_Go();
        } else {
            return;
        }
        break;
    case IN_Horizontal_Reset:
        if (IN_Horizontal_Reset & IO_in_H_ADD) {
            qDebug() << "2018-01-22 IN_Horizontal_Reset";
            L_Horizontal_Reset = true;
        } else {
            qDebug() << "2018-01-22 !! IN_Horizontal_Reset";
            L_Horizontal_Reset = false;
            return;
        }
        if (System_ing) { // 防止在单个信号控制时，进行动作
            //
        } else {
            return;
        }
        break;
    case IN_ADD_4:
        if (IN_ADD_4 & IO_in_H_ADD) {
            qDebug() << "2017-12-13 IN_ADD_4";
        } else {
            //
        }
        break;
    default:
        Get_H_All_In_ADD();
        break;
    }
}

void pic::Get_H_All_In() {
    int i;
    for (i = 0; i < 8; i++) {
        if ((1 << i) & IO_in_H_State) {
            Get_H_In(1 << i);
        }
    }
}

void pic::Get_H_In(int in_h) {
    int Current_State;
    int Current_H;
    switch (in_h) {
    case IN_R_Light_Curtain:
        if (IN_R_Light_Curtain & IO_in_H) {
            qDebug() << "右光幕";
            if (IN_R_Cylinder & IO_in_L) { // 右真空罩到位
                //
            } else {
                R_Stop_Push = true;
                if (IMP_Sample_ing) {
                    R_Cylinder_Up();
                } else if (R_Station_Order) {
                    R_Station_Order = false;
                    R_Cylinder_Up();
                } else {
                    R_Stop = true;
                    Out_Singal_H = Out_Singal_H & (~OUT_R_Station);

                    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
                }
            }
        } else {
            qDebug() << "右光幕 离开";
        }
        break;
    case IN_L_Light_Curtain:
        if (IN_L_Light_Curtain & IO_in_H) {
            qDebug() << "左光幕";
            if (IN_L_Cylinder & IO_in_L) { // 2018-01-12 左真空罩到位
                //
            } else {
                L_Stop_Push = true;
                if (IMP_Sample_ing) {
                    L_Cylinder_Up();
                } else if (L_Station_Order) {
                    L_Station_Order = false;
                    L_Cylinder_Up();
                } else {
                    L_Stop = true;
                    Out_Singal_H = Out_Singal_H & (~OUT_L_Station);
                    Out_Singal_L = Out_Singal_L & (~OUT_L_Cylinder_Down); //
                    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
                }
            }
            if (System_ing) {
                Test_Start_Auto_Disnable = true;
            }
        } else {
            qDebug() << "左光幕 离开";
            if (Test_Start_Auto_Disnable) {
                qDebug() << "Test_Start_Auto_Disnable return";
                Test_Start_Auto_Disnable = false;
                return;
            } else {
                //
            }
            if ((Test_Product) && (Test_Start_Auto)) {
                Current_H = IO_in_H;
                Current_State = IO_in_H_State;

                IO_in_H_State = IN_L_Start; // 左启动
                IO_in_H = IN_L_Start;
                Get_H_In(IO_in_H_State);

                IO_in_H_State = Current_State;
                IO_in_H = Current_H;
            } else {
                qDebug() << "Test_Product" << Test_Product <<\
                            "Test_Start_Auto" << Test_Start_Auto;
            }
        }
        break;
    case IN_R_Start:
        if (Current_Test_Window) {
            //
        } else {
            break;
        }
        if (IN_R_Light_Curtain & IO_in_H) {
            qDebug() << "R_Light_Curtain  -----Return";
            break;
        } else {
            //
        }
        if ((R_Cylinder_Uping) || (!R_Horizontal_Reset)) {
            qDebug() << "return----R_Cylinder_Uping  ||  R_Horizontal_Reset" \
                     << R_Cylinder_Uping << R_Horizontal_Reset;
            break;
        } else {
            //
        }
        if (IN_R_Start & IO_in_H) { qDebug() << "右启动";
            if (System_ing) {
                if (R_Station_Ok) { // 当前工位测试中
                    break;
                } else {
                    R_Station_Order = true; // 右工位进入预约状态->右工位真空泵下落
                    if (Test_Model_Vacuum) {
                        R_Ready_Inhale_Air = false;
                        R_Cylinder_Down();
                        R_Stop_Push = false;
                        R_Stop = false;
                    } else {
                        //
                    }
                    break;
                }
            } else {
                //
            }
            if (IN_R_Cylinder & IO_in_L) {
                break;
            } else {
                //
            }

            R_Station_Order = false;
            R_Stop = false;
            R_Station_Start();
            if (Test_Model_Vacuum) {
                R_Ready_Inhale_Air = false;
                R_Cylinder_Down();
                R_Stop_Push = false;
                R_Stop = false;
            } else {
                //
            }
            if (Test_IR) {
                Test_IR_Finsh = false;
            } else {
                //
            }
            if (Test_ACW) {
                Test_ACW_Finsh = false;
            } else {
                //
            }
        } else {
            //
        }
        break;
    case IN_L_Start:
        qDebug() << "1";
        if (Current_Test_Window) {
            qDebug() << "2";//
        } else {
            qDebug() << "3";//
            break;
        }
        if (IN_L_Light_Curtain & IO_in_H) {
            qDebug() << "L_Light_Curtain -----Return";
            break;
        } else {
            //
        }
        if ((L_Cylinder_Uping) || (!L_Horizontal_Reset)) {
            qDebug() << "return----L_Cylinder_Uping  ||  L_Horizontal_Reset" \
                     << L_Cylinder_Uping << L_Horizontal_Reset;
            break;
        } else {
            //
        }
        if (Test_Product) {
            qDebug() << "Test_Product -----Return";
        } else {
            qDebug() << "4";//
            break;
        }
        if (IN_L_Start & IO_in_H) { qDebug() << "左启动";
            if (System_ing) {
                if (L_Station_Ok) { // 当前工位测试中
                    qDebug() << "5";
                    break;
                } else {
                    L_Station_Order = true; // 左工位进入预约状态->左工位真空泵下落
                    if (Test_Model_Vacuum) {
                        L_Ready_Inhale_Air = false;
                        L_Cylinder_Down();
                        L_Stop_Push = false;
                        L_Stop = false;
                    } else {
                        //
                    }
                    qDebug() << "6";
                    break;
                }
            } else {
                //
            }
            if (IN_L_Cylinder & IO_in_L) {
                qDebug() << "7";
                break;
            } else {
                //
            }
            L_Station_Order = false;
            L_Stop = false;
            L_Station_Start();
            if (Test_Model_Vacuum) {
                L_Ready_Inhale_Air = false;
                L_Cylinder_Down();
                L_Stop_Push = false;
                L_Stop = false;
            } else {
                if (Test_MAG && MAG_Drive) {
                    qDebug() << "启动反嵌装置";
                    MAG_Drive_Enable();
                } else {
                    qDebug() << "8";
                    //
                }
            }
            if (Test_IR) {
                Test_IR_Finsh = false;
            } else {
                //
            }
            if (Test_ACW) {
                Test_ACW_Finsh = false;
            } else {
                //
            }
            if (Test_ACW_Finsh_Default) {
                Test_ACW_Finsh = true;
            } else {
                // -
            }
        } else {
            // -
        }
        break;
    case IN_L_Stop:
        if ((IN_L_Stop & IO_in_H)) { // !
            qDebug() << "左停止";
            L_Stop_Push = true;
            L_Station_Order = false;
            if (IN_L_Cylinder & IO_in_L) { // 2018-01-12 左真空罩到位
                // -
            } else {
                L_Stop = true;
            }
            if (Out_Singal_H & OUT_L_Station) {
                Out_Singal_H = Out_Singal_H & (~OUT_L_Station);
                Out_Singal_L = Out_Singal_L & (~OUT_L_Cylinder_Down); //
                PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
            } else {
                qDebug() << "左工位 滑罩已经断开";
                L_Judge_OK = true;
                if (IN_L_Cylinder & IO_in_L) { // 判断气缸是否下面
                    qDebug() << "1";
                    L_Exhaust_Air();
                    return;
                } else {
                    qDebug() << "2";
                }

                if (IN_L_Exhaust_Volt & IO_in_L) {
                    qDebug() << "3";
                } else {
                    qDebug() << "4";
                    L_Exhaust_Air();
                    return;
                }

                qDebug() << "IO_in_H_ADD" << IO_in_H_ADD;
                if ((IN_Vertical_Reset) & (IO_in_H_ADD)) { // 垂直复位
                    // -
                } else {
                    L_Vertical_Cylinder_Back();
                    return;
                }
                if ((IN_Horizontal_Reset) & (IO_in_H_ADD)) { // 水平复位
                    // -
                } else {
                    L_Horizontal_Cylinder_Back();
                }
            }
        } else {
            qDebug() << "测试----";
        }
        break;
    case IN_R_Stop:
        if (!(IN_R_Stop & IO_in_H)) {
            qDebug() << "右停止";
            R_Stop_Push = true;
            R_Station_Order = false;
            if (IN_R_Cylinder & IO_in_L) { // 右真空罩到位
                // -
            } else {
                R_Stop = true;
            }
            if (Out_Singal_H & OUT_R_Station) {
                Out_Singal_H = Out_Singal_H & (~OUT_R_Station);

                PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
            } else {
                qDebug() << "右工位 滑罩已经断开";
                R_Judge_OK = true;
                R_Exhaust_Air();
            }
        }
        break;
    case IN_R_Inhale_Volt:
        if (IN_R_Inhale_Volt & IO_in_H) {
            if (L_Air_Press_Hold) {
                Out_Singal_H = Out_Singal_H & (~OUT_R_Inhale_Air);

                PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
                break;
            } else {
                //
            }
            qDebug() << "右气压 Right Air Pressure ok";
            Out_Singal_H = Out_Singal_H & (~OUT_R_Inhale_Air);

            PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
            Signal_pic_to_Test(QString(""), 1, 101);
            Signal_pic_to_Main(QString(""), 5, 5);
        } else {
            qDebug() << "右气压 false";
            if (R_Air_Press_Hold) {
                R_Inhale_Air();
            }
        }
        break;
    case IN_R_Exhaust_Volt:
        if (IN_R_Exhaust_Volt & IO_in_H) {
            qDebug() << "右气压 放气 Right Air Pressure Exhaust ok";
            R_Cylinder_Up();
        } else {
            qDebug() << "右气压 放气 false";
        }
        break;
    default:
        qDebug() << "Error---Get_H_In";
        Get_H_All_In();
        break;
    }
}

void pic::Get_L_All_In() {
    int i;
    for (i = 0; i < 8; i++) {
        if ((1 << i) & IO_in_L_State) {
            Get_L_In(1 << i);
        }
    }
}

void pic::Get_L_In(int in_l) {
    int i;
    switch (in_l) {
    case IN_R_Cylinder:
        qDebug() << "55";
        if (IN_R_Cylinder & IO_in_L) {
            if (System_ing) {
                if (R_Station_Order) {
                    break;
                } else {
                    // -
                }
            } else {
                qDebug() << "22";
                break;
            }
            if (R_Stop) {
                qDebug() << "33";
                break;
            }
            qDebug() << "右气缸到位";
            R_Stop = false;
            if ((Test_IR_Finsh) && (Test_ACW_Finsh)) {
                // -
            } else {
                qDebug() << "IR ACW-----no finsh";
                R_Ready_Inhale_Air = true;
                break;
            }
            if (Test_Model_Vacuum) { // 右气缸到位——————>抽气
                R_Inhale_Air();
            }
        } else {
            qDebug() << "右气缸---回";
            R_Stop = false;
            if (L_Station_Order) { // 左工位预约
                Timer_Order->setSingleShot(TRUE);
                Timer_Order->start(200); // 定时器200ms, 自启动
            } else {
                // -
            }
        }
        break;
    case IN_L_Cylinder:
        if (IN_L_Cylinder & IO_in_L) {
            qDebug() << "左气缸到位---1";
            if (System_ing) {
                if (L_Station_Order) {
                    break;
                } else {
                    //
                }
            } else {
                break;
            }
            if (L_Stop) {
                break;
            }
            qDebug() << "左气缸到位";
            L_Stop = false;
            if ((Test_IR_Finsh) && (Test_ACW_Finsh)) {
                //
            } else {
                L_Ready_Inhale_Air = true;
                qDebug() << "IR ACW-----no finsh" << Test_IR_Finsh << Test_ACW_Finsh;
                break;
            }
            if (Test_Model_Vacuum) { // 左气缸到位——————>抽气
                L_Inhale_Air();
            }
        } else {
            qDebug() << "左气缸---回 300";
            Timer_Real->setSingleShot(TRUE);
            Timer_Real->start(300);
        }
        break;

    case IN_L_Inhale_Volt:
        if (IN_L_Inhale_Volt & IO_in_L) {
            if (L_Air_Press_Hold) {
                Out_Singal_H = Out_Singal_H & (~OUT_L_Inhale_Air);

                PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
                break;
            } else {
                //
            }
            L_Air_Press_Hold = true;
            qDebug() << "左气压 Left Air Pressure ok";
            Out_Singal_H = Out_Singal_H & (~OUT_L_Inhale_Air);
            PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
            Signal_pic_to_Test(QString(""), 1, 101);
            Signal_pic_to_Main(QString(""), 3, 3);
        } else {
            qDebug() << "左气压 false";
            if (L_Air_Press_Hold) {
                L_Inhale_Air();
            }
        }
        break;
    case IN_L_Exhaust_Volt:
        if (IN_L_Exhaust_Volt & IO_in_L) {
            qDebug() << "左气压 放气 Left Air Pressure Exhaust ok";
            L_Cylinder_Up();
        } else {
            qDebug() << "左气压 放气 false";
        }
        break;
    case IN_Error_1:
        if (IN_Error_1 & IO_in_L) {
            Signal_pic_to_Main(QString(""), 14, 7);
        }
        break;
    case IN_Error_2:
        if (IN_Error_2 & IO_in_L) {
            Signal_pic_to_Main(QString(""), 15, 7);
        }
        break;
    case IN_Motor_Ready:
        if (IN_Motor_Ready & IO_in_L) { // 电机样品到位
            Test_Product = true; qDebug() << "IN_Motor_Ready";
        } else {
            Test_Product = false;
            Signal_pic_to_Main(QString(""), 8, 8);
        }
        break;
    case IN_Vertical_GO:
        if (System_ing) { // 防止在单个信号控制时，进行动作
            //
        } else {
            return;
        }
        if (IN_Vertical_GO & IO_in_L) { // 步进电机到位
            qDebug() << "IN_Vertical_GO";
            MAG_Drive_Ready();
        } else {
            //
        }
        break;
    default:
        qDebug() << "Error---Get_L_In";
        Get_L_All_In();
        break;
    }
}

void pic::MAG_Drive_Ready() { //
    qDebug() << "2017-12-07 MAG_Drive_Ready()";
    if (IMP_Sample_ing) { // 测试匝间中
        L_Cylinder_Down_True();
        return;
    } else {
        //
    }
    if (Current_Test_Window) {
        Signal_pic_to_Test(QString(""), 2, 102); // 继续执行操作，真空装置就位
    } else {
        Signal_pic_to_Main(QString(""), 3, 3);
        return;
    }
    if (Test_Model_Vacuum) { // 真空状态, 落下气缸
        L_Cylinder_Down_True();
    } else { // 非真空状态
        // -
    }
}

void pic::R_Station_Start() {
    Out_Singal_H = Out_Singal_H | OUT_R_Station; //
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}
void pic::R_Cylinder_Down() {
    if (IN_R_Cylinder & IO_in_L) { // R真空泵下降到位
        qDebug() << "右真空泵已经到位";
        if ((Test_IR_Finsh) && (Test_ACW_Finsh)) {
            //
        } else {
            qDebug() << "IR ACW-----no finsh";
            R_Ready_Inhale_Air = true;
            return;
        }
        if (Test_Model_Vacuum) { // 右气缸到位——————>抽气
            R_Inhale_Air();
        }
        return;
    } else {
        // -
    }
    Out_Singal_H = Out_Singal_H & (~OUT_R_Exhaust_Air);
    Out_Singal_H = Out_Singal_H | OUT_R_Cylinder_Down;
    Out_Singal_H = Out_Singal_H & (~OUT_R_Cylinder_Up);
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}
void pic::R_Cylinder_Up() {
    if (R_Stop_Push) {
        qDebug() << "R_Stop_Push" << R_Stop_Push << "R_Judge_OK" << R_Judge_OK;
        R_Stop_Push = false;
        R_Judge_OK = true;
    }
    if (IN_R_Exhaust_Volt & IO_in_H) {
        //
    } else {
        qDebug() << "Error------------------------------R_Cylinder_Up()";
        return;
    }
    if (R_Judge_OK) {
        //
    } else {
        qDebug() << "R_Judge_OK false";
        return;
    }
    Out_Singal_H = Out_Singal_H | OUT_R_Cylinder_Up;
    Out_Singal_H = Out_Singal_H & (~OUT_R_Cylinder_Down);
    Out_Singal_H = Out_Singal_H & (~OUT_R_Station);
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
    qDebug() << "启动定时器";
    R_Cylinder_Uping = true;
    Timer_Cylinder_Up->setSingleShot(TRUE);
    Timer_Cylinder_Up->start(1500); // 定时器1500ms, 自启动
}
void pic::R_Inhale_Air() {
    if (IN_R_Cylinder & IO_in_L) {
        //
    } else {
        qDebug() << "Error------------------------------R_Inhale_Air()";
        return;
    }
    Out_Singal_H = Out_Singal_H | OUT_R_Inhale_Air;
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}
void pic::R_Exhaust_Air() {
    R_Air_Press_Hold = false;
    if (IN_R_Exhaust_Volt & IO_in_H) {
        R_Cylinder_Up();
        qDebug() << "2018-01-18---Return--- L_Judge_OK else";
        return;
    }
    Out_Singal_H = Out_Singal_H & (~OUT_R_Inhale_Air);
    Out_Singal_H = Out_Singal_H | OUT_R_Exhaust_Air;
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}

void pic::L_Station_Start() {
    Out_Singal_H = Out_Singal_H | OUT_L_Station;
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}
void pic::L_Cylinder_Down() {
    if (MAG_Drive) {
        qDebug() << "PIC 2017-12-20降落真空，必须先落下反嵌装置";
        MAG_Drive_Enable();
        qDebug() << "2018-01-18---Return--- MAG_Drive_Enable()";
        return;
    } else {
        L_Cylinder_Down_True();
    }
}
void pic::L_Cylinder_Down_True() {
    if (IN_L_Cylinder & IO_in_L) { // L真空泵下降到位
        if ((Test_IR_Finsh) && (Test_ACW_Finsh)) {
            //
        } else {
            L_Ready_Inhale_Air = true;
            qDebug() << "2018-01-18---Return--- L_Ready_Inhale_Air = true else";
            return;
        }
        if (Test_Model_Vacuum) { // 左气缸到位——————>抽气
            L_Inhale_Air();
        }
        qDebug() << "2018-01-18---Return--- L_Judge_OK else";
        return;
    } else {
        //
    }
    Out_Singal_H = Out_Singal_H & (~OUT_L_Exhaust_Air);
    Out_Singal_L = Out_Singal_L | OUT_L_Cylinder_Down;
    Out_Singal_L = Out_Singal_L & (~OUT_L_Cylinder_Up);
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}

void pic::L_Cylinder_Up() {
    if (L_Stop_Push) {
        qDebug() << "L_Stop_Push" << L_Stop_Push << "L_Judge_OK" << L_Judge_OK;
        L_Stop_Push = false;
        L_Judge_OK = true;
    }
    if (IN_L_Exhaust_Volt & IO_in_L) {
        //
    } else {
        qDebug() << "2018-01-18---Return--- IN_L_Exhaust_Volt & IO_in_L else";
        return;
    }
    if (L_Judge_OK) {
        //
    } else {
        qDebug() << "2018-01-18---Return--- L_Judge_OK else";
        return;
    }
    Out_Singal_L = Out_Singal_L | OUT_L_Cylinder_Up;
    Out_Singal_L = Out_Singal_L & (~OUT_L_Cylinder_Down);
    Out_Singal_H = Out_Singal_H & (~OUT_L_Station);
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
    qDebug() << "启动定时器";
    L_Cylinder_Uping = true;
    Timer_Cylinder_Up->setSingleShot(TRUE);
    Timer_Cylinder_Up->start(1500); // 定时器1500ms, 自启动
}
void pic::L_Inhale_Air() {
    if (IN_L_Cylinder & IO_in_L) {
        qDebug() << " ";
    } else {
        qDebug() << "2018-01-18---Return--- if (IN_L_Cylinder & IO_in_L) else";
        return;
    }
    /* 没有真空泵时进行测试
   * Signal_pic_to_Test(QString(""), 1, 101);
     Signal_pic_to_Main(QString(""), 3, 3); */
    qDebug() << "L_Inhale_Air()";
    Out_Singal_H = Out_Singal_H | OUT_L_Inhale_Air;
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}
void pic::L_Exhaust_Air() {
    L_Air_Press_Hold = false;
    if (IN_L_Exhaust_Volt & IO_in_L) { // 左排气 ok
        qDebug() << "L_Exhaust_Air() L_Cylinder_Up();";
        L_Cylinder_Up();
        qDebug() << "2018-01-18---Return--- L_Cylinder_Up()";
        return;
    }
    Out_Singal_H = Out_Singal_H & (~OUT_L_Inhale_Air);
    Out_Singal_H = Out_Singal_H | OUT_L_Exhaust_Air;
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}

void pic::MAG_Drive_Enable() {
    qDebug() << "MAG_Drive_Enable()";
    if (IN_Vertical_GO & IO_in_L) {
        MAG_Drive_Ready();
    } else {
        //
    }
    L_Horizontal_Cylinder_Go();
}

void pic::MAG_Drive_Disable() {
    qDebug() << "2017-12-07 MAG_Drive_Disable";
    if (MAG_Drive) {
        //
    } else {
        qDebug() << "2018-01-18---Return--- MAG_Drive else";
        return;
    }
    qDebug() << "2017-12-07 MAG_Drive_Disable Go";
    if (IN_L_Cylinder & IO_in_L) { // 判断气缸是否下面
        qDebug() << "2017-12-07 左气缸未升起------危险";
        L_Exhaust_Air();
        qDebug() << "2018-01-18---Return--- L_Exhaust_Air()";
        return;
    } else {
        //
    }
    MAG_Sample_ing = true;
    L_Vertical_Cylinder_Back();
}

/*************横向气缸->Gｏ*************/
void pic::L_Horizontal_Cylinder_Go() {
    Out_Singal_L = Out_Singal_L | OUT_Horizontal;
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}

/*************横向气缸->Back*************/
void pic::L_Horizontal_Cylinder_Back() {
    Out_Singal_L = Out_Singal_L & (~OUT_Horizontal);
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}

/*************纵向气缸->Gｏ*************/
void pic::L_Vertical_Cylinder_Go() {
    Out_Singal_L = Out_Singal_L | OUT_Vertical;
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
}

/*************纵向气缸->Back*************/
void pic::L_Vertical_Cylinder_Back() {
    Out_Singal_L = Out_Singal_L & (~OUT_Vertical);
    PLC_CAN_Data(Out_Singal_H, Out_Singal_L, Out_Singal_Add);
    Test_Back_Horizontal = true;
}

/******************************************************************************
      * version:    1.0
      * author:     sl
      * date:       2017.12.01
      * brief:      Can发送ＩＯ版信息
******************************************************************************/
void pic::PLC_CAN_Data(int Data_1, int Data_2, int Data_3) {
    qDebug() << "PIC 2017-12-20 ---------------------------->" << Data_1 << Data_2 << Data_3;
    frame.can_id  = 0x2D;  frame.can_dlc = 0x04;
    frame.data[0] = 0xf1;
    frame.data[1] = Data_1;
    frame.data[2] = Data_2;
    frame.data[3] = Data_3;
    canSend(frame, 88);
    usleep(600);
}

