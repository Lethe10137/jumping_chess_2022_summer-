#ifndef MYLCD_H
#define MYLCD_H

#include <QWidget>
#include <QLCDNumber>

class MyLCD : public QLCDNumber
{
    Q_OBJECT
public:
    MyLCD(QWidget *parent = nullptr);

signals:
    void become_zero();

public slots:
    void set20(){
        this->display(20);
    }
    void set0(){
        this->display(0);
    }
    void bevisible(){
        this->setVisible(true);
    }
    void beinvisible(){
        this->setVisible(false);
    }
    void minusone(){

        int k = this->intValue();
        if(k <= 0)return;
        k--;
        this->display(k);
        if(k == 0){
            emit become_zero();
        }
        return;
    }
    void plusone(){

        int k = this->intValue();
        k++;
        this->display(k);

        return;
    }
};

#endif // MYLCD_H
