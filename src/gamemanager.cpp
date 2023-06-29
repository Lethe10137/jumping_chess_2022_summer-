#include "gamemanager.h"
#include "mainwindow.h"

GameManager::GameManager(QAction * start, QAction* defeat, QLabel *l, QLabel*l2, Board* b,SideBar* s,QObject *parent)
    : QObject{parent}
{
    _start = start;
    _defeat = defeat;
    _label_right = l2;
    _label = l;
    _mainwindow = static_cast<MainWindow*>(parent);
    _board = b;
    _sidebar = s;
    bool a0 = connect(_board,&Board::successfully_moved,this,&GameManager::step_handler);
    bool a1 = connect(this,SIGNAL(receive_move_first()),this,SLOT(move_first()));
    bool a2 = connect(this,SIGNAL(receive_move_second()),this,SLOT(move_second()));
    bool a3 = connect(this,SIGNAL(receive_wait()),this,SLOT(do_wait()));
    bool a4 = connect(this,SIGNAL(receive_stop()),this,SLOT(do_stop()));
    bool a5 = connect(_sidebar,&SideBar::time_out,this,&GameManager::time_out_handler);

    qDebug() <<"connect status"<< a0 <<a1 << a2 << a3 << a4 << a5;

    mytimer = new QTimer(this);
    connect(mytimer,&QTimer::timeout,_sidebar,&SideBar::timer_tick);



      for(int i = 0; i < 10 ; i++){record_board[i] = 1;record_board[120-i] = 2;}

}

QString GameManager::initServer()
{
//    myrole = server;
    this->listenSocket_s =new QTcpServer;
    this->listenSocket_s->listen(QHostAddress::Any,8888);
    QObject::connect(this->listenSocket_s,SIGNAL(newConnection()),this,SLOT(acceptConnection_s()));

    QString ip = "";
    QString hostname = QHostInfo::localHostName();
    QHostInfo hostinfo = QHostInfo::fromName(hostname);
    QList<QHostAddress> addList = hostinfo.addresses();
    for(auto x : addList){
      // qDebug() << x.toString();
        if(QAbstractSocket::IPv4Protocol==x.protocol()){
            ip = x.toString();
        }
    }

    _label->setText("waiting");
    return ip;


}

void GameManager::acceptConnection_s()
{
    mytimer->start(1000);
    connected = true;
    static_cast<MainWindow*>(_mainwindow)->connectMenu->setEnabled(false);
    this->readWriteSocket =this->listenSocket_s->nextPendingConnection();
    QObject::connect(this->readWriteSocket,SIGNAL(readyRead()),this,SLOT(recvMessage()));

    Msg msg_blank;
    QByteArray blank_array = msg_blank.get_binary();
    send(blank_array);

    _label->setText("connected");
    _start->setEnabled(true);




}

void GameManager::handle_receive(){
    _label->setText("connected");
    _start->setEnabled(true);

   connected = true;
   static_cast<MainWindow*>(_mainwindow)->connectMenu->setEnabled(false);
   msg_got = Msg(info_got);

//   QString n;
//   char * d = info_got.data();
//    for(int i = 0 ; i < 72;i++){
//        int k = d[i];
//        n = n + QString::number(k)+",";
//    }

//   qDebug() << "got a message" << (int)msg_got.my_request<<(int)msg_got.my_msg
//            <<msg_got.first_player_time_out<<msg_got.last_player_time_out<< n;

   request request_got = msg_got.my_request;
//   qDebug() <<"in handle_receive"<<msg_got.lattice_status[0];
   this->update_status();

   switch(request_got){
        case you_move_first:
            emit receive_move_first();
            break;
        case you_move_second:
            emit receive_move_second();
            break;
        case please_wait:
            emit receive_wait();
            break;
        case please_stop:
            emit receive_stop();
            break;
    }
  info_got.resize(0);
  get_count = 0;

}



void GameManager::recvMessage()
{   

   QByteArray this_time_get  = this->readWriteSocket->readAll();
   get_count += this_time_get.size();
   info_got.append(this_time_get);
   if(get_count == 72)handle_receive();

}

void GameManager::send(const QByteArray& array)
{


//    QString n;

//     for(int i = 0 ; i < 72;i++){
//         int k = array[i];
//         n = n + QString::number(k)+",";
//     }

//     Msg msg_got(array);
//     qDebug() << "sent a message" << (int)msg_got.my_request<<(int)msg_got.my_msg
//              <<msg_got.first_player_time_out<<msg_got.last_player_time_out<< n;

    this->readWriteSocket->write(array.data());


}

