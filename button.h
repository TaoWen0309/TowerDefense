#ifndef BUTTON_H
#define BUTTON_H

 
#include <QWidget>
#include <QPushButton>      //要创建按钮控件，必须加入此头文件

class button : public QWidget
{
    Q_OBJECT

public:
    button(QWidget *parent = 0);
    ~button();
    void custom1();
    void custom2();
    void custom3();
    int custom;
    QPushButton b1;
    QPushButton b2;
    QPushButton b3;
};

#endif // BUTTON_H
