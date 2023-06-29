#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "sidebar.h"
#include "board.h"
#include "msg.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QTimer>
#include <QTime>
#include <QLabel>

 enum role{server,client};
 enum end_status{unknown,win_by_moving,lose_by_moving,win_by_not_moving,lose_by_not_moving,win_by_time_out,lose_by_time_out,win_by_admit,lose_by_admit};

class GameManager : public QObject
{
    Q_OBJECT
public:
    explicit GameManager(QAction* start,QAction* defeat,QLabel*l1,QLabel* l2,Board* b,SideBar* s,QObject *parent = nullptr);
int record_board[121];
    QTcpServer *listenSocket_s = nullptr;
    QTcpSocket *readWriteSocket = nullptr;
    QLabel* _label;

signals:
    void receive_move_first();
    void receive_move_second();
    void receive_wait();
    void receive_stop();


public slots:
    void acceptConnection_s();
    void recvMessage();
    void handle_receive();
    void send(const QByteArray&);

    void update_status();
    void move_first();
    void move_second();
    void do_wait();
    void do_stop();
    void set_want_to_play(bool me, bool want);
    void start_game();
    void step_handler(int,int);
    void time_out_handler(bool);
    void game_over(end_status);
    void defeat_handler();


    QString setRole(role me, QString host = "");



private:
    QString initServer();
    void connectHost();
    void* _mainwindow;
    Board* _board;
    SideBar* _sidebar;
    QAction *_start;
    QAction *_defeat;

    QLabel* _label_right;

    QByteArray info_got;
    QTimer *mytimer;
    Msg msg_got;
    QString host_address;
    role myrole;
    int my_id = 0;
    int get_count = 0;
    int round_count = 0;
    int lattice_back_up[121] = {};

    bool something_wrong = false;


    bool connected = false;
    bool I_want_to_play = false;
    bool you_want_to_play = false;
    bool game_on_going = false;

    void a_step();
    void normal_time_out();
    void fatal_time_out();
    void normal_step(int,int);
    void win_step(int,int);
    void lose_step(int,int);


    int move_enough_checker(); // 1 win 0 normal -1 lose




signals:

};

#endif // GAMEMANAGER_H
