#include "linux_Thread.h"
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

static int hotplug_sock;
static int add1, add2;

MyThread::MyThread(QObject *parent) :
    QThread(parent)
{
    stop = false;
    hotplug_sock = init_hotplug_sock();

    fd=access( "/dev/sda1", 0 );
    if( fd==0 )
    {
        add1=1;
    }
    else
    {
        add1=0;
    }

    fd=access( "/dev/sdb1", 0 );
    if( fd==0 )
    {
        add2=1;
    }
    else
    {
        add2=0;
    }
}
int MyThread::init_hotplug_sock()
{
   const int buffersize = 1024;
   int ret;

   struct sockaddr_nl snl;
   bzero(&snl, sizeof(struct sockaddr_nl));
   snl.nl_family = AF_NETLINK;
   snl.nl_pid = getpid();
   snl.nl_groups = 1;

   int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
   if (s == -1)
   {
       perror("socket");
       return -1;
   }
   setsockopt(s, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));

   ret = bind(s, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));
   if (ret < 0)
   {
       perror("bind");
       close(s);
       return -1;
   }

   return s;
}
void MyThread::run()
{
    static char buf[UEVENT_BUFFER_SIZE * 2] = {0};
    char str[256];
    while(!stop)
    {
        recv(hotplug_sock, &buf, sizeof(buf), 0);
        if( strncmp(buf, "add", 3 )==0 )
        {
            QString path=("/dev/");
            QDir *dir=new QDir(path);
            QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(QDir::System ));
       //     QString sd_name;
            QStringList sd_list_name;
            sd_list_name.clear();

            qDebug()<<"fileInfo->count()"<<fileInfo->count();
            for(int i=0; i<fileInfo->count(); i++) {
                if(fileInfo->at(i).fileName().size() < 4)
                {
                    continue;
                }
                if(fileInfo->at(i).fileName().contains(QString(tr("sd"))))
                {
                    sd_list_name.append(fileInfo->at(i).fileName());
                }
            }
          if (sd_list_name.size() > 0) {
                sprintf(str, "mount -t vfat /dev/%s /mnt/usb1",QString(sd_list_name.at(sd_list_name.size()-1)).toUtf8().data());
                qDebug()<<"str---------------------------------->"<<str;
                system(str);
                add1 = 1;
                Singal_U_Test(1,"usb1");
            }
        /*   if (sd_list_name.size() > 0) {
                sprintf(str, "mount -t vfat /dev/%s /mnt/usb1",QString(sd_list_name.at(sd_list_name.size()-1)).toUtf8().data());
                qDebug()<<"str---------------------------------->"<<str;
                QByteArray m;
                QProcess pro;
                pro.execute(str);
                pro.waitForFinished();
                qDebug()<<"打印 QString";
                m = pro.readAllStandardOutput();

                QByteArray all = pro.readAll();
                 if(all.isEmpty())
                 {
                     all=pro.readAllStandardOutput();
                     if(all.isEmpty())
                     {
                         all=pro.readAllStandardError();
                     }
                 }
                qDebug()<<"  "<< QString(all);
                add1 = 1;
            //    Singal_U_Test(1,"usb1");
            }*/
       //     sd_name = fileInfo->at(i).fileName();


//            p = strstr( buf, "sdb/sd");
//            if((p!=NULL )&& (add1==0))
//            {
//                qDebug()<<"p>>>>>>>>>if"<<p;
//                sleep(1);
//                system( "umount  /mnt/usb1/" );
//                system( "mount -t vfat /dev/sda1 /mnt/usb1" );
//                qDebug()<<"mount -t vfat /dev/sda1 /mnt/usb1";
//                Singal_U_Test(1,"usb1");
//                add1 = 1;
//            }
//            else
//            {
//                p = strstr( buf, "sdb/sd");
//                if((p!=NULL)&& (add2==0))
//                {
//                    qDebug()<<"p>>>>>>>>>else"<<p;
//                    sleep(1);
//                    system( "umount  /mnt/usb2/" );
//                    system( "mount -t vfat /dev/sdb1 /mnt/usb2" );
//                    qDebug()<<"mount -t vfat /dev/sdb1 /mnt/usb2";
//                    Singal_U_Test(2,"usb2");
//                    add2 = 1;
//                }
//            }
        }
        if( strncmp(buf, "remove", 6 )==0 )
        {
            if (add1 == 1) {
                system( "umount-usb.sh 1" );
                Singal_U_Test(3,"u");
                add1 = 0;
            }
//            p = strstr( buf, "sdb/sd");
//            if(p==NULL)
//            {
//                qDebug()<<"p==NULL";
//            }
//            if( (p!=NULL )&& (add1==1) )
//            {
//                system( "umount-usb.sh 1" );
//                qDebug()<<"umount-usb.sh 1";
//                Singal_U_Test(3,"u");
//                add1 = 0;
//            }
//            else
//            {
//                p = strstr( buf, "sdb/sd");
//                if( (p!=NULL )&& (add2==1) )
//                {
//                    system( "umount-usb.sh 2" );
//                    qDebug()<<"umount-usb.sh 2";
//                    Singal_U_Test(4,"u");
//                    add2 = 0;
//                }
//            }
        }
    }
}
