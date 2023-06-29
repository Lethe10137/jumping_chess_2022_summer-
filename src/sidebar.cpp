#include "sidebar.h"
#include "qpainter.h"


SideBar::SideBar(QWidget *parent)
    : QWidget{parent}
{
//    your_id = 1; // 1 means up, 2 means down; 0 means unknown
//    current_id = 2;
//    timer = 7;
//    round_count = 5;
//    up_time_out =2 ;
//    down_time_out = 1;

}


void SideBar::paintEvent(QPaintEvent *event){
       if(!paint_or_not)return;
    int h = height();
    int w = width();
    int height_per_line = h/11;

    QPainter p;
    p.begin(this);

    p.setPen(QPen(Qt::black,5));

    QFont font = this->font();
    font.setPointSize(height_per_line * 0.6);
    p.setFont(font);

    QRect rect;
    QString to_display;

    rect = QRect(0,0.5 * height_per_line,w,height_per_line);
    to_display = "Red";
    if(your_id == 1)to_display += "(you)";
    if(current_id==1) p.setPen(QPen(QColor("#FFD700"),5));
    p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,to_display);
    p.setPen(QPen(Qt::black,5));

    rect = QRect(0,1.5 * height_per_line,w,height_per_line);
    switch (up_time_out){
        case 1:
            p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,"One Timeout");
            break;
        case 2:
            p.setPen(QPen(Qt::red,5));
            p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,"Two Timeouts");
            p.setPen(QPen(Qt::black,5));
            break;
        case 3:
            p.setPen(QPen(Qt::darkRed,5));
            p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,"Three Timeouts");
            p.setPen(QPen(Qt::black,5));
            break;
    }
//    qDebug() << current_id << your_id << timer;
    if(current_id == 1 && timer>= 0){
        if(up_time_out)rect = QRect(0,2.5 * height_per_line,w,height_per_line);
        else rect = QRect(0,1.5 * height_per_line,w,height_per_line);
        to_display = QString::number(timer)+"s";
        if(timer > 0 && timer <= 3)p.setPen(QPen(Qt::red,5));
        if(timer == 0)p.setPen(QPen(Qt::darkRed,5));
        p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,to_display);
        p.setPen(QPen(Qt::black,5));
    }

    if(round_count > 0){
        rect = QRect(0,5 * height_per_line,w,height_per_line);
        to_display = "Round" + QString::number(round_count);
        p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,to_display);
    }


    rect = QRect(0,7.5 * height_per_line,w,height_per_line);
    to_display = "Green";
    if(your_id == 2)to_display += "(you)";
    if(current_id==2) p.setPen(QPen(QColor("#FFD700"),5));
    p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,to_display);
    p.setPen(QPen(Qt::black,5));

    rect = QRect(0,8.5 * height_per_line,w,height_per_line);
    switch (down_time_out){
        case 1:
            p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,"One Timeout");
            break;
        case 2:
            p.setPen(QPen(Qt::red,5));
            p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,"Two Timeouts");
            p.setPen(QPen(Qt::black,5));
            break;
        case 3:
            p.setPen(QPen(Qt::darkRed,5));
            p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,"Three Timeouts");
            p.setPen(QPen(Qt::black,5));
            break;
    }

    if(current_id == 2 && timer>= 0){
        if(down_time_out)rect = QRect(0,9.5 * height_per_line,w,height_per_line);
        else rect = QRect(0,8.5 * height_per_line,w,height_per_line);
        to_display = QString::number(timer)+"s";
        if(timer > 0 && timer <= 3)p.setPen(QPen(Qt::red,5));
        if(timer == 0)p.setPen(QPen(Qt::darkRed,5));
        p.drawText(rect,Qt::AlignLeft | Qt::TextWordWrap,to_display);
        p.setPen(QPen(Qt::black,5));
    }


}
