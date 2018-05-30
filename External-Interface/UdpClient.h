#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QDir>
#include <QFile>
#include <QTime>
#include <time.h>
#include <QObject>
#include <QSettings>
#include <QUdpSocket>
#include <QStringList>
#include <QFile>

#include <QtXml/QDomDocument>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "define.h"

#define CMD_ITEM        0x04    //更新测试项目
#define CMD_STATUS      0x15    //状态查询
#define CMD_STATUSING   0x18    //测试中状态
#define CMD_START       0x16    //启动
#define CMD_STOP        0x17    //停止
#define CMD_Param       0x19    //参数
#define CMD_ICO         0x20    //图标

#define CMD_Login       0x21    // 登入
#define CMD_Reply       0x22    // 应答
#define CMD_WMessage    0x23    // 警告

#define CMD_CONF_Data   0x60    // 配置数据
#define CMD_Start_Item  0x61    // 启动单个项目
#define CMD_Init_Data   0x62    // 初始化数据
#define CMD_Judge       0x63    // 跳转到测试界面
#define CMD_Add_Model   0x65    // 添加测试型号
#define CMD_Del_Model   0x66    // 删除型号
#define CMD_Oder_Model  0x67    // 获取顺序
#define CMD_Updae_Model 0x68    // 切换
#define CMD_All_Start   0x69    // 总启动
#define CMD_All_Stop    0x70    // 总停止 急停
#define CMD_Time        0x71    // 超时   急停
#define CMD_ad          0x72    //
#define CMD_73          0x73    //
#define CMD_75          0x75    //
#define CMD_76          0x76    //
#define CMD_77          0x77    //
#define CMD_IO_Put      0x78    //
#define CMD_IO_IN       0x79    //
#define CMD_MAG_Sample  0x80
#define CMD_81          0x81
#define CMD_82          0x82
#define CMD_IMP_Sample  0x83
#define CMD_85          0x85
#define CMD_86          0x86
#define CMD_87          0x87
#define CMD_IMP_Ready   0x88
#define CMD_Vacuum_IMP  0x89
#define CMD_V_State     0x90
#define CMD_IMP_Finsh   0x91
#define CMD_92          0x92
#define CMD_IMP_PL      0x93
#define CMD_PIC         0x95
#define CMD_96          0x96
#define CMD_97          0x97
#define CMD_98          0x98
#define CMD_Close_Pumb  0x99 // 关闭真空泵
#define CMD_9a          0x9a
#define CMD_Vacuum_ACW  0x9b
#define CMD_Start_Auto  0x9c
#define CMD_Curtain     0x9d // 光幕
#define CMD_Action      0x9e // 样品到位
#define CMD_Start_No    0x9f //
#define CMD_Beep        0xa0 // Beep.ok Beep.ng
#define CMD_A1          0xa1
#define CMD_A2          0xa2
#define CMD_A3          0xa3

class UdpClient : public QUdpSocket
{
    Q_OBJECT
public:
    explicit UdpClient(QObject *parent = 0);


signals:
    void SendCommand(quint16 addr, quint16 cmd, QByteArray msg);

private slots:
    void Init(void);
    void Quit(void);
    void InitSettings(void);
    void SaveSettings(void);
    void ReadAll(void);
    void Analy_XML_Data(QStringList);

public slots:
    void ReadMessage(quint16 addr, quint16 cmd, QByteArray msg);

private:
    QString Number;
    QString Types;
    QString FileInUse;
    QString TxMsg;
    QString Status;
    QStringList Items;
    QHostAddress sender_Record;

    int Reply_conut; // -PC回应计数
};

#endif // UDPCLIENT_H
