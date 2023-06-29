#ifndef MSG_H
#define MSG_H

#include <QByteArray>

enum chess_playing_msg{
    blank,timeout,fatal_timeout,admit_defeat,fatal_not_moving_enough,winning,
    sending_invitation,accepting_invitation,refusing_invitation,game_start
};

enum request{
    you_move_first, you_move_second, please_wait, please_stop
};

class Msg{
public:
    chess_playing_msg my_msg = blank;
    request my_request = please_wait;
    int round_count = 0;
    int last_from = 127;
    int last_to = 127;
    int first_player_time_out = 0;
    int last_player_time_out = 0;

    int lattice_status[121] = {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2};

    Msg(){
//        for(int i = 0; i < 10 ; i++){lattice_status[i] = 1;lattice_status[120-i] = 2;}
    }

    Msg(const QByteArray &content);
    void set_binary(const QByteArray &content);

    QByteArray get_binary();

private:
    void fill_others(const QByteArray &content);
//    QByteArray content;

};

#endif // MSG_H
