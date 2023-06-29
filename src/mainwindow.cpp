#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include "board.h"
#include "gamemanager.h"
#include <QByteArray>
#include "msg.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Jumping Chess");

    //tool bar
    QMenuBar *bar = menuBar();
    connectMenu = bar -> addMenu ("Connect");
    QMenu *playMenu = bar -> addMenu("Play");
    QAction* creat_the_connection = connectMenu -> addAction("Create the connection");
    connect(creat_the_connection, SIGNAL(triggered()), this, SLOT(on_create_the_connection_triggered()));

    connectMenu -> addSeparator();

    QAction* connect_to_server = connectMenu -> addAction("Connect to server");

    connect(connect_to_server, SIGNAL(triggered()), this, SLOT(on_connect_to_server_triggered()));

    start =  playMenu -> addAction("Start");
    connect(start, SIGNAL(triggered()), this, SLOT(on_Start_triggered()));
    playMenu -> addSeparator();
    start->setEnabled(false);



     admit_defeat = playMenu -> addAction("Admit Defeat");
     admit_defeat->setEnabled(false);
    connect(admit_defeat, SIGNAL(triggered()), this, SLOT(on_admit_defeat_triggered()));


    //Status bar
    QStatusBar* sBar = statusBar();
    this -> setStatusBar(sBar);
    QLabel *label1 = new QLabel("",this);
    QLabel *label2 = new QLabel("Lethe",this);
    sBar->addWidget(label1);
    sBar->addPermanentWidget(label2);

    Board* _board = ui->myBoard;
    SideBar* _sidebar = ui->sidebar;
    gm = new GameManager(start, admit_defeat,label1,label2,_board,_sidebar,this);

    createServer = new QMessageBox(this);
    createServer->setWindowTitle("Listening");
    createServer->addButton(QMessageBox::Ok);

    QPushButton *cancelbtn_of_create_connection = new QPushButton("cancel");
    createServer->addButton(cancelbtn_of_create_connection,QMessageBox::RejectRole);



    connectServer = new QWidget();
    connectServer->setWindowTitle("Connecting");



    connectServer_edit = new QLineEdit(connectServer);
    connectServer_edit->setPlaceholderText("e.g. 183.173.105.188");
    connectServer_edit->setFixedSize(300,50);
    QVBoxLayout *layout = new QVBoxLayout;
    QLabel* text_in_window = new QLabel(connectServer);
    QPushButton *ok = new QPushButton(connectServer);
    ok->setText("ok");
    connect(ok,&QPushButton::clicked,this,&MainWindow::connect_to_server_ok_puhsed);

    QPushButton *cancel = new QPushButton(connectServer);
    cancel->setText("cancel");
    connect(cancel,&QPushButton::clicked,this,&MainWindow::connect_to_server_cancel_puhsed);

    text_in_window->setText("Please enter server IP");
    layout->addWidget(text_in_window);
    layout->addWidget(connectServer_edit);
    layout->addWidget(ok);
    layout->addWidget(cancel);
    connectServer->setLayout(layout);

    connectServer->adjustSize();

    connectSuccess = new QMessageBox(this);
    connectSuccess->setWindowTitle("Connection");
    connectSuccess->setText("Connection is ready");

    win_or_lose = new QMessageBox(this);
    win_or_lose -> setWindowTitle("Game Over");


//    Msg test;
//    test.my_msg = timeout;
//    test.my_request = you_move_first;
//    test.last_from = 60;
//    test.last_to = 79;
//    test.round_count = 65000;
//    test.first_player_time_out = 2;
//    test.last_player_time_out = 3;
//    for(int i =0 ; i < 121; i++)test.lattice_status[i]=i%3;

//    QByteArray qt = test.get_binary();

//    Msg test2(qt);

//    bool ans =   (test.first_player_time_out == test2.first_player_time_out)&&
//            ( test.last_player_time_out == test2.last_player_time_out)&&
//            (test.last_from == test2.last_from)&&
//            (test.last_to == test2.last_to )&&
//            (test.my_request == test2.my_request)&&
//            (test.my_msg == test2.my_msg)&&
//            (test.round_count == test2.round_count);

//    for(int i = 0; i < 121; i++){
//        if(test.lattice_status[i] != test2.lattice_status[i]){
//            ans = false;
//        }
//    }

//    qDebug() << "result:"<<ans;


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Start_triggered(){
    qDebug()<<"click1";
    gm -> set_want_to_play(true,true);

}

void MainWindow::on_create_the_connection_triggered(){

    QString text = "Listening on" + gm->setRole(server) + ":8888";



    createServer ->setText(text);
    gm->_label->setText(text);
    createServer ->show();
    createServer ->exec();
    if(createServer->clickedButton() == cancelbtn_of_create_connection){
        create_connection_cancel_pushed();
    }



}

void MainWindow::create_connection_cancel_pushed(){
    qDebug() << "cancel_pushed";

    if(gm->listenSocket_s) {QObject::disconnect(this->gm->listenSocket_s,SIGNAL(newConnection()),this->gm,SLOT(acceptConnection_s()));
        gm->listenSocket_s->close();
        delete gm->listenSocket_s;
        }
    gm->listenSocket_s =nullptr;
    gm->_label->setText("Canceled");
}

void MainWindow::on_connect_to_server_triggered(){
    connectServer->show();
}

void MainWindow::connect_to_server_ok_puhsed(){
    QString user_input = connectServer_edit->text();
    bool everything_right = true;
    try {
        QStringList ips = user_input.split(".");
        if(ips.size() == 4){
            int ip0 = ips.at(0).toInt();
            int ip1 = ips.at(1).toInt();
            int ip2 = ips.at(2).toInt();
            int ip3 = ips.at(3).toInt();
            if(ip0 <1 || ip0 > 255)throw 1;
            if(ip1 <0 || ip1 > 255)throw 1;
            if(ip2 <0 || ip2 > 255)throw 1;
            if(ip3 <0 || ip3 > 255)throw 1;
        }else{
              throw 1;
        }
    } catch(...){
        everything_right = false;
    }
    if(everything_right){
        gm->setRole(client,user_input);
        connectServer->close();
    }else{
        connectServer_edit->clear();
        connectServer_edit->setPlaceholderText("Invalid IPv4! Please try again.");
    }
}

void MainWindow::connect_to_server_cancel_puhsed(){

    connectServer->close();
}
void MainWindow::on_admit_defeat_triggered(){
    gm->defeat_handler();
}
