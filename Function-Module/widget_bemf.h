#ifndef WIDGET_BEMF_H
#define WIDGET_BEMF_H

#include <QWidget>
#include "linux_can.h"
#include <QTableWidgetItem>

namespace Ui {
class Widget_BEMF;
}

class Widget_BEMF : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_BEMF(QWidget *parent = 0);
    ~Widget_BEMF();
    QStringList Copy_BEMF_List;
    void Pub_Conf_Set_BEMF(QString, int);
    QStringList BEMF_Test_Param();
    void BEMF_NetData(QStringList, QString);

private:
    Ui::Widget_BEMF *ui;
    QList<QTableWidgetItem*>  BEMF_QTableWidgetItem;

    void Pri_BEMF_Init();
    void Pri_BEMF_Data_Save();
signals:
    void canSend(can_frame frame);
};

#endif // WIDGET_BEMF_H