QString GameManager::setRole(role me, QString host){
    if(me == server){
        myrole = server;
        _label_right->setText("server");
        _board->paint_or_not = true;
        _sidebar->paint_or_not = true;
        _board->update();
        _sidebar->update();
        return initServer();
    }else{
        myrole = client;
        _label_right->setText("client");
        _board->paint_or_not = true;
        _sidebar->paint_or_not = true;
        _board->update();
        _sidebar->update();
        host_address = host;
        connectHost();
        return "";
    }
}

void GameManager::connectHost()
{
    this->readWriteSocket = new QTcpSocket;
    this->readWriteSocket->connectToHost(QHostAddress(host_address),8888);
    QObject::connect(this->readWriteSocket,SIGNAL(readyRead()),this,SLOT(recvMessage()));
    mytimer->start(1000);
}

void GameManager::update_status(){
    qDebug()<<"entering updating status";

    for(int i = 0; i < 121; i++)lattice_back_up[i] = msg_got.lattice_status[i];

    _board->set_last_move(msg_got.last_from,msg_got.last_to);
    _board->set_lattice(msg_got.lattice_status);
    _board->repaint();
    _sidebar->set_time_out(msg_got.first_player_time_out,msg_got.last_player_time_out);
    round_count = msg_got.round_count;

    if(msg_got.my_request == you_move_first){
        qDebug()<<"get you move first";
        _board->set_id(1);
        _sidebar->set_your_id(1);
        round_count++;
    }
    if(msg_got.my_request == you_move_second){
         qDebug()<<"get you move second";
        _board->set_id(2);
        _sidebar->set_your_id(2);
    }

    _sidebar->set_round_count(round_count);

    if(round_count >= 20){qDebug()<<"able to defeat";_defeat->setEnabled(true);}


    if(msg_got.my_request == please_wait){
         qDebug()<<"get please wait";
        if(msg_got.my_msg == game_start){
            _board->set_id(2);
            _sidebar->set_your_id(2);
            my_id = 2;
            _sidebar->change_id(1);
            _sidebar->set_round_count(1);
        }

    }
    if(msg_got.my_request == please_stop){
        qDebug()<<"get please stop";

    }
    if(msg_got.my_msg == accepting_invitation || msg_got.my_msg == sending_invitation){
        set_want_to_play(false,true);
    }
    if(msg_got.my_msg == game_start){
        game_on_going = true;
        _label->setText("Game Start!");
        _start->setEnabled(false);
        mytimer->start(1000);
    }

    _board->update();
    _sidebar->update();
}

void GameManager::a_step(){



    _board->move_permitted();
    _board->update();
//    _sidebar->change_id();
    _sidebar->set_timmer(20);
}



void GameManager::move_first(){
     qDebug()<<"move_first_triggered";
     _sidebar->change_id(1);
    my_id = 1;
    a_step();
}

void GameManager::move_second(){
    _sidebar    ->change_id(2);
    qDebug()<<"move_second_triggered";
    my_id = 2;
    a_step();
}

void GameManager::do_wait(){
    qDebug()<<"wait_triggered";
//    _board->move_banned();
}

void GameManager::do_stop(){
    qDebug()<<"stop_triggered";
    _board->move_banned();
    end_status myend = unknown;
    switch(msg_got.my_msg){
        case fatal_timeout:
            myend = win_by_time_out;break;
        case admit_defeat:
            myend = win_by_admit;break;
        case fatal_not_moving_enough:
            myend = win_by_not_moving;break;
        case winning:
             myend = lose_by_moving;break;
        default:
            myend = unknown;
    }
    game_over(myend);
}

