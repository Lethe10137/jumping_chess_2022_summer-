#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#include "ui_main_window.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QMessageBox>
#include "gamemanager.h"
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    Ui::MainWindow *ui;
    GameManager * gm;
    QMessageBox * createServer, * connectSuccess,* win_or_lose;
    QLineEdit * connectServer_edit;
    QWidget *connectServer;
    QAction* start;
    QAction* admit_defeat;
    QAction* creat_the_conection;
    QAction* connect_to_server;
    QMenu* connectMenu;
    QPushButton *cancelbtn_of_create_connection;



signals:
    void want_to_play(bool,bool);



public slots:
    void make_board_visible_or_not(bool b = false){
        ui->myBoard->setVisible(b);
    }
    void on_create_the_connection_triggered();
    void on_Start_triggered();
    void on_connect_to_server_triggered();
    void on_admit_defeat_triggered();
    void connect_to_server_ok_puhsed();
    void connect_to_server_cancel_puhsed();

    void create_connection_cancel_pushed();
};
#endif // MAINWINDOW_H
