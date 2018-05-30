#ifndef WIDGET_MAG_H
#define WIDGET_MAG_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QTableWidgetItem>
#include <QString>
#include <QComboBox>
#include <QMessageBox>

#include "define.h"
#include "wt_wave.h"
#include "linux_can.h"

namespace Ui {
class Widget_MAG;
}

class Widget_MAG : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_MAG(QWidget *parent = 0);
    ~Widget_MAG();

    QList<QCheckBox*>  Box_MAG_List;
    QList<QComboBox*>  Comb_MAGConf_List;
    QList<QLineEdit*>  Line_MAG_List[16];
    QList<QLineEdit*>  Line_RMAG_List[8];
    QList<QTableWidgetItem *> MAG_WidgetList[3];

    int  MAG_TestItems;
    int  MAG_TestNumber;
    QStringList Copy_MAG_List;

    void MagDir_Conf(int, int, int *waveItt);
    int  Test_MagData(int, int, can_frame);
    void Pub_Conf_Set_MAG(QString str, int flag);
    QStringList MAG_Test_Param(int Mag_shapehide);
    void MAG_NetData(QStringList, QString);
private:
    Ui::Widget_MAG *ui;
    QButtonGroup *MAG_Group;
    QLabel *MAG_Labelhide;
    QList<wt_Wave *> MAG_WaveList;
    can_frame frame;
    QLabel    *label_Waring;

    void mousePressEvent(QMouseEvent * event);
    void Pri_MagDir_Conf(int); // Conf测试磁旋
    void Pri_MAG_Init(QString);
    void Pri_MAG_Save_Data();
    void Pri_MAG_SetTaskDir(QString);

    void Pri_MAG_Default_value(QString);
    void Pri_MAG_TestSample(); // TestSampleOPP
    void Pri_MAG_Test();       // TestOPP

signals:
    void canSend(can_frame frame);

private slots:
    void initSpinBox(QDoubleSpinBox *spin,  int dec,  int min,  int max);
    void buttonJudge(int);
    void Box_0_clicked(int);
    void Box_1_clicked(int);
    void on_oppTab_cellChanged(int row,  int column);
    void on_oppTab_cellClicked(int row,  int column);
};

#endif // WIDGET_MAG_H