void GameManager::set_want_to_play(bool me, bool want){
    qDebug()<<"set_want_to_play"<<me<<I_want_to_play<<you_want_to_play;
    if(me){I_want_to_play = want;}
    else{you_want_to_play = want;}
    if(!want)return;

    if((!game_on_going)&&I_want_to_play && you_want_to_play && connected && myrole == server){
        qDebug() << "all want to play(server)";
        start_game();
        return;
    }
    if((!game_on_going)&&I_want_to_play && you_want_to_play && connected && myrole == client){
        qDebug() << "all want to play(client)";
        Msg blank;
        for(int i = 0; i < 10 ; i++){blank.lattice_status[i] = 1;blank.lattice_status[120-i] = 2;}
        blank.my_request = please_wait;
        blank.my_msg = accepting_invitation;
        send(blank.get_binary());
        return;
    }

    if(me && (!game_on_going) && connected && (!you_want_to_play) &&(I_want_to_play)){
        qDebug() << "only I want to play";
        Msg blank;
        for(int i = 0; i < 10 ; i++){blank.lattice_status[i] = 1;blank.lattice_status[120-i] = 2;}
        blank.my_request = please_wait;
        blank.my_msg = sending_invitation;
        send(blank.get_binary());
        _label->setText("invitation send");
    }
    if((!me) && (!game_on_going) && connected && (you_want_to_play) &&(!I_want_to_play)){
        qDebug() << "only you want to play";
        _label->setText("invitation got , click play->start to accept");
    }

}

void GameManager::start_game(){
    I_want_to_play = false;
    you_want_to_play = false;
    game_on_going = true;

    QTime time(QTime::currentTime());
    int second = time.second();
    my_id = second % 2 + 1;

    my_id = 2;
    _sidebar->set_your_id(my_id);
    _sidebar->set_round_count(1);

    _board->set_id(my_id);
    _sidebar->update();



    if(my_id == 1){
         _sidebar->change_id(2);
        Msg act_later_please;
        for(int i = 0; i < 10 ; i++){act_later_please.lattice_status[i] = 1;act_later_please.lattice_status[120-i] = 2;}
        act_later_please.my_request = please_wait;
        act_later_please.my_msg = game_start;
        act_later_please.round_count = 1;
        send(act_later_please.get_binary());
        _label->setText("Game Start!");
        _sidebar->set_round_count(1);
        round_count = 1;
        _sidebar->update();
        _start->setEnabled(false);
        QMessageBox a;

        move_first();
    }else{
        _sidebar->change_id(1);
        Msg act_please;
        for(int i = 0; i < 10 ; i++){act_please.lattice_status[i] = 1;act_please.lattice_status[120-i] = 2;}
        act_please.my_request = you_move_first;
        act_please.my_msg = blank;
        act_please.round_count = 0;
        round_count = 1;

        send(act_please.get_binary());
    }
}

int GameManager::move_enough_checker(){

    int round = round_count;

    int in_up =0;
    int in_down = 0;
    for(int i = 0; i < 10; i++){
        if(lattice_back_up[i] == my_id)in_up+=1;
        if(lattice_back_up[120-i] == my_id)in_down+=1;
    }
    int remain = (my_id == 1) ? in_up : in_down;
    int arrive = (my_id == 2 ) ? in_up : in_down;

    qDebug() << "checking round" << round <<"remain" <<remain << "arrive" <<arrive;
    if(round == 20 && remain >5)return -1;
    if(round == 25 && remain >2)return -1;
    if(round == 25 && remain > 0)return -1;
    if(arrive == 10)return 1;
    return 0;

}

void GameManager::step_handler(int from,int to){
    _sidebar->set_timmer(-1);
    if(something_wrong) return;
    if(0 <= from && from <=120 && 0 <= to && to <= 120){
        lattice_back_up[to] =  lattice_back_up[from] ;
        lattice_back_up[from] = 0;
    }
    int status = move_enough_checker();
    if(status == 0)normal_step(from,to);
    if(status == 1){win_step(from,to);game_over(win_by_moving);return;}
    if(status == -1){lose_step(from,to);game_over(lose_by_not_moving);return;}


}

void GameManager::normal_step(int from ,int to){
    qDebug() << "peacefully_end_a_step";
    Msg to_return;
    _sidebar->collect_info(to_return); //time out times
    _board->collect_info(to_return); // lattice status
    to_return.last_from = from;
    to_return.last_to = to;

    to_return.round_count = round_count;
    if(my_id == 1)to_return.my_request = you_move_second;
    else to_return.my_request = you_move_first;
    to_return.my_msg = blank;
    send(to_return.get_binary());
    for(int i = 0; i < 121; i++)lattice_back_up[i] = to_return.lattice_status[i];
    _sidebar->change_id();
    _sidebar->set_timmer(20);
//    _sidebar->set_round_count(round_count);
    if(my_id == 2)_sidebar->set_round_count(round_count+1);
}


