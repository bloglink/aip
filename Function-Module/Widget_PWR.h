#ifndef WIDGET_PWR_H
#define WIDGET_PWR_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QDebug>
#include <QLabel>
#include <QDockWidget>
#include <QPushButton>
#include <QTableWidgetItem>

#include "linux_can.h"

namespace Ui {
class Widget_PWR;
}

class Widget_PWR : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_PWR(QWidget *parent = 0);
    ~Widget_PWR();

    int   PWM_Row_Count;
    QStringList PWR_Test_Data;
    QStringList PG_Test_Order;
    void Pub_Conf_Set_PWR(QString str, int flag);
    QStringList Copy_PWR_List;

    QStringList Pub_Set_PG(can_frame frame);
    QStringList PWR_Test_Param();

signals:
    void canSend(can_frame frame);

private slots:
    void on_pwrTab_cellClicked(int row,  int column);
    void on_pwrTab_cellChanged(int row,  int column);
    void on_PWR_Volt_up_editingFinished();
    void on_PWR_Volt_down_editingFinished();
    void on_pgTab_cellChanged(int row,  int column);


    void on_toolButton_4_clicked();
    void buttonJudge(int);
    void pg_buttonJudge(int);

private:
    Ui::Widget_PWR *ui;
    QList<QTableWidgetItem*>  PG_QTableWidgetItem;
    //    QList<QCheckBox *>        Box_PWR_Grade_List;
    can_frame frame;
    QWidget *Widget_Button;
    QList<QPushButton *>  Pwr_Button;
    QButtonGroup *Pwr_Button_Group;
    QButtonGroup *Pg_Button_Group;
    QLabel       *PWR_Labelhide; // 功率隐藏
    bool   EPAR_Init_Flag;
    QStringList pwrturn;
    QStringList Button_Text;

    int    PWR_Row;
    int    Old_Id;

    int     Pwr_Usart_Count;
    bool    Turn_Shan;
    QString PWM_Model;


    double pg_Cur, pg_floor, pg_up;
    double pg_freq_min, pg_freq_avr, pg_freq_max;
    double pg_duty_min, pg_duty_avr, pg_duty_max;
    double pg_speed;

    void  mousePressEvent(QMouseEvent * event);
    void  Pri_PWR_Init(QString);
    void  Pri_PWR_Data_Save();
    void  Pri_PWR_Test_Parameter(QString);
    void  Pri_PWR_Test_start(QString);
    void  Pri_PWR_power_chose(int);
    void  dd(int);
};

#endif // WIDGET_PWR_H
