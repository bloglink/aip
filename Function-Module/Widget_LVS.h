#ifndef WIDGET_LVS_H
#define WIDGET_LVS_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QDebug>
#include <QTableWidgetItem>

#include "linux_can.h"

namespace Ui {
class Widget_LVS;
}

class Widget_LVS : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_LVS(QWidget *parent = 0);
    ~Widget_LVS();
    QStringList Copy_LVS_List;
    void Pub_Conf_Set_LVS(QString, int);
    QStringList LVS_Test_Param();

private slots:
    void on_lvsTab_cellChanged(int row,  int column);

signals:
    void canSend(can_frame frame);

private:
    Ui::Widget_LVS *ui;
    QStringList lvsturn;
    can_frame frame;

    bool  LVS_Init_Flag;
    void  Pri_LVS_Init();
    void  Pri_LVS_Data_Save();
    void  Pri_LVS_Test_Parameter(QString);
    void  Pub_Conf_power_chose(int);
};

#endif // WIDGET_LVS_H
