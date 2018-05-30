#ifndef WIDGET_BLOCK_H
#define WIDGET_BLOCK_H

#include <QWidget>
#include <QComboBox>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QDebug>
#include <QTableWidgetItem>

#include "linux_can.h"
#include "wt_wave.h"

namespace Ui {
class Widget_BLOCK;
}

class Widget_BLOCK : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_BLOCK(QWidget *parent = 0);
    ~Widget_BLOCK();
    QStringList Copy_BLOCK_List;
    void Pub_Conf_Set_BLOCK(QString, int, int);
    QStringList BLOCK_Test_Param();

private slots:
    void on_dzTab_cellChanged(int row,  int column);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

    void on_toolButton_4_clicked();

signals:
    void canSend(can_frame frame);

private:
    Ui::Widget_BLOCK *ui;
    wt_Wave    *wave[1];  // 波形 1 显示区
    QList<QTableWidgetItem*>  DZ_QTableWidgetItem;
    can_frame frame;

    int  Collect_Highest_Point;
    bool Update_Motor_Flag;
    int  Narrow_Proportion;
    int  Collect_Cycle;
    int  pushButton_Count;
    int  wave_show_count;
    int  power_chose;
    QStringList Collect_pwr_data;  QStringList Collect_pwr_data_copy;
    QStringList Collect_current_data;


    bool  EPAR_Init_Flag;
    void  Pri_BLOCK_Init();
    void  Pri_BLOCK_Data_Save();
    void  Pri_BLOCK_Display();
    void  Pub_Conf_power_chose(int);
};

#endif // WIDGET_BLOCK_H
