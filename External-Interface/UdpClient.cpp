#include "UdpClient.h"

UdpClient::UdpClient(QObject *parent) : QUdpSocket(parent)
{
    Status = "free";
    Number = "----";
    Reply_conut = 0;
}

void UdpClient::Init()
{
    this->bind(6000); // QHostAddress::Broadcast,
    connect(this,  SIGNAL(readyRead()),  this,  SLOT(ReadAll()));
}

void UdpClient::Quit()
{
    this->close();
}

void UdpClient::InitSettings()
{
    qDebug() << QTime::currentTime().toString() << "Read UDP";

    QSettings *set_Test_File = new QSettings(Sys_path,   QSettings::IniFormat);
    set_Test_File->setIniCodec("GB18030");
    Number = set_Test_File->value("Factory/text",  "V-0.0.0").toString();

    QSettings File_Types(Test_File_path,   QSettings::IniFormat);
    File_Types.setIniCodec("UTF-8");
    QStringList Type_list = File_Types.value("DateFile/currentfile").toStringList();
    Types = Type_list.join(" ");
    qDebug() << QTime::currentTime().toString() << "Read UDP OK";
}

void UdpClient::SaveSettings() {
    //
}

void UdpClient::ReadAll()
{
    while (this->hasPendingDatagrams()) {
        QByteArray msg;
        msg.resize(this->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        this->readDatagram(msg.data(),  msg.size(),  &sender,  &senderPort);

        QStringList n = QString(msg).split(" ");
        quint16 Command;
        Command = quint16(n.at(0).toInt());
        n.removeFirst();
        QByteArray Param;
        Param = n.join(" ").toUtf8();
        sender_Record = sender;

        if (Command != 6030) {
            qDebug() << "Command-------------------->" << Command << msg;
        }
        switch (Command) {
        case 3000: //查询在线主机 ---成功---
            InitSettings();
            TxMsg = "3001 ";
            TxMsg.append(Number);
            this->writeDatagram(TxMsg.toUtf8(),  sender,  6000);
            break;
        case 3002: //修改主机编号

            break;
        case 3004: //获取测试型号 ---成功---
            InitSettings();
            TxMsg = "3005 ";
            TxMsg.append(Types);
            this->writeDatagram(TxMsg.toUtf8(),  sender,  6000);
            break;
        case 3006: //设置测试型号 ---成功---
            if (!Param.isEmpty())
                emit SendCommand(6000,  CMD_Param,  Param);
            break;
        case 3008: //启动左工位测试  ---成功---
            emit SendCommand(6000,  CMD_START,  QString("%1 %2").arg(0x13).arg(0x03).toUtf8());
            TxMsg = "3009 ";
            TxMsg.append(Number);
            this->writeDatagram(TxMsg.toUtf8(),  sender,  senderPort);
            break;
        case 3010: //停止测试  ---成功---
            emit SendCommand(6000,  CMD_STOP,  QString("%1 %2").arg(0x13).arg(0x03).toUtf8());
            TxMsg = "3011 ";
            TxMsg.append(Number);
            this->writeDatagram(TxMsg.toUtf8(),  sender,  senderPort);
            break;
        case 3012: //获取状态  ---成功---
            emit SendCommand(6000,  CMD_STATUSING,  QString("").toUtf8());
            break;
        case 3014:
//            TxMsg = "3015 ";
//            TxMsg.append(Items.join("\n"));
//            this->writeDatagram(TxMsg.toUtf8(),  sender,  senderPort);
            break;
        case 1666:
            qDebug() << "PC Asking";
            emit SendCommand(6000,  CMD_ICO,  QString("").toUtf8());
            break;
        case 2000:
            Reply_conut = 0;
            break;
        case 6000: // 获取警告
            SendCommand(6000,  CMD_WMessage,  QString("").toUtf8());
            break;
        case 6002:
            qDebug() << "msg---init" << msg;
            SendCommand(6000,  CMD_Init_Data,  msg);
            break;
        case 6004: // 解析接收数据
            Analy_XML_Data(n);
            break;
        case 6006: // 解析启动数据
            emit SendCommand(6000, CMD_Start_Item, msg);
            break;
        case 6008: // 跳转测试界面
            emit SendCommand(6000,  CMD_Judge,  QString("").toUtf8());
            break;
        case 6010: //-添加型号
            emit SendCommand(6000,  CMD_Add_Model,  msg);
            break;
        case 6012: //-删除型号
            emit SendCommand(6000,  CMD_Del_Model,  msg);
            break;
        case 6016:
            emit SendCommand(6000,  CMD_Oder_Model,  QString("").toUtf8());
            break;
        case 6018:
            emit SendCommand(6000,  CMD_Updae_Model,  msg);
            break;
        case 6020:
            emit SendCommand(6000,  CMD_All_Start,  msg);
            break;
        case 6022:
            emit SendCommand(6000,  CMD_All_Stop,  msg);
            break;
        case 6026:
            emit SendCommand(6000,  CMD_Time,  msg);
            break;

        case 6030:
            this->writeDatagram("0x31",  sender_Record,  6000);
            break;
        case 6036:
            emit SendCommand(6000,  CMD_IO_Put,  msg);
            break;
        case 6038:
            emit SendCommand(6000,  CMD_MAG_Sample,  msg);
            break;
        case 6041:
            emit SendCommand(6000,  CMD_IMP_Sample,  msg);
            break;
        case 6046:
            emit SendCommand(6000,  CMD_IMP_Finsh,  msg);
            break;
        case 6050:
            emit SendCommand(6000,  CMD_IMP_Ready,  msg);
            break;
        case 6052:
            emit SendCommand(6000,  CMD_Vacuum_IMP,  msg);
            break;
        case 6056:
            emit SendCommand(6000,  CMD_IMP_PL,  msg);
            break;
        case 6058:
            emit SendCommand(6000,  CMD_PIC,  msg);
            break;
        case 6066:
             emit SendCommand(6000,  CMD_9a,  msg);
            break;
        case 6067:
            emit SendCommand(6000,  CMD_Start_Auto,  msg);
            break;
        case 6068: //
            emit SendCommand(6000,  CMD_Vacuum_ACW,  msg);
            break;
        case 6070:
            emit SendCommand(6000,  CMD_Start_No,  msg);
            break;
        case 6071:
            emit SendCommand(6000,  CMD_Beep,  msg);
            break;
        case 6072:
            emit SendCommand(6000,  CMD_A1,  msg);
            break;
        case 6073:
            emit SendCommand(6000,  CMD_A2,  msg);
            break;
        case 6078:
            emit SendCommand(6000,  CMD_A3,  msg);
            break;
        default:
            this->writeDatagram("Error command",  sender_Record,  6000);
            break;
        }
    }
}

void UdpClient::Analy_XML_Data(QStringList data)
{
    if (data.at(0) == QString(tr("Conf"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("1").toUtf8());
    } else if (data.at(0) == QString(tr("Sys"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("2").toUtf8());
    } else if (data.at(0) == QString(tr("DCR"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("3").toUtf8());
    } else if (data.at(0) == QString(tr("MAG"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("15").toUtf8());
    } else if (data.at(0) == QString(tr("IR"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("4").toUtf8());
    } else if (data.at(0) == QString(tr("ACW"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("5").toUtf8());
    } else if (data.at(0) == QString(tr("IMP"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("6").toUtf8());
    } else if (data.at(0) == QString(tr("IND"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("7").toUtf8());
    } else if (data.at(0) == QString(tr("HALL"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("11").toUtf8());
    } else if (data.at(0) == QString(tr("LOAD"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("12").toUtf8());
    } else if (data.at(0) == QString(tr("NOLOAD"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("13").toUtf8());
    } else if (data.at(0) == QString(tr("BEMF"))) {
        SendCommand(6000,  CMD_CONF_Data,  QString("14").toUtf8());
    } else {
        qDebug() << "Error data--Analy_XML_Data" << data;
    }
}


void UdpClient::ReadMessage(quint16 addr,   quint16 cmd,   QByteArray msg)
{
  //  qDebug()<<"cmd"<<cmd<<"msg"<<msg;
    if (addr != 6000)
        return;
    switch (cmd) {
    case CMD_ITEM:
        Items.append(msg);
        break;
    case CMD_STATUS:
        Status = msg;
        if (Status  ==  "free") {
            this->writeDatagram(Items.join("\n").toUtf8(),  sender_Record,  6000);
            // QHostAddress::Broadcast
            Items.clear();
        } else {
            //
        }
        break;
    case CMD_STATUSING:
        Status = msg;
        TxMsg = "3013 "; TxMsg.append(Status);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_Param:
        TxMsg = "3007 ";
        TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_Login:
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_Reply:
        Reply_conut++;
        TxMsg = "2000 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        qDebug() << "Rleply_conut" << Reply_conut;
        if (Reply_conut > 30) { // >30s
            qDebug() << "Realy Fail";
        }
        break;
    case CMD_WMessage:
        TxMsg = "6001 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_CONF_Data:
        TxMsg = "6005 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_Start_Item:
        TxMsg = "6007 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_Oder_Model:
        TxMsg = "6017 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_ad:
        TxMsg = "6015 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_73:
        TxMsg = "6019 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_75:
        TxMsg = "6021 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_76:
        TxMsg = "6032 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_77:
        TxMsg = "6033 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_IO_IN:
        TxMsg = "6037 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_81:
        TxMsg = "6039 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_82:
        TxMsg = "6040 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;

    case CMD_85:
        TxMsg = "6042 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_86:
        TxMsg = "6043 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_87:
        TxMsg = "6035 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_V_State:
        TxMsg = "6053 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_92:
        TxMsg = "6055 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_96:
        TxMsg = "6059 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_97:
        TxMsg = "6060 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_98:
        TxMsg = "6061 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_Close_Pumb:
        TxMsg = "6062 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_Curtain:
        TxMsg = "6063 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    case CMD_Action:
        TxMsg = "6064 "; TxMsg.append(msg);
        this->writeDatagram(TxMsg.toUtf8(),  sender_Record,  6000);
        break;
    default:

        break;
    }
}






