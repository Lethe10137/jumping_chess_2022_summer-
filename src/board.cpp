#include "board.h"
#include <qpainter.h>
#include <QDebug>
#include <QMouseEvent>

Board::Board(QWidget *parent):QWidget(parent)
{
    QVector<int> test_red = {0,1,2,3,4,5,6,7,8,9};
    QVector<int> test_green = {111,112,113,114,115,116,117,118,119,120};
    set_lattice_status(test_red,test_green);
//    current_selected = -1;
    connect(this,&Board::click_chess,this,&Board::handle_click);
//    current_status =hasnt_decided_yet;
//    last_from = 60;
//    last_to = 61;
}

Board::~Board()
{

}

void Board::paintEvent(QPaintEvent *event){

//    qDebug() <<"lattice_status[0]:"<< lattice_status[0];

    if(!paint_or_not)return;
    int h = height();
    int w = width();
    int total_size = h > w ? w : h;
    total_size *= 9; total_size /= 10;
    gridH = total_size / 16.5;
    gridW = gridH / 1.732050807;

    QPainter p(this);


    p.setPen(Qt::white);
    p.setBrush(Qt::black);

    for(int i =0; i < 121; i++){
        int x = Board::lattice[i][1] - 48;
        int y = Board::lattice[i][2] - 49;
        p.save();
        p.translate(gridW * x , gridH * y );
        p.translate(- gridH/4,-gridH/4);
        p.drawEllipse(0,0,gridH/2,gridH/2);
        p.restore();

    }


    for (int i = 0; i < 121; i++){
        if (this->lattice_status[i] == 0)continue;
        int n = i;
        p.save();
        p.translate((Board::lattice[n][1]-48 )* gridW, (Board::lattice[n][2]-49) * gridH);

        if(this->lattice_status[i] == 1){
            p.setBrush(Qt::red);}
        else{
            p.setBrush(Qt::green);}
        p.translate(- gridH/2,-gridH/2);
        p.drawEllipse(0,0,gridH,gridH);
        p.restore();

    }

      QVector<int> le;
      if(current_selected >= 0) {
          le = get_leagal_jumps(current_selected ,current_id );
          p.save();
          p.translate((Board::lattice[current_selected][1]-48 )* gridW, (Board::lattice[current_selected][2]-49) * gridH);
          p.setPen(QColor("#FFD700"));
          p.setBrush(QColor("#FFD700"));
          p.drawRect(-gridH/16,-gridH/3,gridH/8,2*gridH/3);
          p.drawRect(-gridH/3,-gridH/16,2*gridH/3,gridH/8);
          p.restore();
      }
      golden_points = le;


    for(int i = 0; i < le.count(); i++){
        int n = le[i];
        p.save();
        p.translate((Board::lattice[n][1]-48 )* gridW, (Board::lattice[n][2]-49) * gridH);
        p.setPen(QColor("#FFD700"));
        p.setBrush(QColor("#FFD700"));
        p.translate(- gridH/4,-gridH/4);
        p.drawEllipse(0,0,gridH/2,gridH/2);
        p.restore();
    }

    if(last_from >= 0 && last_from <= 120){
        p.save();
        p.translate((Board::lattice[last_from][1]-48 )* gridW, (Board::lattice[last_from][2]-49) * gridH);
        p.setPen(QColor("#FFD700"));
        p.setBrush(QColor("#FFD700"));
        p.drawRect(-gridH/16,-gridH/3,gridH/8,2*gridH/3);
        p.drawRect(-gridH/3,-gridH/16,2*gridH/3,gridH/8);
        p.restore();
    }
    if(last_to >= 0 && last_to <= 120){
        p.save();
        p.translate((Board::lattice[last_to][1]-48 )* gridW, (Board::lattice[last_to][2]-49) * gridH);
        p.setBrush(QColor("#FFD700"));
        p.translate(- gridH/4,-gridH/4);
        p.drawEllipse(0,0,gridH/2,gridH/2);
        p.restore();
    }




}


