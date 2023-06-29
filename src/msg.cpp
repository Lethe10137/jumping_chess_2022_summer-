#include "msg.h"
#include "qdebug.h"
#include <QtEndian>

void Msg::set_binary(const QByteArray &content_raw){



    const unsigned char * half_content = (unsigned char*)content_raw.constData();

   unsigned char content[36];

    for(int i = 0; i < 36 ; i++){
        int k = (half_content[i*2]-40) * 16 + (half_content[i*2 +1]-40);
        content[i] = k;
        }
        unsigned char cur = content[0];
        unsigned char msg_id = cur >> 4;
        switch(msg_id){
            case 0: my_msg = blank;break;
            case 1: my_msg = timeout;break;
            case 2: my_msg = fatal_timeout;break;
            case 3: my_msg = admit_defeat;break;
            case 4: my_msg = fatal_not_moving_enough;break;
            case 8: my_msg = winning;break;
            case 9: my_msg = sending_invitation;break;
            case 11: my_msg = accepting_invitation;break;
            case 10: my_msg = refusing_invitation;break;
            case 12: my_msg = game_start;break;
        }

        if(msg_id <= 8){
            fill_others(content_raw);
        }
}


Msg::Msg(const QByteArray &content_raw)
 {
    set_binary(content_raw);
}

void Msg::fill_others(const QByteArray & content_raw){
    const unsigned char * half_content = (unsigned char*)content_raw.constData();

   unsigned char content[36];

    for(int i = 0; i < 36 ; i++){
        int k = (half_content[i*2]-40) * 16 + (half_content[i*2 +1]-40);
        content[i] = k;
        }
   int cur;

    for(int i = 0; i < 31;i++){
        cur = content[i];
        if(i)lattice_status[i * 4 - 2] = (cur >> 6) & 3;
        if(i)lattice_status[i * 4 - 1] = (cur >> 4) & 3;
        lattice_status[i * 4] = (cur >> 2) & 3;
        if(i != 30)lattice_status[i * 4 + 1] = cur & 3;
    }

    cur = ((int)content[30]  ) & 3;
//qDebug() << cur;
    switch(cur){
        case 0: my_request = you_move_first;break;
        case 1: my_request = you_move_second;break;
        case 2: my_request = please_wait;break;
        case 3: my_request = please_stop;break;
    }

    cur = content[31] * 256 + content[32];
    round_count = cur;
    last_from = content[33];
    last_to = content[34];
    cur = content[35] >> 6;
    cur &= 3;
    first_player_time_out = cur;
    cur = content[35] >> 4;
    cur &= 3;
    last_player_time_out = cur;
}

void w(char* des, int value, int bits, int offsite){
    for(int i = 0; i < bits; i++){
        des[offsite + bits - i - 1] = value & 1;
        value >>=1;
    }
}

QByteArray Msg::get_binary(){
    char b[288] = {};
    switch(my_msg){
    case blank: w(b,0,4,0);break;
    case timeout: w(b,1,4,0);;break;
    case fatal_timeout: w(b,2,4,0);;break;
    case admit_defeat: w(b,3,4,0);;break;
    case fatal_not_moving_enough: w(b,4,4,0);;break;
    case winning: w(b,8,4,0);break;
    case sending_invitation: w(b,9,4,0);;break;
    case accepting_invitation: w(b,11,4,0);;break;
    case refusing_invitation: w(b,10,4,0);;break;
    case game_start: w(b,12,4,0);break;
   }
    for(int i = 0; i < 121 ; i ++){
        w(b,lattice_status[i],2,4+i*2);
    }
    switch(my_request){
        case you_move_first : w(b,0,2,246);break;
        case you_move_second : w(b,1,2,246);break;
        case please_wait : w(b,2,2,246);break;

        case please_stop : w(b,3,2,246);break;
    }

    qDebug() << (int)b[246] <<(int) b[247];
    w(b,round_count,16,248);
    w(b,last_from,8,264);
    w(b,last_to,8,272);
    w(b,first_player_time_out,2,280);
    w(b,last_player_time_out,2,282);
    w(b,0,4,284);

    QByteArray ans;
    for(int i = 0; i < 72;i++){
        unsigned char tmp = 0;
        for(int j = 0; j < 4; j ++){
            tmp <<= 1;
            tmp += b[i*4+j];
        }
        tmp += 40;
        ans.append(tmp);
    }
    return ans;
}
