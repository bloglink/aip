#ifndef LINUX_COLOR_H
#define LINUX_COLOR_H

#include <QWidget>

class LINUX_COLOR : public QWidget
{
    Q_OBJECT
public:
    explicit LINUX_COLOR(QWidget *parent = 0);
signals:

public slots:
    QColor backColor(int color);
};

#endif // LINUX_COLOR_H
