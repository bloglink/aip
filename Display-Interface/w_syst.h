#ifndef W_SYST_H
#define W_SYST_H

#include <QTimer>
#include <QDialog>
#include <QDockWidget>
#include <QGridLayout>
#include <QTextCodec>
#include <QSettings>
#include <QCheckBox>
#include <QDir>
#include <QTime>
#include <QButtonGroup>
#include <QSpinBox>
#include <QString>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

#include <netdb.h>

#include "define.h"
#include "linux_can.h"
#include "linux_rtc.h"
#include "pwm_api.h"


namespace Ui {
class w_Syst;
}

class w_Syst : public QDialog
{
    Q_OBJECT
public:
    explicit w_Syst(QWidget *parent = 0);
    ~w_Syst();
private:
    Ui::w_Syst *ui;
    QList<QDoubleSpinBox *> dateList;
    QList<QLabel*>    PushHist_lable;
    QList<QLabel*>    SQL_lable;
    QWidget           *net_Widget;
    QWidget           *SQL_Widget;
    QCheckBox         *net_sql_enable;
    QCheckBox         *net_sql_NG;
    QCheckBox         *stagnate;
    QButtonGroup      *Syst_Group;
    QList<QLineEdit*> line_Text;

    QWidget           *IO_Widget;
    QList<QLabel*>    IO_Show_Lable;
    QList<QLineEdit*> IO_Line_Text;

    struct {
        int User;
        int Beep_OK;
        int Beep_NG;
        int Start_Model;
        bool SaveNG;
        QString SQL_enable;
        QStringList Sum;
    }System;
    QString System_power;
    int  power_para;
    int L_count,R_count;

    void update_system();
    void check_finsh(QString);
    void gettime();
    QString GetGateWay();
    QString GetIP();

public slots:
    void slot_from_main(QString, int);

signals:
    void signal_to_main(QStringList, int, int);

private slots:
    void ButtonJudge(int);
    void on_Timer_clicked();
    void on_Key_H_clicked();
    void on_Beep_Button_clicked();
    void on_Led_Button_clicked();
    void on_Button_Check_clicked();
    void on_Button_Check_2_clicked();
    void on_Button_Check_3_clicked();
    void net_Widget_set_net();
    void net_Widget_autoquit();
    void IO_Widget_autoquit();
    void SQL_Widget_autoquit();
    void net_Widget_set_SQL();
    void Left_Widget_autoquit();
    void Right_Widget_autoquit();

    void on_lineEdit_editingFinished();
    void on_scan_start_editingFinished();
    void on_scan_stop_editingFinished();
    void on_Button_SQL_clicked();
    void on_Button_IO_clicked();

    void IO_RFID_Read();
    void IO_RFID_Write();
    void on_Button_IO_2_clicked();
};

#endif // W_SYST_H
