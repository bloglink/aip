#ifndef IPMETHOD_H
#define IPMETHOD_H

#include <QWSInputMethod>
#include <w_keyb.h>

class IPMethod : public QWSInputMethod
{
    Q_OBJECT

public:
    IPMethod();
    ~IPMethod();

public:
    void updateHandler(int);

private:
    w_Keyb* wKeyb;

    QString content;
    bool isCapsLock;

signals:
    void SendCaps(bool);

private slots:
    void SendString(QString);
    void Preedit(QString);
    void Confirm();
    void Backspace();
    void Escape();
    void CapsLock();

    void Upspace();
    void Downspace();
    void Tabspace();
    void BackTabspace();
};

#endif // IPMETHOD_H