void GameManager::win_step(int from ,int to){
    qDebug() << "win_step";
    Msg to_return;
    _sidebar->collect_info(to_return); //time out times
    _board->collect_info(to_return); // lattice status
    to_return.last_from = from;
    to_return.last_to = to;

    to_return.round_count = round_count;

   to_return.my_request = please_stop;
    to_return.my_msg = winning;
    send(to_return.get_binary());


}


void GameManager::lose_step(int from ,int to){
    qDebug() << "lose_step";
    Msg to_return;
    _sidebar->collect_info(to_return); //time out times
    _board->collect_info(to_return); // lattice status
    to_return.last_from = from;
    to_return.last_to = to;

    to_return.round_count = round_count;

   to_return.my_request = please_stop;
    to_return.my_msg = fatal_not_moving_enough;
    send(to_return.get_binary());

}





void GameManager::time_out_handler(bool fatal){
    _sidebar->set_timmer(-1);
    if(something_wrong) return;

    int status = move_enough_checker();
    if(status == -1){lose_step(127,127);game_over(lose_by_not_moving);return;}


    qDebug() << "time_out";
    _board->move_banned();
    _board->update();
    something_wrong = true;
    if(fatal){
        fatal_time_out();
    }else{
       normal_time_out();
       something_wrong = false;
    }

}



void GameManager::normal_time_out(){
    Msg to_return;
     qDebug() << "normal_time_out";
    _sidebar->set_timmer(-1);
    _sidebar->collect_info(to_return); //time out times
    for(int i = 0; i < 121; i++) to_return.lattice_status[i] = lattice_back_up[i] ;
    to_return.last_from = 127;
    to_return.last_to = 127;
    to_return.round_count = round_count;
    if(my_id == 1)to_return.my_request = you_move_second;
    else to_return.my_request = you_move_first;
    to_return.my_msg = timeout;
    qDebug() << "time_out_count in normal" << to_return.first_player_time_out<<to_return.last_player_time_out;
    send(to_return.get_binary());

    _sidebar->change_id();
    _sidebar->set_timmer(20);
    if(my_id == 2)_sidebar->set_round_count(round_count+1);

}

void GameManager::fatal_time_out(){
    qDebug() << "fatal_time_out";
    Msg to_return;
    _sidebar->set_timmer(-1);
    _sidebar->collect_info(to_return); //time out times
    for(int i = 0; i < 121; i++) to_return.lattice_status[i] = lattice_back_up[i] ;
    to_return.last_from = 127;
    to_return.last_to = 127;
    to_return.round_count = round_count;
    to_return.my_request = please_stop;
    to_return.my_msg = fatal_timeout;
    send(to_return.get_binary());


    qDebug() << "time_out_count" << to_return.first_player_time_out<<to_return.last_player_time_out;
    game_over(lose_by_time_out);
}

void GameManager::defeat_handler(){
    _sidebar->set_timmer(-1);
    if(something_wrong) return;
    Msg to_return;
    _sidebar->set_timmer(-1);
    _sidebar->collect_info(to_return); //time out times
    for(int i = 0; i < 121; i++) to_return.lattice_status[i] = lattice_back_up[i] ;
    to_return.last_from = 127;
    to_return.last_to = 127;
    to_return.round_count = round_count;
    to_return.my_request = please_stop;
    to_return.my_msg = admit_defeat;
    send(to_return.get_binary());
    game_over(lose_by_admit);

}



void GameManager::game_over(end_status ending){
    something_wrong = true;
    _sidebar->set_timmer(-1);

    _board->move_banned();
    QMessageBox msg;
    QString text;
    switch (ending){
    case unknown:
    text = "unknown";break;
    case win_by_moving:
    text = "You win since all your chess pieces arrived!";break;
    case lose_by_moving:
    text = "You lose since all your opponent's chess pieces arrived!!";break;
    case win_by_not_moving:
    text = "You win since your opponent remains too many chess pieces in his original field!";break;
    case lose_by_not_moving:
    text = "You lose since you remain too many chess pieces in your original field!";break;
    case win_by_time_out:
    text = "You win since your opponent has 3 timeouts! ";break;
    case lose_by_time_out:
    text = "You lose since you have 3 timeouts!";_sidebar->timer_tick();break;
    case win_by_admit:
    text = "Your opponent has admitted defeat!";break;
    case lose_by_admit:
    text = "You have admitted defeat!";break;
    }


    msg.setText(text);

    msg.exec();

}
