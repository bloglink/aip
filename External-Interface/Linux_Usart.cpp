/*******************************************************************************
 * Copyright (c) 2015,青岛艾普智能仪器有限公司
 * All rights reserved.
 *
 * version:     2.0
 * author:      SL
 * date:        2015.12.10
 * brief:       串口通信函数
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <termios.h>
#include <QDebug>
#include <pwm_api.h>
#include <QIODevice>

#include "Linux_Usart.h"

typedef struct port_info {
    int baud_rate;
    int port_fd;
    char parity;
    char stop_bit;
    char flow_ctrl;
    char data_bits;
}*pport_info;
// get the baud_rate defination according to the baud rate

int get_baud_rate(int64_t baud_rate) {
    switch (baud_rate) {
    case 0:
        return B0;
    case 50:
        return B50;
    case 75:
        return B75;
    case 110:
        return B110;
    case 134:
        return B134;
    case 150:
        return B150;
    case 200:
        return B200;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;

    default:
        return -1;
    }
}

int set_port(pport_info p_info) {
    struct termios new_opt;
    int baud_rate;
    int status;

    tcgetattr(p_info->port_fd, &new_opt);
    bzero( &new_opt, sizeof(new_opt));
    baud_rate = get_baud_rate(p_info->baud_rate);
    tcflush(p_info->port_fd, TCIOFLUSH);
    cfsetispeed(&new_opt, baud_rate);
    cfsetospeed(&new_opt, baud_rate);
    status = tcsetattr(p_info->port_fd, TCSANOW, &new_opt);
    if (status != 0) {
        perror("tcsetattr::set baud rate failed\n");
        return -1;
    } else {
        // -
    }

    new_opt.c_cflag |= CLOCAL;
    new_opt.c_cflag |= CREAD;
    new_opt.c_cflag |= HUPCL;
    switch (p_info->flow_ctrl) {
    case '0':
        new_opt.c_cflag &=~CRTSCTS;
        break;
    case '1':
        new_opt.c_cflag |=CRTSCTS;
        break;
    case '2':
        new_opt.c_iflag |= IXON | IXOFF | IXANY;
        break;
    }
    new_opt.c_cflag &=~CSIZE;
    switch (p_info->data_bits) {
    case '5':
        new_opt.c_cflag |=CS5;
        break;
    case '6':
        new_opt.c_cflag |=CS6;
        break;
    case '7':
        new_opt.c_cflag |=CS7;
        break;
    case '8':
        new_opt.c_cflag |=CS8;
        break;
    default:
        new_opt.c_cflag |=CS8;
    }
    switch (p_info->parity) {
    case 'n':
    case 'N':
        new_opt.c_cflag &= ~PARENB;    /* Clear parity enable */
        new_opt.c_iflag &= ~INPCK;     /* Enable parity checking */
        break;
    case 'o':
    case 'O':
        new_opt.c_cflag |= (PARODD | PARENB);
        new_opt.c_iflag |= INPCK;      /* Disable parity checking */
        break;
    case 'e':
    case 'E':
        new_opt.c_cflag |= PARENB;      /* Enable parity */
        new_opt.c_cflag &= ~PARODD;
        new_opt.c_iflag |= INPCK;       /* Disable parity checking */
        break;
    case 'S':
    case 's':  /*as no parity*/
        new_opt.c_cflag &= ~PARENB;
        new_opt.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported parity\n");
        return -1;
    }

    if (p_info->stop_bit == '2') {
        new_opt.c_cflag |=CSTOPB;
    } else {
        new_opt.c_cflag &=~CSTOPB;
    }

    if ((p_info->parity != 'n') || (p_info->parity != 'N')) {
        new_opt.c_iflag |= INPCK;
    }

    new_opt.c_lflag &= ~(ICANON | ECHO | ISIG); /*Input*/
    new_opt.c_oflag &= ~OPOST;                  /*Output*/
    new_opt.c_cc[VMIN] = 1;
    new_opt.c_cc[VTIME] = 1;

    tcflush(p_info->port_fd, TCIFLUSH);

    status = tcsetattr(p_info->port_fd, TCSANOW, &new_opt);
    if (status != 0) {
        perror("Cannot set the serial port parameters");
        return -1;
    } else {
        // -
    }
    return status;
}

CSerial::CSerial() {
    m_DatLen = 0;
    m_ExitThreadFlag = 0;
    ReceiveThreadFunc_Flag = 0;
}

CSerial::~CSerial() {
    // -
}
bool CSerial::ReceiveThreadFunc() {
    fd_set fdRead;
    int ret;
    struct timeval aTime;

    if (m_ExitThreadFlag) {
        return false;
    }

    FD_ZERO(&fdRead);
    FD_SET(m_fd, &fdRead);

    aTime.tv_sec = 0;
    aTime.tv_usec = 0; // 300000
    ret = select(m_fd+1, &fdRead, NULL, NULL, &aTime);
    if (ret < 0) {
        ClosePort();
        return false;
    }
    if (ret == 0) {
        return false;
    } else {
        if (FD_ISSET(m_fd, &fdRead)) {
            m_DatLen = read(m_fd, DatBuf, 100);
            if ( m_DatLen > 0 ) {
                DatBuf[m_DatLen] = 0;
                return true;
            } else {
                // -
            }
        } else {
            // -
        }
    }
    return true;
}
int CSerial::OpenPort(char *PortNo, int baudrate, char databits, char stopbits, char parity) {
    char portname[20];
    struct port_info info;
    sprintf(portname, "/dev/%s", PortNo);

    if ((m_fd = open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK)) == -1) {
        perror("Cannot open the desired port");
        return -1;
    }

    info.baud_rate = baudrate;
    info.parity = parity;
    info.data_bits = databits;
    info.flow_ctrl = '0';
    info.port_fd = m_fd;
    info.stop_bit = stopbits;

    if (set_port(&info)== -1) {
        return -1;
    }
    return 0;
}

void CSerial::WritePort(char *Buf, int len) {
    write(m_fd, Buf, len);
}
int CSerial::ClosePort() {
    m_ExitThreadFlag = 1;
    if (m_fd > 0) {
        close(m_fd);
    }
    return 0;
}


