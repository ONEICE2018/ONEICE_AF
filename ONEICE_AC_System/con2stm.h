#ifndef CON2STM_H
#define CON2STM_H

#include <QWidget>
#include <QtSerialPort>
#include "QTcpSocket"
#include <QUdpSocket>
#include <QThread>
#include "qextserialport.h"
#include "win_qextserialport.h"
#include "qextserialenumerator.h"
#include "qextserialport.h"
#include "qextserialbase.h"
#include "mainwindow.h"
#define CF_DATA 0

namespace Ui {
class con2stm;
}
class heart;
class con2stm : public QWidget
{
    Q_OBJECT

public:
    explicit con2stm(QWidget *parent = 0);
    ~con2stm();
    static con2stm *cs;
    static int UDP_CON;
    static int TCP_CON;
    static int USART_CON;

protected:
    void closeEvent(QCloseEvent *event);
signals:
    void dataget(int,QByteArray);
private slots:
    void stm_tcpdisconnected();
    void stm_tcpback();
    void stm_senddata(int w,QByteArray data);
    void on_con_2tcp_clicked();

    void on_con_2udp_clicked();

    void on_con_2usart_clicked();
    void stm_serialread();
    void stm_findserial(int w);
    void on_con_clearsend_clicked();
    void on_con_clearget_clicked();

    void on_tcp_sendtest_clicked();

private:
    Ui::con2stm *ui;
    QTcpSocket *socket ;
    void init();
    heart *findserial=NULL;
    QString nowcon;
    Win_QextSerialPort *stmserial;
    void stm_usartsend(QByteArray data );
    void showsend(QString send);
    void showget(QString get);
    void analysis_backdata(QByteArray data);

    QString TCP_IP="";
    int  TCP_PORT=0;
    void stm_TCPsend(QByteArray data );



};

#endif // CON2STM_H