void Board::set_lattice_status(QVector<int>& red,QVector<int>& green ){ // 0 ~ 120
    if(red.count() != 10 || green.count() != 10)return;
    for(int i = 0; i < 121; i++){
        this->lattice_status[i] = 0;
    }
    for(int i = 0; i < 10; i++){
        if(red[i] < 0 && red[i] > 120)continue;
        this->lattice_status[red[i]] = 1;
    }
    for(int i = 0; i < 10; i++){
        if(green[i] < 0 && green[i] > 120)continue;
        this->lattice_status[green[i]] = 2;
    }
    return;
}

QVector<int> Board::get_leagal_jumps(int chess, int id){

    QVector<int> ans;
    if(chess < 0 || chess > 120) return ans;
    if(lattice_status[chess] != id)return ans;
    int visited[121] = {};
    QVector<int> stack;
    stack << chess; visited[chess] = 1;
    while(stack.count() > 0){
        int cur = stack[stack.count() -1]; // 0 ~ 120
        stack.pop_back();
        if(lattice_status[cur]==0 &&visited[cur]==0)  ans << cur;
        visited[cur] = 1;
        for(int i = 3; i < 9; i++){
            int next = this->lattice[cur][i];
            if(next < 0 || next > 120)continue;
            if(lattice_status[next]<=0)continue;
            int next2 = this->lattice[next][i];
            if(next2 < 0 || next2 > 120)continue;
            if(lattice_status[next2]!=0)continue;
            if(visited[next2]==0){stack<<next2;}
        }
    }

    for(int i = 3; i < 9; i++){
        if(lattice_status[lattice[chess][i]] == 0 && visited[lattice[chess][i]] == 0){
            ans << lattice[chess][i];
        }
    }
    return ans;
}


void Board::mousePressEvent(QMouseEvent *event){
     int x = event->pos().x();
     int y = event->pos().y();

     if(event->button()!=Qt::LeftButton)
        return QWidget::mousePressEvent(event);
     int chess_pushed = get_clicked_at(x,y);

     if(chess_pushed >= 0){
         emit click_chess(chess_pushed);
     }
     return QWidget::mousePressEvent(event);
}

int Board::get_clicked_at(int x, int y){
    int ans = -1;
    double distance_sq = (gridH/1.9) * (gridH/1.9);
    for(int i =0; i < 121;i++){
        double dx = x - gridW * (this->lattice[i][1] - 48);
        double dy = y - gridH * (this->lattice[i][2] - 49);
//        if(i==0)qDebug()<<"("<<gridH * this->lattice[i][1]<<", "<< gridW * this->lattice[i][2]<<")"<<"("<<x<<", "<<y<<")";
        double d = dx * dx + dy * dy;
        if(d < distance_sq){
            distance_sq = d;
            ans = i;
        }
    }
    return ans;
}

void Board::handle_click(int a){
    qDebug()<<a;
    if(current_status == no_right_to_move || a < 0 || a > 120){
        emit illegal_click();
        return;
    }
    if(current_status == has_decided){
        if(lattice_status[a] == current_id){
            current_selected = a;
            repaint();
            return;
        }
        if(lattice_status[a] == 0){
            bool is_golden = false;
            for(auto golden : golden_points){
                if(a == golden){
                    is_golden = true;
                    break;

                }
                qDebug()<<golden;

            }
            if(is_golden){
                current_status = no_right_to_move;
                lattice_status[a] = current_id;
                lattice_status[current_selected] = 0;


//                int in_red = 0;
//                int in_green = 0;
//                for(int i = 0; i < 10; i++){
//                    if(lattice_status[a] == current_id)in_red++;
//                    if(lattice_status[120-a] == current_id)in_green++;
//                }



                    emit successfully_moved(current_selected,a);

                    qDebug() << "successfully_moved"<<this << current_selected << a;
\


                current_selected = -1;
                  repaint();
                return;
            }else{
                emit illegal_click();
                return;
            }
        }
        emit illegal_click();
        return;
    }
    if(current_status == hasnt_decided_yet){
        if(lattice_status[a] == current_id){
            current_selected = a;
            current_status = has_decided;
            last_from = -1;
            last_to = -1;
            repaint();
            return;
        }else{
            emit illegal_click();
            return;
        }
    }

}

void Board::collect_info(Msg& info){
    for(int i = 0;i < 121; i++){
        info.lattice_status[i] = lattice_status[i];
    }
}
