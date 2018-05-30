#ifndef WIDGET_DCW_H
#define WIDGET_DCW_H

#include <QWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QString>

#include "linux_can.h"
#include "define.h"

namespace Ui {
class Widget_DCW;
}

class Widget_DCW : public QWidget
{
    Q_OBJECT
public:
    explicit Widget_DCW(QWidget *parent = 0);
    ~Widget_DCW();

    QStringList Copy_DCW_List;
    QList<QLineEdit *> Line_DCW_List[1];
    can_frame frame;
    void Init_DCW();
    void Save_DCW_Data();
    void Slot_DCW_Setpoint(QString);

private:
    Ui::Widget_DCW *ui;

signals:
    void canSend(can_frame frame);

public slots:
};

#endif // WIDGET_DCW_H
