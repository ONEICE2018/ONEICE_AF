#include "con2stm.h"
#include "ui_con2stm.h"
#include "QMessageBox"
#include "mainwindow.h"
con2stm* con2stm::cs = NULL;
int con2stm::UDP_CON  = 0;
int con2stm::TCP_CON  = 0;
int con2stm::USART_CON = 0;
void con2stm::stm_senddata(int w, QByteArray data)
{
    if(USART_CON == 1)
    {
        if(data.length() > 0)
        {
            stm_usartsend(data);
        }
        else
        {
            showsend("NULL data");
        }
    }
    if(TCP_CON == 1)
    {
        if(data.length() > 0)
        {
            stm_TCPsend(data);
        }
        else
        {
            showsend("NULL data");
        }
    }
}
con2stm::con2stm(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::con2stm)
{
    ui->setupUi(this);
    cs = this;
    setWindowFlags(Qt::FramelessWindowHint);//无边框
    if(curve_fitting::cf != NULL)
    {
        QObject::connect(this, SIGNAL(dataget(int, QByteArray)), curve_fitting::cf, SLOT(stm_acdataget(int, QByteArray)));
        QObject::connect(curve_fitting::cf, SIGNAL(cfdatasend(int, QByteArray)), this, SLOT(stm_senddata(int, QByteArray)));
    }
    init();
}

con2stm::~con2stm()
{
    delete ui;
}
void con2stm::closeEvent(QCloseEvent* event)
{
    if(MainWindow::goto_once_con2stm == 1)
    {
        MainWindow::goto_once_con2stm = 0;
        cs = NULL;
        if(findserial != NULL)
        {
            QObject::disconnect(findserial, SIGNAL(condorefserial(int)), this, SLOT(stm_findserial(int)));
            findserial->quit();
        }
        if(ui->con_2usart->text() == "DISCON")
        {
            nowcon = "NULL";
            //stmserial->close();
            stmserial->disconnect();
            QObject::disconnect(stmserial, SIGNAL(readyRead()), this, SLOT(stm_serialread()));
        }
        if(curve_fitting::cf != NULL)
        {
            QObject::disconnect(this, SIGNAL(dataget(int, QByteArray)), curve_fitting::cf, SLOT(stm_acdataget(int w, QByteArray data)));
            QObject::disconnect(curve_fitting::cf, SIGNAL(cfdatasend(int, QByteArray)), this, SLOT(stm_senddata(int, QByteArray)));
        }
    }
}
void con2stm::init()
{
    findserial = new heart(DOREFSTMSERIALPORT);
    QObject::connect(findserial, SIGNAL(condorefserial(int)), this, SLOT(stm_findserial(int)));
    findserial->start();
}
void con2stm::stm_findserial(int w)
{
    if(w == DOREFSTMSERIALPORT)
    {
        QList<QString> newportlist;
        foreach(const QSerialPortInfo& info, QSerialPortInfo::availablePorts())
        {
            newportlist.append(info.portName());
        }
        if(ui->con_2usart->text() == "CON")
        {
            if(newportlist.count() != ui->con_usartcombox->count())
            {
                ui->con_usartcombox->clear();
                ui->con_usartcombox->addItems(newportlist);
            }
        }
        else
        {
            if(!newportlist.contains(nowcon))
            {
                QObject::disconnect(stmserial, SIGNAL(readyRead()), this, SLOT(stm_serialread()));
                //关闭串口
                //DCserial->close();
                stmserial->disconnect();
                nowcon = "NULL";
                ui->con_2usart->setText("CON");
                USART_CON = 0;
            }
        }
    }
}
void con2stm::on_con_2usart_clicked()
{
    if(ui->con_2usart->text() == tr("CON"))
    {
        //portSettings = { BAUD57600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 2 };
        //设置串口名
        stmserial = new Win_QextSerialPort(ui->con_usartcombox->currentText(), QextSerialBase::EventDriven);
        //打开串口
        if(!stmserial->open(QIODevice::ReadWrite))
        {
            QMessageBox::about(NULL, "msg", "can not open serial port！");
            return;
        }
        // DCserial= new Win_QextSerialPort(QextSerialPort::EventDriven,this);
        stmserial->setPortName(ui->con_usartcombox->currentText());
        //设置波特率
        stmserial->setBaudRate(BAUD115200);
        //设置数据位数
        stmserial->setDataBits(DATA_8);
        //设置奇偶校验
        stmserial->setParity(PAR_NONE);
        //设置停止位
        stmserial->setStopBits(STOP_1);
        //设置流控制
        stmserial->setFlowControl(FLOW_OFF);
        USART_CON = 1;
        ui->con_2usart->setText("DISCON");
        QObject::connect(stmserial, SIGNAL(readyRead()), this, SLOT(stm_serialread()));
        nowcon = ui->con_usartcombox->currentText();
        // stm_usartsend(QString("asdasasd").toLatin1().data());
    }
    else
    {
        USART_CON = 0;
        ui->con_2usart->setText("CON");
        nowcon = "NULL";
        stmserial->disconnect();
        stmserial->close();
        QObject::disconnect(stmserial, SIGNAL(readyRead()), this, SLOT(stm_serialread()));
    }
}
void con2stm::on_con_2tcp_clicked()
{
    if(ui->con_2tcp->text() == "CON")
    {
        QString IP = ui->con_eidt_tcpip->text();
        if( IP.split(".").count() != 4)
        {
            QMessageBox::about(NULL, "错误", "请输入正确的IP!");
        }
        else
        {
            if(ui->con_eidt_tcpport->text() != "")
            {
                int Port = ui->con_eidt_tcpport->text().toInt();
                TCP_IP = IP;
                TCP_PORT = Port;
                TCP_CON = 1;
                ui->con_2tcp->setText("DIS");
                socket = new QTcpSocket();
                QObject::connect(socket,  SIGNAL(readyRead()), this, SLOT(stm_tcpback()));
                QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(stm_tcpdisconnected()));
                socket->connectToHost(TCP_IP, TCP_PORT);
            }
        }
    }
    else
    {
        if(TCP_CON == 1)
        {
            socket->disconnectFromHost();
            QObject::disconnect(socket,  SIGNAL(readyRead()), this, SLOT(stm_tcpback()));
            QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(stm_tcpdisconnected()));
            delete socket;
        }
        TCP_CON = 0;
        ui->con_2tcp->setText("CON");
    }
