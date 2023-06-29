#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include "msg.h"

class SideBar : public QWidget
{
    Q_OBJECT
public:
    explicit SideBar(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
    bool paint_or_not = false;

signals:
//void report_after_my_action_on_certain_round(int);
void time_out(bool fatal);

public slots:
void change_id(int id = -1){
    if(id == -1)current_id = 3 - current_id;
    else current_id = id;
    timer = 20;
}

void collect_info(Msg &info){
    info.first_player_time_out = up_time_out;
    info.last_player_time_out = down_time_out;
     qDebug() <<"timeouts when clooecting"<<up_time_out<<down_time_out;
}

void set_timmer(int n){
    timer = n;
}

void timer_tick(){
    int k = timer;
    if(timer > 0)timer-=1;


    if(k == 1 && (your_id == current_id)){
        if(your_id == 1)up_time_out+=1;
        if(your_id == 2)down_time_out+=1;
        qDebug() <<"timeouts in slide"<<up_time_out<<down_time_out;
        emit time_out((your_id == 1 && up_time_out == 3) ||(your_id == 2 && down_time_out == 3));

    }
    update();
}

void set_round_count(int n){
    round_count = n;
//    repaint();

}

void set_your_id(int n){
    your_id = n;
}


void round_count_tick(){
    round_count++;
}

void set_time_out(int u, int d){
    up_time_out = u;
    down_time_out = d;
}




private:
    int your_id = 0; // 1 means up, 2 means down; 0 means unknown
    int current_id = -1;
    int timer = -1;
    int round_count = 0;
    int up_time_out =0 ;
    int down_time_out = 0;

public:
    void reset(){
        your_id = 0; // 1 means up, 2 means down; 0 means unknown
        current_id = -1;
        timer = -1;
        round_count = 0;
        up_time_out =0 ;
        down_time_out = 0;
    }

};

#endif // SIDEBAR_H
