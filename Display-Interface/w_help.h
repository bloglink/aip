#ifndef W_HELP_H
#define W_HELP_H

#include <QWidget>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QTextCodec>

namespace Ui {
class w_Help;
}

class w_Help : public QWidget
{
    Q_OBJECT
public:
    explicit w_Help(QWidget *parent = 0);
    ~w_Help();
    int cWin;
    void updateText(int win, int index);
private:
    Ui::w_Help *ui;
    QStringList name;
    int Picture_count;

signals:
    void sendData(int win);

private slots:
    void on_Key_A_clicked();
    void on_pushButton_clicked();
    void on_pushButton_down_clicked();
    void on_pushButton_up_clicked();
};

#endif // W_HELP_H
