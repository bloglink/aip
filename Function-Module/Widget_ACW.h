#ifndef WIDGET_ACW_H
#define WIDGET_ACW_H

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QDebug>
#include <QString>

#include "define.h"
#include "linux_can.h"


namespace Ui {
class Widget_ACW;
}

class Widget_ACW : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_ACW(QWidget *parent = 0);
    ~Widget_ACW();

    int ACW_Test_Total; //-相间耐压测试总数
    QStringList ACWgroup_num; //-相间测试的数组
    QStringList Copy_ACW_List; //-耐压存储数据

    int  ACW_Compensate(double);
    bool ACW_Back_Key_E();
    bool ACW_If_Compensate(QString);
    void Pub_Conf_Set_ACW(QString str, int value);
    QStringList ACW_Test_Param();
    QStringList ACW_Test_Result(QStringList);
    void ACW_NetData(QStringList, QString);

private:
    Ui::Widget_ACW *ui;

    bool ACW_Between_IR; //-普通交耐和相间交耐的区别
    bool Init_Flag; // -IR 初始化进行中
    bool ACW_Compensate_Flag; //-交耐补偿标志位
    double ACW_Compensate_Result; //-交耐补偿数据
    QLabel *ACW_Labelhide; //-交耐隐藏标签
    can_frame frame;
    QButtonGroup *ACW_btnGroup;
    QList<QCheckBox *> Box_All_List;
    QList<QCheckBox *> Box_ACW_list;
    QList<QTableWidgetItem*>  ACW_T1_List; //-相间高端
    QList<QTableWidgetItem*>  ACW_T2_List; //-相间低端
    QList<QTableWidgetItem*>  ACW_voltage; //-相间电压
    QList<QTableWidgetItem*>  ACW_I_Max; //-相间电流上限
    QList<QTableWidgetItem*>  ACW_I_Min; //-相间电流下限
    QList<QTableWidgetItem*>  ACW_ARC; //-相间电弧
    QList<QTableWidgetItem*>  ACW_time; //-相间时间
    QList<QTableWidgetItem*>  ACW_compensate; //-相间补偿
    QStringList freq;

    bool  Pri_ACW_T1T2_Judge(QString, QString);
    void  mousePressEvent(QMouseEvent * event);
    void  Pri_ACW_Init(QString);
    void  Pri_ACW_Save_Data();
    void  Pri_ACW_Default_value();
    void  Pri_ACW_Test();
    void  Pri_ACW_WMessage(QString Waring_Text);

signals:
    void canSend(can_frame frame);

private slots:
    void join_buttonJudge(int);
    void on_acwTab_cellChanged(int row,  int column);
    void on_acwTab_cellClicked(int row,  int column);
    void box_statechange();
    void on_double_I_Down_editingFinished();
    void on_double_I_Up_editingFinished();
};

#endif // WIDGET_ACW_H
