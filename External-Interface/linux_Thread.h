#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
#include <QDir>
#include <QProcess>

#define UEVENT_BUFFER_SIZE 2048

class MyThread : public QThread
{
    Q_OBJECT
private:
    char* p;
    static  int init_hotplug_sock();
public:
    int	fd;

    bool stop ;
    explicit MyThread(QObject *parent = 0);
    void run();
signals:
    void Singal_U_Test(int,QString);
public slots:

};
#endif // MYTHREAD_H
