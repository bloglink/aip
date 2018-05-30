/*******************************************************************************
 * Copyright (c) 2015,青岛艾普智能仪器有限公司
 * All rights reserved.
 *
 * version:     2.0
 * author:      SL
 * date:        2015.12.10
 * brief:       串口通信函数
*******************************************************************************/
#ifndef _SERIAL_H
#define _SERIAL_H

#include <pthread.h>
#include <QApplication>
#include "define.h"
#include "linux_can.h"

class CSerial: public QObject
{
    Q_OBJECT
private:
    pthread_t m_thread;

public:
    CSerial();
    virtual ~CSerial();
    int  m_fd;
    int  ReceiveThreadFunc_Flag;
    int  m_DatLen;
    char DatBuf[1500];

    int  m_ExitThreadFlag;
    int  OpenPort(char *PortNo, int baudrate, char databits, char stopbits, char parity);
    int  ClosePort();
    void WritePort(char* Buf, int len);
    bool ReceiveThreadFunc();

signals:
    void readyRead(QString);
    void BClicked(int);
};

#endif
