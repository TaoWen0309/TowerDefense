#include "button.h"
#include <QPushButton>

button::button(QWidget *parent)
    : QWidget(parent)
{

       b1.setParent(this);//把b1按钮的父对象设置成窗口(qt_class就是表示窗口)，这样的作用是，按钮不会跟窗口分离，只会在窗口里面
       b1.setText("第一关");//b1按钮上写着关卡数
       b1.setGeometry(200,100,100,70);
       connect(&b1,&QPushButton::pressed,this,&button::custom1);

       b2.setParent(this);
       b2.setText("第二关");
       b2.setGeometry(200,200,100,70);
       connect(&b2,&QPushButton::pressed,this,&button::custom2);

       b3.setParent(this);
       b3.setText("第三关");
       b3.setGeometry(200,300,100,70);
       connect(&b3,&QPushButton::pressed,this,&button::custom3);
}

button::~button()
{

}

void button::custom1(){
    custom = 0;
    this->close();
}

void button::custom2(){
    custom = 1;
    this->close();
}
void button::custom3(){
    custom = 2;
    this->close();
}