//     if(TorU==0){
//         //TCP客户端
//     socket = new QTcpSocket();
//     QObject::connect(socket, &QTcpSocket::readyRead, this, &MainWindow::socket_Read_Data);
//     QObject::connect(socket, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);
//     socket->connectToHost(IP,Port);
//     }else{
//        //  if(UCsocket->bind(Port,QUdpSocket::ShareAddress)&&UCsocket->state()!=0){
//       //UCsocket
//              UDPIP=IP;
//              UDPPort=Port;
//              UCsocket->connectToHost(UDPIP,UDPPort,QIODevice::ReadWrite);
////    }else{
////          showmsgs("服务开启失败");
////       }
//     }
//    // socket->write("634");
//     ui->CconnectS->setText("关闭服务端连接");
//     CconS_OK=1;
//}
//    }else{
//        CconS_OK=0;
//        askT=13;
//        UCsocket->disconnectFromHost();
//        QObject::disconnect(socket, &QTcpSocket::readyRead, this, &MainWindow::socket_Read_Data);
//        QObject::disconnect(socket, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);
//        socket->disconnectFromHost();
//         ui->CconnectS->setText("打开服务端连接");
//    }
}
void con2stm::stm_tcpdisconnected()
{
    if(TCP_CON == 1)
    {
        socket->disconnectFromHost();
        QObject::disconnect(socket,  SIGNAL(readyRead()), this, SLOT(stm_tcpback()));
        QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(stm_tcpdisconnected()));
//      delete socket;
        TCP_CON = 0;
        ui->con_2tcp->setText("CON");
    }
}

//DC发送
void con2stm::stm_usartsend(QByteArray data)
{
    if(ui->con_2usart->text() == tr("DISCON"))
    {
        stmserial->write(data, data.length());
    }
    else
    {
        showsend("not con");
    }
}
void con2stm::on_tcp_sendtest_clicked()
{
    MainWindow::nowMainWindow->callcurvefitting();
    setWindowState(Qt::WindowMinimized);
}

void con2stm::stm_TCPsend(QByteArray data )
{
    if(TCP_CON == 1)
    {
        if(socket->state() == QTcpSocket::ConnectedState)
        {
            socket->write(data, data.length());
        }
        else
        {
            stm_tcpdisconnected();
        }
    }
}
/*
QTcpSocket类的方法connectToHost会泄露内存，即使把调用这个方法的QTcpSocket实例delete掉，内存也不会释放！反复connectToHost会导致段错误，十分危险。必须控制connectToHost的使用次数！
所以采用长连接
*/
void con2stm::stm_tcpback()
{
    QByteArray   tcpbackdata = socket->readAll();
    showget(QString(tcpbackdata));
    analysis_backdata(tcpbackdata);
}

void con2stm::stm_serialread()
{
    if(ui->con_2usart->text() == "DISCON")
    {
        QByteArray back = stmserial->readAll();
        if(back.length() > 0)
        {
            showget(QString(back));
            analysis_backdata(back);
        }
    }
}
void con2stm::analysis_backdata(QByteArray data)
{
    emit dataget(CF_DATA, data);
}

void con2stm::on_con_2udp_clicked()
{
}
void con2stm::showsend(QString send)
{
    QString msgs = ui->con_sendtextBrowser->toPlainText();
    if(msgs.length() > 5000)
    {
        msgs = "";
    }
    msgs.append(send);
    msgs.append("\n");
    ui->con_sendtextBrowser->setText(msgs);
}
void con2stm::showget(QString get)
{
    QString msgs = ui->con_gettextBrowser->toPlainText();
    if(msgs.length() > 200)
    {
        msgs = "";
    }
    msgs.append(get);
    msgs.append("\n");
    ui->con_gettextBrowser->setText(msgs);
}



void con2stm::on_con_clearsend_clicked()
{
    ui->con_sendtextBrowser->clear();
}

void con2stm::on_con_clearget_clicked()
{
    ui->con_gettextBrowser->clear();
}

void con2stm::on_con2stmclose_clicked()
{
    setWindowState(Qt::WindowMinimized);//最小化
}
