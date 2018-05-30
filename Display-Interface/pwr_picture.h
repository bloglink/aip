#ifndef PWR_PICTURE_H
#define PWR_PICTURE_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class pwr_picture;
}

class pwr_picture : public QWidget
{
    Q_OBJECT

public:
    explicit pwr_picture(QWidget *parent = 0);
    ~pwr_picture();

private:
    Ui::pwr_picture *ui;
};

#endif // PWR_PICTURE_H
