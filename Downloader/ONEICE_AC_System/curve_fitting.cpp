#include "curve_fitting.h"
#include "ui_curve_fitting.h"
#include "QMessageBox"
#include <qinputdialog.h>
#include "onelistwidget.h"
#include "onecurve.h"
/**
沃特塞恩自动曲线拟合:
所有仪器增加数据访问锁.当此特殊功能——自动曲线拟合运行时，对应的主界面上位机禁止主动访问数据。
**/

bool  curve_fitting::eventFilter(QObject* watched, QEvent* event)
{
    if( watched == ui->Panel_CPUUsed && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->Panel_CPUUsed);
        // 反走样
        painter.setRenderHint(QPainter::Antialiasing, true);
        if(DownDataSize > 0)
        {
            // 设置画笔颜色
            if(CPU_used < 80)
            {
                painter.setPen(QPen(QColor(0, 255, 0), 10, Qt::SolidLine));
            }
            else
            {
                painter.setPen(QPen(QColor(255, 100, 10), 10, Qt::SolidLine));
            }
            QRectF rect(ui->Panel_CPUUsed->width() * 0.05, ui->Panel_CPUUsed->height() * 0.05, ui->Panel_CPUUsed->width() * 0.9, ui->Panel_CPUUsed->height() * 0.9);
            painter.drawArc(rect, 270 * 16, (360 * CPU_used / 100) * 16);
            ui->text_CPUUsed->setText(QString("CPUUsed:%1%").arg(CPU_used));
            ui->cpuused_fname->setText("name:" + cpuused_fname);
            // painter.drawLine(QPointF(10,10),QPointF( 100, 100));
        }
    }
}



int curve_fitting::powerasked = 0;
curve_fitting* curve_fitting::cf = NULL;
void curve_fitting::cfsendcon(QByteArray data)
{
    if(con2stm::cs != NULL && (con2stm::UDP_CON == 1 || con2stm::TCP_CON == 1 || con2stm::USART_CON == 1))
    {
        emit cfdatasend(CF_DATA, data);
    }
    else
    {
        showmsg("please goto con2stm to connect anyone");
    }
}

void curve_fitting::stm_acdataget(int w, QByteArray databk) //访问的数据反馈
{
    if(w == CF_DATA)
    {
        uchar data[databk.length()];
        for(int i = 0; i < databk.length(); i++)
        {
            data[i] = (uchar)databk[i];
        }
        //////////数据上载
        if(data[0] == 'A' && data[1] == 'F' && data[2] == '_' && data[3] == 'U' && data[4] == 'P')
        {
            if(data[9] == 'O' && data[10] == 'V' && data[11] == 'E' && data[12] == 'R')
            {
                if(AF_UPDdataTlen == data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8])
                {
                    showmsg("上载完毕 OK");
                    if(AF_updataFile.isOpen())
                    {
                        AF_updataFile.close();
                    }
                    CF_LoadProject("LoadData");
                }
                else
                {
                    showmsg("上载失败,长度错误");
                }
                if(AF_updataFile.isOpen())
                {
                    AF_updataFile.close();
                }
                ui->progressBar_AFUpload->setValue(0);
            }
            else
            {
                if(  data[8 + data[6]] == 'L' &&
                        data[9 + data[6]] == 'O' &&
                        data[10 + data[6]] == 'A' &&
                        data[11 + data[6]] == 'D' )
                {
                    //过程帧
                    //将数据保存到上载文件夹
                    if(AF_updataFile.isOpen())
                    {
                        char* wd = databk.data() + 7;
                        AF_updataFile.write(wd, data[6]);
                    }
                    ui->progressBar_AFUpload->setValue(data[5]);
                    AF_UPDdataTlen = AF_UPDdataTlen + data[6];
                    cfsendcon("AF_UPOK");//请求下一帧传输
                }
                else
                {
                    if(AF_updataFile.isOpen())
                    {
                        AF_updataFile.close();
                    }
                    showmsg("上载数据格式错误");
                }
            }
        }
        //////////**********
        if(data[0] == 'A' && data[1] == 'F' && data[2] == 'E' && data[3] == 'R' && data[4] == 'R' && data[5] == 'O' && data[6] == 0)
        {
            showmsg("指令错误");
            return;
        }
        if(data[0] == 'A' && data[1] == 'F' && data[2] == 'E' && data[3] == 'R' && data[4] == 'R' && data[5] == 'O' && data[6] == 1)
        {
            QMessageBox::about(NULL, "erro", "未成功装载Lineclass");
            return;
        }
        if(data[0] == 'A' && data[1] == 'F' && data[2] == 'E' && data[3] == 'R' && data[4] == 'R' && data[5] == 'O' && data[6] == 2)
        {
            QMessageBox::about(NULL, "erro", "NO AF Data");
            return;
        }
        if( data[0] == 'A'   && data[1] == 'F'   && data[2] == '_'
                && data[3] == 'C'   && data[4] == 'L'   && data[5] == 'A' && data[6] == 'S'  && data[7] == 'S'
                && data[12] == 'S'  && data[13] == 'E'  && data[14] == 'T')
        {
            int classok = 1;
            //索引
            if(!(CJ_I == "FREQ" && data[8] == AF_I_FREQ))
            {
                classok = 0;
            }
            //自变量
            if(!(CJ_V == "DAC" && data[9] == AF_V_DAC) &&
                    !(CJ_V == "TIME" && data[9] == AF_V_TIME) &&
                    !(CJ_V == "PSdBm" && data[9] == AF_V_PSdBm)
              )
            {
                classok = 0;
            }
            //因变量X
            if(!(CJ_X == "ADC" && data[10] == AF_X_ADC) &&
                    !(CJ_X == "POWER" && data[10] == AF_X_POWER)
              )
            {
                classok = 0;
            }
            //因变量Y
            if(!(CJ_Y == "POWER" && data[11] == AF_Y_POWER) &&
                    !(CJ_Y == "POWERF" && data[11] == AF_Y_POWERF) &&
                    !(CJ_Y == "DAC" && data[11] == AF_Y_DAC)
              )
            {
                classok = 0;
            }
            if(classok == 0)
            {
                QMessageBox::about(NULL, "erro", "未成功装载Lineclass");
                ui->AF_LineClass->setText("LineClass:___");
                ui->LED_lineclassok->setStyleSheet("background-color: rgb(0, 0, 0);");
            }
            else
            {
                ui->AF_LineClass->setText("LineClass:" + CJ_C);
                ui->LED_lineclassok->setStyleSheet("background-color: rgb(0, 255, 0);");
            }
            return;
        }
        if(  data[0] == 'A'  && data[1] == 'F'  && data[2] == '_' && data[3] == 'P'  && data[4] == 'O'  && data[5] == 'W' && data[6] == 'E'  && data[7] == 'R'  && data[8] == '_'
                && data[9] == 'O' && data[10] == 'N')
        {
            if(data[11] == 1)
            {
                ui->AF_OpenLoop_POWERSW->setText("POWEROFF");
                showmsg("STPOWER ON");
                openloop_PowerON = true;
            }
            else if(data[11] == 0)
            {
                ui->AF_OpenLoop_POWERSW->setText("POWERON");
                showmsg("STPOWER OFF");
                openloop_PowerON = false;
            }
            else
            {
                showmsg("STPOWER ERRO");
            }
            return;
        }
        if(  data[0] == 'A'  && data[1] == 'F'  && data[2] == '_'
                && data[3] == 'R'  && data[4] == 'F'  && data[5] == '_'
                && data[6] == 'O' && data[7] == 'N')
        {
            if(data[8] == 1)
            {
                ui->AF_OpenLoop_RFSW->setText("RFOFF");
                showmsg("STRF ON");
                openloop_RFON = true;
            }
            else if(data[8] == 0)
            {
                ui->AF_OpenLoop_RFSW->setText("RFON");
                showmsg("STRF OFF");
                openloop_RFON = false;
            }
            else
            {
                showmsg("STRF ERRO");
            }
            return;
        }
        if(  data[0] == 'A'   && data[1] == 'F'   && data[2] == '_'
                && data[3] == 'I'   && data[4] == 'V'
                && data[9] == 'V'   && data[10] == 'A'   && data[11] == 'L'   && data[12] == 'U' && data[13] == 'E') //AF_IV
        {
            // data[5]; bd[6] =data[6]; bd[7] =data[7]; bd[8] =data[8];
            uint16_t bki = (data[5] << 8 | data[6]);
            if(CJ_IV != bki)
            {
                ui->LED_lineindex_ok->setStyleSheet("background-color: rgb(0, 0, 0);");
                if(AF_START)//曲线数据错误结束自校准
                {
                    showmsg("AF ERRO CJ_IV!=bki");
                    AF_AC_Stop();
                }
            }
            else
            {
                ui->LED_lineindex_ok->setStyleSheet("background-color: rgb(0, 255, 0);");
                ui->AF_Value_I->setText(QString("%1").arg(CJ_IV));
                MainWindow::nowMainWindow->syncdata_pm("pm_stm_Index", QString("%1").arg(CJ_IV));
                CJ_VV = data[7] << 8 | data[8];
                ui->AF_Value_V->setText(QString("%1").arg(CJ_VV));
                MainWindow::nowMainWindow->syncdata_pm("pm_stm_Variable", QString("%1").arg(CJ_VV));
                ui->openloop_varible->setText(QString("V:%1").arg(CJ_VV));
                ui->horizontalSlider_set_Variable->setValue(CJ_VV);
                ui->LED_linevariable_ok->setStyleSheet("background-color: rgb(0, 255, 0);");
                if(AF_START)
                {
                    ui->progress_ACC->setText(QString("%1:").arg(CJ_IV));
                    if(ui->AF_StartAF->text() != "ACStop")
                    {
                        ui->AF_StartAF->setText("AC_Stop");
                    }
                    AF_now_OKCount = 0;
                    on_AF_askonceXY_clicked();
                }
            }
            return;
        }
        if(  data[0] == 'A'   && data[1] == 'F'   && data[2] == '_'
                && data[3] == 'X'   && data[4] == 'Y')                    //AF_XY
        {
            ST_X = data[5] << 8 | data[6];
            ST_Y = data[7] << 8 | data[8];
            // MainWindow::CENTERdata.value("PM").value("pm_lowervalue")));pm_upervalue
            if(CJ_X == "ADC" && (CJ_Y == "POWER" || CJ_Y == "POWERF"))
            {
                CJ_XV = ST_X;
                MainWindow::nowMainWindow->syncdata_pm("pm_stm_ADC", QString("%1").arg(CJ_XV));
                //CJ_YV=MainWindow::CENTERdata.value("PM").value("pm_upervalue").toInt();
                EV_X_OK = true;
            }
            else if(CJ_X == "POWER" && CJ_Y == "DAC")
            {
                //   CJ_XV=MainWindow::CENTERdata.value("PM").value("pm_upervalue").toInt();
                CJ_YV = ST_Y;
                MainWindow::nowMainWindow->syncdata_pm("pm_stm_DAC", QString("%1").arg(CJ_YV));
                EV_Y_OK = true;
            }
            else if(CJ_X == "POWER" && CJ_Y == "POWER")
            {
                //   CJ_XV=MainWindow::CENTERdata.value("PM").value("pm_upervalue").toInt();
                CJ_YV = ST_Y;
                ui->AF_Value_Y->setText(QString("%1").arg(CJ_YV));
                EV_Y_OK = true;
            }
            else
            {
                EV_X_OK = false;
                EV_Y_OK = false;
                ui->AF_Value_X->setText("FFFF");
                ui->AF_Value_Y->setText("FFFF");
                return;
            }
            if(EV_X_OK && EV_Y_OK)
            {
                AF_chuli_xy();
            }
            if((CJ_X != "POWER" && CJ_X != "POWERF"))
            {
                ui->AF_Value_X->setText(QString("%1").arg(CJ_XV));
            }
            if((CJ_Y != "POWER" && CJ_Y != "POWERF"))
            {
                ui->AF_Value_Y->setText(QString("%1").arg(CJ_YV));
            }
            ui->LED_askXY_ok->setStyleSheet("background-color: rgb(0, 0, 0);");
            return;
        }
        //解析下载
        if(  data[0] == 'A'   && data[1] == 'F'
                && data[2] == '_'
                && data[3] == 'D'   && data[4] == 'O'  && data[5] == 'K'
          )//下载中
        {
            AF_DownloadPro++;
            cfsendcon(AF_getProData(AF_DownloadPro));
            return;
        }
        if(  data[0] == 'A'   && data[1] == 'F'   && data[2] == '_'
                && data[3] == 'D'   && data[4] == 'B'  && data[5] == 'D') //下载失败
        {
            showmsg("ERRO:✘Download failed");
            AF_DownLoad_Stop();
            return;
        }
        if(  data[0] == 'A'   && data[1] == 'F'   && data[2] == '_'
                && data[3] == 'D'   && data[4] == 'O'  && data[5] == 'V') //下载终止
        {
            showmsg("ERRO:✔Download successful!");
            AF_DownLoad_Stop();
            return;
        }
    }
}
void curve_fitting::AF_chuli_xy()
{
    if(AF_START)
    {
        if(CJ_IV <= AF_close_Index_stop)
        {
            if(abs(AF_old_Y - CJ_YV) <= AF_close_Var)
            {
                AF_now_OKCount++;
                if(AF_now_OKCount >= AF_close_OKCount)
                {
                    //下一个点
                    on_openloop_save_onedata_clicked();//先记录数据
                    AF_now_OKCount = 0;
                    CJ_VV = CJ_VV + (AF_close_VStop - AF_close_VStart) / AF_close_Datanum;
                    ui->progressBar_AC_CPro->setValue(100 * ((1.0 * (CJ_VV - AF_close_VStart)) / (AF_close_VStop - AF_close_VStart)));
                    if(CJ_VV > AF_close_VStop
                            || (CJ_Y == "POWER" && CJ_X == "ADC" && CJ_YV >= AF_close_MaxPower)
                            || (CJ_Y == "POWERF" && CJ_X == "ADC" && CJ_YV >= AF_close_MaxPower)
                            || (CJ_X == "POWER" && CJ_Y == "DAC" && CJ_XV >= AF_close_MaxPower)
                      )
                    {
                        //下一条线
                        CJ_VV = AF_close_VStart;
                        CJ_IV = CJ_IV + AF_close_Index_setp;
                        ui->progressBar_AC_TPro->setValue(100 * ((1.0 * (CJ_IV - AF_close_Index_start)) / (AF_close_Index_stop - AF_close_Index_start)));
                        if(CJ_IV > AF_close_Index_stop)
                        {
                            //所有曲线采样结束
                            AF_AC_Stop();
                            return;
                        }
                        AF_AC();
                        return;
                    }
                    else
                    {
                        on_horizontalSlider_set_Variable_valueChanged(CJ_VV);
                        return;
                    }
                }
            }
            else
            {
                AF_old_Y = CJ_YV;
                AF_now_OKCount = 0;
            }
            on_AF_askonceXY_clicked();
        }
        else
        {
            //采样已经结束了
            AF_AC_Stop();
        }
    }
}
void curve_fitting:: refpowerdata(int w)//从功率计传过来的
{
    if(CJ_C != "" && CJ_V != ""
            && ((CJ_X == "POWER" || CJ_X == "POWERF")
                || (CJ_Y == "POWER" || CJ_Y == "POWERF")))
    {
        if((CJ_X == "POWER" || CJ_X == "POWERF"))
        {
            ui->AF_Value_X->setText(QString("%1").arg( MainWindow::CENTERdata.value("PM").value("pm_upervalue")));
        }
        if((CJ_Y == "POWER" || CJ_Y == "POWERF"))
        {
            if(CJ_X == "POWER" && CJ_Y == "POWER") {}
            else
                ui->AF_Value_Y->setText(QString("%1").arg( MainWindow::CENTERdata.value("PM").value("pm_upervalue")));
        }
        if(CJ_X == "ADC" && (CJ_Y == "POWER" || CJ_Y == "POWERF"))
        {
            CJ_YV = (uint16_t)MainWindow::CENTERdata.value("PM").value("pm_upervalue").toDouble();
            EV_Y_OK = true;
        }
        else if(CJ_X == "POWER" && CJ_Y == "DAC")
        {
            CJ_XV = (uint16_t)MainWindow::CENTERdata.value("PM").value("pm_upervalue").toDouble();
            EV_X_OK = true;
        }
        else if(CJ_X == "POWER" && CJ_Y == "POWER")
        {
            CJ_XV = (uint16_t)MainWindow::CENTERdata.value("PM").value("pm_upervalue").toDouble();
            EV_X_OK = true;
        }
        else
        {
            EV_X_OK = false;
            EV_Y_OK = false;
            return;
        }
        if(EV_X_OK && EV_Y_OK)
        {
            AF_chuli_xy();
        }
        powerasked = 1; //重新访问 看是否能同步
    }
}

curve_fitting::curve_fitting(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::curve_fitting)
{
    ui->setupUi(this);
    cf = this;
    if(con2stm::cs != NULL)
    {
        QObject::connect(con2stm::cs, SIGNAL(dataget(int, QByteArray)), this, SLOT(stm_acdataget(int, QByteArray)));
        QObject::connect(this, SIGNAL(cfdatasend(int, QByteArray)), con2stm::cs, SLOT(stm_senddata(int, QByteArray)));
    }
    else
    {
        //   QMessageBox::about(NULL, "erro", "pleas open con2stm first！");
    }
    ui->Panel_CPUUsed->installEventFilter(this);
    CF_init();
}

curve_fitting::~curve_fitting()
{
    delete ui;
}
/*
数据测试
*/
void curve_fitting::test_Data_init()
{
    QString test_Projectname = "TEST_1";
    CF_NewProject(test_Projectname);
    QMap<uint16_t, QMap<uint16_t, FinalDatas>> test_linclass;
    QMap<uint16_t, FinalDatas> test_Linedexs;
    QMap<uint16_t, uint16_t> test_KV;
    test_KV.insert(2000, 2000);
    test_KV.insert(3000, 3000);
    test_KV.insert(4000, 4000);
    finalDataTemp.KVS = test_KV;
    finalDataTemp.mVdata = 1500;
    test_Linedexs.insert(3, finalDataTemp);
    test_KV.clear();
    test_KV.insert(200, 2000);
    test_KV.insert(300, 3000);
    test_KV.insert(400, 4000);
    finalDataTemp.KVS = test_KV;
    finalDataTemp.mVdata = 1500;
    test_Linedexs.insert(1, finalDataTemp);
    test_Linedexs.insert(2, finalDataTemp);
    test_linclass.insert(55, test_Linedexs);
    test_linclass.insert(11, test_Linedexs);
    test_linclass.insert(22, test_Linedexs);
    test_linclass.insert(15, test_Linedexs);
    nowCFProject_insert(test_Projectname, test_linclass);
    CF_SaveData(test_Projectname);
}

void curve_fitting::CF_init()
{
    //ONEListwidget 第一步:设定不同部分 0:右键item 可以重名 1 右键item 不能重命名
    ui->list_projects->setDifferent(0);
    ui->list_lineclass->setDifferent(1);
    ui->list_lineindex->setDifferent(1);
    ui->AF_closeloop_Max_Power ->setText(QString("%1").arg(AF_close_MaxPower));
    ui->AF_closeloop_indexStart->setText(QString("%1").arg(AF_close_Index_start));
    ui->AF_closeloop_indexStop->setText    (QString("%1").arg(AF_close_Index_stop));
    ui->AF_closeloop_CutNum->setText       (QString("%1").arg(AF_close_Index_Cutnum));
    ui->AF_closeloop_variableStart->setText(QString("%1").arg(AF_close_VStart));
    ui->AF_closeloop_variableStop->setText (QString("%1").arg(AF_close_VStop));
    ui->AF_closeloop_DataNum->setText      (QString("%1").arg(AF_close_Datanum));
    ui->AF_closeloop_OKCount->setText      (QString("%1").arg(AF_close_OKCount));
    setGetherUI("OpenLoopTest");
    ui->openloop_maxvarible->setText(QString("MaxV:%1").arg(openloop_MaxVariable));
    ui->horizontalSlider_set_Variable->setMinimum(0);
    ui->horizontalSlider_set_Variable->setMaximum(openloop_MaxVariable);
    //ONEListwidget 第二步：连接信号和槽
    QObject::connect(ui->list_projects, SIGNAL(OneListWidget_ListDataChanged(QString)), this, SLOT(oneListWidget_dataChanged(QString)));
    QObject::connect(ui->list_projects, SIGNAL(OneListWidget_DelatLastItem(QString)), this, SLOT(oneListWidget_delate_last(QString)));
    QObject::connect(ui->list_lineclass, SIGNAL(OneListWidget_ListDataChanged(QString)), this, SLOT(oneListWidget_dataChanged(QString)));
    QObject::connect(ui->list_lineclass, SIGNAL(OneListWidget_DelatLastItem(QString)), this, SLOT(oneListWidget_delate_last(QString)));
    QObject::connect(ui->list_lineindex, SIGNAL(OneListWidget_ListDataChanged(QString)), this, SLOT(oneListWidget_dataChanged(QString)));
    QObject::connect(ui->list_lineindex, SIGNAL(OneListWidget_DelatLastItem(QString)), this, SLOT(oneListWidget_delate_last(QString)));
    //连接曲线界面 监听数据变化
    QObject::connect(ui->widget_cuvers, SIGNAL(on_OneCurver_Datahchanged(uint16_t, FinalDatas)), this, SLOT(on_curver_dataChanged(uint16_t, FinalDatas)));
    //初始话工作空间路径
    WorkSpace = QCoreApplication::applicationDirPath() + "\\ONELINEWorkspace";
    //加载文件数据
    CF_Load(WorkSpace);
    ui->widget_disablewithoutlinedown->setVisible(false);
}

int curve_fitting::CF_LoadProject(QString proname)//不需要带.txt的名字
{
    //3加载文件内容
    QFile f(QString(WorkSpace + "\\" + proname + ".txt").replace("/", "\\"));
    if(!f.open(QIODevice::ReadWrite | QIODevice::Unbuffered ))
    {
        return 0;
    }
    QByteArray array = f.readAll();//多查看帮助文档
    putload(proname, array);
    f.close();
    return 1;
}
int curve_fitting::putload(const QString proname, const QByteArray& loaddata)
{
    //loaddata.toUInt()
    // QString lineStr;
    // lineStr=ByteArrayToHexString(loaddata);
    // qDebug()<<lineStr;
    lineClassTemp.clear();
    nowCFProject_insert(proname, lineClassTemp);
    if(!ui->list_projects->contains_text(proname))
    {
        ui->list_projects->addItem(proname);//展示索引
    }
    if(loaddata.length() == 0)
    {
        return 0;
    }
    if(loaddata[0] != 'N')
    {
        showmsg(proname + "读取文件错误 包头不为'N'");
        return 0;
    }
    uint16_t namelen = ((uchar)loaddata[1] << 8) | ((uchar)loaddata[2]);
    if(namelen == 0)
    {
        showmsg(proname + "读取文件错误 保存的工程名长度为0");
        return 0;
    }
    QByteArray pronamedata;
    for(int i = 0; i < namelen; i++)
    {
        pronamedata.append(loaddata[3 + i]);
    }
    QString projectname = pronamedata; //QString(loaddata,3,namelen);
    if(projectname != proname)
    {
        if(proname != "LoadData")
        {
            showmsg(proname + "读取文件错误 存储的工程名：" + projectname + "不等于文件名:" + proname);
            return 0;
        }
        else
        {
            nowloadname = projectname;
        }
    }
    if(loaddata[3 + namelen] != 'A' || loaddata[3 + namelen + 1] != 'S' || loaddata[3 + namelen + 2] != 'P' || loaddata[3 + namelen + 3] != '\n')
    {
        showmsg("ASP错误");
        return 0;
    }
    uint16_t linelen = ((uchar)loaddata[3 + namelen + 4] << 8) | ((uchar)loaddata[3 + namelen + 5]);
    int Beforlen = 3 + namelen + 5;
    int AllIndexLen = linelen * 12;
    int dataaddr_start = AllIndexLen + Beforlen + 1;
    lineClassTemp.clear();
    //finalDataTemp
    uint32_t ALL_datalen = 0;
    for(int i = 0; i < linelen; i++)
    {
        uint16_t tLCID    = ((uchar)loaddata[Beforlen + 1 + i * 12] << 8) | ((uchar) loaddata[Beforlen + 2 + i * 12]);
        uint16_t tINID    = ((uchar)loaddata[Beforlen + 3 + i * 12] << 8) | ((uchar) loaddata[Beforlen + 4 + i * 12]);
        uint16_t tmV_data = ((uchar)loaddata[Beforlen + 5 + i * 12] << 8) | ((uchar) loaddata[Beforlen + 6 + i * 12]);
        uint16_t tdatalen = ((uchar)loaddata[Beforlen + 7 + i * 12] << 8) | ((uchar) loaddata[Beforlen + 8 + i * 12]);
        uint32_t tdataaddr = ((uchar)loaddata[Beforlen + 9 + i * 12]) << 24 | ((uchar)loaddata[Beforlen + 10 + i * 12]) << 16 | ((uchar)loaddata[Beforlen + 11 + i * 12] << 8) | ((uchar)loaddata[Beforlen + 12 + i * 12]);
        clearFinalDatas();
        finalDataTemp.addr = tdataaddr;
        finalDataTemp.mVdata = tmV_data;
        for(int j = 0; j < tdatalen * 4; j += 4) //tdatalen是总数据字节数 一对数据是4个字节
        {
            uint16_t t_Key   = ((uchar)loaddata[dataaddr_start + ALL_datalen + j + 0]) << 8 | ((uchar)loaddata[dataaddr_start + ALL_datalen + j + 1]);
            uint16_t t_Value = ((uchar)loaddata[dataaddr_start + ALL_datalen + j + 2]) << 8 | ((uchar)loaddata[dataaddr_start + ALL_datalen + j + 3]);
            finalDataTemp.KVS.insert(t_Key, t_Value);
        }
        if(lineClassTemp.contains(tLCID))
        {
            LineIndexTemp.clear();
            LineIndexTemp = lineClassTemp.value(tLCID);
            LineIndexTemp.insert(tINID, finalDataTemp);
            lineClassTemp.insert(tLCID, LineIndexTemp);
        }
        else
        {
            LineIndexTemp.clear();
            LineIndexTemp.insert(tINID, finalDataTemp);
            lineClassTemp.insert(tLCID, LineIndexTemp);
        }
        ALL_datalen += tdatalen * 4;
    }
    int ONEICE_len = dataaddr_start + ALL_datalen;
    if(loaddata[ONEICE_len + 0] != 'O')
    {
        showmsg("erro ONEICE freeze O");
        return 0;
    }
    if(loaddata[ONEICE_len + 1] != 'N')
    {
        showmsg("erro ONEICE freeze N");
        return 0;
    }
    if(loaddata[ONEICE_len + 2] != 'E')
    {
        showmsg("erro ONEICE freeze E");
        return 0;
    }
    if(loaddata[ONEICE_len + 3] != 'I')
    {
        showmsg("erro ONEICE freeze I");
        return 0;
    }
    if(loaddata[ONEICE_len + 4] != 'C')
    {
        showmsg("erro ONEICE freeze C");
        return 0;
    }
    if(loaddata[ONEICE_len + 5] != 'E')
    {
        showmsg("erro ONEICE freeze E");
        return 0;
    }
    nowCFProject_insert(proname, lineClassTemp); //如果是对的就再次添加 将数据加进去
    return 1;
}

int curve_fitting::CF_Load(QString ws)
{
    //1遍历文件下的所有文件名
    QDir dir(ws);
    QStringList nameFilters;
    nameFilters << "*.txt";
    QStringList PronameList = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    //2展示到屏幕
    for(QString proname : PronameList)
    {
        if(proname.split(".").length() == 2 && proname.split(".")[1] == "txt")
        {
            CF_LoadProject(proname.split(".")[0]);
        }
    }
    return 1;
}
void curve_fitting:: disable_saveUI()
{
    this->setEnabled(false);
    // showmsg("waiting to be saved...");
}
void  curve_fitting::enable_saveUI()
{
    this->setEnabled(true);
    //showmsg("save OK!");
}
int curve_fitting::CF_SaveData(QString proname)
{
    //保存的时候不能做任何事情
    //保存规则 工程文件发送非曲线拟合改变
    ///组织对应工程的内容 并存储
    if(CFProDatas.contains(proname))
    {
        disable_saveUI();
        QByteArray sfcontext;
        sfcontext.append('N');                             ///N          1             byte
        uint16_t pronamelen;
        if(proname != "LoadData")
        {
            pronamelen = (uint16_t)proname.length();
            sfcontext.append(qbyteArray_u16(pronamelen));             ///namelen     2             byte
            sfcontext.append(proname.toUtf8());               ///poename     pronamelen    byte
        }
        else
        {
            //对加载文件的特殊处理
            pronamelen = (uint16_t)nowloadname.length();
            sfcontext.append(qbyteArray_u16(pronamelen));             ///namelen     2             byte
            sfcontext.append(nowloadname.toUtf8());               ///poename     pronamelen    byte
        }
        sfcontext.append('A')             ;
        sfcontext.append('S')             ;
        sfcontext.append('P')             ;
        sfcontext.append('\n')            ;               ///ASP\n       4             byte
        lineClassTemp = CFProDatas.value(proname); //获取到曲线类层的数据
        //遍历整理内容
        uint16_t sum_lines = 0  ; //总共多少条曲线
        QByteArray Indexbox   ;  //每条曲线对应的数据块索引 索引的 ID Index 均按从小到大排序
        QByteArray combin_Data;  //所有数据按照key从小到大的组合
        uint32_t nowaddr = 0    ; //地址索引的分配与叠加标志位 数据组合完成后 获得总数据区长度
        lineClassTempi = lineClassTemp.begin();
        while (lineClassTempi != lineClassTemp.end() ) //遍历 ID
        {
            uint16_t NCID = lineClassTempi.key();        // ID
            LineIndexTemp = lineClassTempi.value();
            uint16_t NINL =  LineIndexTemp.size();       //曲线条数
            sum_lines += NINL;                           //累计所有的曲线数 ->数据索引块数
            LineIndexTempi = LineIndexTemp.begin();
            while(LineIndexTempi != LineIndexTemp.end()) //遍历Index 每条曲线
            {
                uint16_t NIDX = LineIndexTempi.key();     //Index 索引值
                finalDataTemp = LineIndexTempi.value();
                uint16_t mV_data = finalDataTemp.mVdata;
                uint16_t data_len = finalDataTemp.KVS.size();
                finalDataTemp.addr = nowaddr   ;              //数据首地址
                Indexbox.append(qbyteArray_u16(NCID));        //记录每条曲线的索引数据块
                Indexbox.append(qbyteArray_u16(NIDX));        //记录每条曲线的索引数据块
                Indexbox.append(qbyteArray_u16(mV_data));     //记录每条曲线的索引数据块
                Indexbox.append(qbyteArray_u16(data_len));    //记录每条曲线的索引数据块
                Indexbox.append(qbyteArray_u32(nowaddr));     //记录每条曲线的索引数据块
                nowaddr += data_len * 4                   ; //每个数据集占4个字节
                QMapIterator<uint16_t, uint16_t> datai(finalDataTemp.KVS)           ;
                while(datai.hasNext())
                {
                    datai.next();
                    uint16_t data_key = datai.key(), data_value = datai.value();
                    combin_Data.append(qbyteArray_u16(data_key));      //结合所有数据内容
                    combin_Data.append(qbyteArray_u16(data_value));    //结合所有数据内容
                    QString seedata = (QString("Class:%1,Index:%2,mv:%3,key:%4,v:%5")
                                       .arg(NCID).arg(NIDX).arg(mV_data).arg(data_key).arg(data_value));
                    seedata.clear();
                }
                LineIndexTempi++;
            }
            lineClassTempi++;
        }
        sfcontext.append(qbyteArray_u16(sum_lines));
        sfcontext.append(Indexbox);
        sfcontext.append(combin_Data);
        sfcontext.append(QString("ONEICE").toUtf8());//数据结尾
        CF_Save(WorkSpace, proname, sfcontext) ;
        //计算内存利用率
        CF_LoadProject(proname);
        enable_saveUI();
    }
}

void curve_fitting:: nowCFProject_insert(QString proname,  QMap<uint16_t, QMap<uint16_t, FinalDatas>> data)
{
    CFProDatas.insert(proname, data);
    if(nowCFProject == proname)
    {
        AF_LineFile.setFileName(QString(WorkSpace + "\\" + nowCFProject + ".txt").replace("/", "\\"));
        if(!AF_LineFile.open(QIODevice::ReadOnly | QIODevice::Unbuffered ))
        {
            return;
        }
        cpuused_fname = nowCFProject;
        DownDataSize = AF_LineFile.size();
        if(CUPtype == WS_F103)
        {
            CPU_used = ((DownDataSize * 1.0) / CPU_MS_WSF103) * 100;
        }
        else if(CUPtype == WS_F429)
        {
            CPU_used = ((DownDataSize * 1.0) / CPU_MS_WSF429) * 100;
        }
        AF_LineFile.close();
        update();
    }
}

void curve_fitting::CF_NewProject(const QString proname)
{
    if(!CFProDatas.contains(proname))
    {
        CF_Save(WorkSpace, proname, ""); //创建工程文件
        lineClassTemp.clear();
        nowCFProject_insert(proname, lineClassTemp);
        ui->list_projects->addItem(proname);
    }
}
void curve_fitting:: CF_append_LineClass(int ID)
{
    lineClassTemp = CFProDatas.value(nowCFProject);
    if(!lineClassTemp.contains(ID))
    {
        if(nowCFProject != NULL && nowCFProject != "")
        {
            LineIndexTemp.clear();
            lineClassTemp.insert(ID, LineIndexTemp);
            nowCFProject_insert(nowCFProject, lineClassTemp);
            ui->list_lineclass->addItem(QString("%1").arg(ID));
            showmsg(QString("PRO%1:%2").arg(nowCFProject).arg(ID));
            // CF_SaveData(nowCFProject);添加曲线种类不存
        }
    }
}
void curve_fitting:: CF_NewIndex(int value)
{
    if(nowCFProject != "" && nowCFProject != NULL,
            nowlineClass != "" && nowlineClass != NULL)
    {
        lineClassTemp.clear();
        lineClassTemp = CFProDatas.value(nowCFProject);
        LineIndexTemp.clear();
        LineIndexTemp = lineClassTemp.value(nowlineClass.toInt());
        if(!LineIndexTemp.contains(value))
        {
            clearFinalDatas();
            LineIndexTemp.insert(value, finalDataTemp);
            lineClassTemp.insert(nowlineClass.toInt(), LineIndexTemp);
            nowCFProject_insert(nowCFProject, lineClassTemp);
            ui->list_lineindex->addItem(QString("%1").arg(value));
            showmsg(QString("CLass%1：%2").arg(nowlineClass).arg(value));
            CF_SaveData(nowCFProject);
        }
        else
        {
            showmsg(QString("Index%1 has been existed").arg(value));
        }
    }
}
void curve_fitting::CF_appenLineData(int k, int v)
{
    if(nowCFProject != "" && nowCFProject != NULL,
            nowlineClass != "" && nowlineClass != NULL &&
            nowIndex != "" && nowIndex != NULL)
    {
        if(CFProDatas[nowCFProject][nowlineClass.toInt()].contains(nowIndex.toInt()))
        {
            CFProDatas[nowCFProject][nowlineClass.toInt()][nowIndex.toInt()].KVS.insert(k, v);
            show_LineDatas(nowCFProject, nowlineClass.toInt(), nowIndex.toInt());
        }
    }
}

void curve_fitting::closeEvent(QCloseEvent* event)
{
    if(MainWindow::goto_once_cf == 1)
    {
        MainWindow::goto_once_cf = 0;
        cf = NULL;
        if(con2stm::cs != NULL)
        {
            QObject::disconnect(con2stm::cs, SIGNAL(dataget(int, QByteArray)), this, SLOT(stm_acdataget(int w, QByteArray data)));
            QObject::disconnect(this, SIGNAL(cfdatasend(int, QByteArray)), con2stm::cs, SLOT(stm_senddata(int, QByteArray)));
        }
    }
    exit(0);
}

void curve_fitting::showmsg(QString msg)
{
    QString msgbox    ;
    msgbox.append(msg + "\n");
    msgbox.append(ui->CF_Showmsg->toPlainText());
    ui->CF_Showmsg->setText(msgbox);
}


void curve_fitting::on_CF_Button_NewProject_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("NewProject"), tr("ProjectName:"), QLineEdit::Normal, 0, &ok);
    if (ok && !text.isEmpty())
    {
        if(CFProDatas.contains(text))
        {
            showmsg("failed! Project Name has been used!");
        }
        else if(text == "LoadData")
        {
            showmsg("failed! Project Name==LoadData");
        }
        else if(text.contains("UP_"))
        {
            showmsg("failed! Project Name canot use \"UP_\"");
        }
        else
        {
            CF_NewProject(text);
        }
    }
    else
    {
        showmsg("ProjectName erro");
    }
}

void curve_fitting:: list_clear_project()
{
    disconnect(ui->list_projects, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_projects_currentRowChanged(int)));
    ui->list_projects->clear();
    connect(ui->list_projects, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_projects_currentRowChanged(int)));
}
void curve_fitting:: list_clear_class()
{
    disconnect(ui->list_lineclass, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_lineclass_currentRowChanged(int)));
    ui->list_lineclass->clear();
    connect(ui->list_lineclass, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_lineclass_currentRowChanged(int)));
}
void curve_fitting:: list_clear_index()
{
    disconnect(ui->list_lineindex, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_lineindex_currentRowChanged(int)));
    ui->list_lineindex->clear();
    connect(ui->list_lineindex, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_lineindex_currentRowChanged(int)));
}

void curve_fitting:: show_LineClass(QString Proname)
{
    //先清除之后的子项
    ui->linedatas->clear();
    nowlineClass = "";
    nowIndex = "";
    list_clear_index();
    list_clear_class();
    if(!CFProDatas.contains(Proname))
    {
        showmsg("no data about" + Proname);
        return;
    }
    lineClassTemp.clear();
    lineClassTemp = CFProDatas.value(Proname);
    lineClassTempi = lineClassTemp.begin();
    while (lineClassTempi != lineClassTemp.end() ) //遍历 ID
    {
        ui->list_lineclass->addItem(QString("%1").arg(lineClassTempi.key()));
        lineClassTempi++;
    }
}
void curve_fitting:: show_LineIndex(QString Proname, uint16_t classID)
{
    //展示数据
    ui->linedatas->clear();
    nowIndex = "";
    list_clear_index();
    if(!CFProDatas.contains(Proname))
    {
        showmsg("no data about" + Proname);
        return;
    }
    lineClassTemp.clear();
    lineClassTemp = CFProDatas.value(Proname);
    if(!lineClassTemp.contains(classID))
    {
        showmsg("no data about" + classID);
        return;
    };
    LineIndexTemp.clear();
    LineIndexTemp = lineClassTemp.value(classID);
    LineIndexTempi = LineIndexTemp.begin();
    while (LineIndexTempi != LineIndexTemp.end()) //遍历 ID
    {
        ui->list_lineindex->addItem(QString("%1").arg(LineIndexTempi.key()));
        LineIndexTempi++;
    }
    //将整定属性填充
    QString LC_BasePath = QCoreApplication::applicationDirPath();
    QString LC_filename = "LCDATA.txt";
    LC_BasePath.append("\\LC_Data");
    QString path = LC_BasePath;
    QString name = LC_filename;
    QMap<QString, int >ClassID;
    QFile f(QString(path + "\\" + name).replace("/", "\\"));
    if(f.open(QIODevice::ReadWrite  | QIODevice::Text))
    {
        QTextStream txtInput(&f);
        QString lineStr;
        while(!txtInput.atEnd())
        {
            QString line = txtInput.readLine();
            lineStr = QString(line.toLocal8Bit());
            if(line.split(",").length() == 5)
            {
                QString rindex = line.split(",")[1],
                        rvarible = line.split(",")[2],
                        rinput = line.split(",")[3],
                        routput = line.split(",")[4];
                QString temp = QString("%1,%2,%3,%4").arg(rindex).arg(rvarible).arg(rinput).arg(routput);
                if(!ClassID.contains(temp))
                {
                    ClassID.insert(temp, QString(line.split(",")[0]).toInt());
                }
                else
                {
                    qDebug() << "load reCover erro" << line;
                }
            }
            else
            {
                qDebug() << "load Line erro" << line;
            }
            qDebug() << lineStr;
        }
        f.close();
    }
    QMapIterator<QString, int>  cm(ClassID);
    while(cm.hasNext())
    {
        cm.next();
        if(cm.value() == classID)
        {
            QStringList temp = cm.key().split(",");
            if(temp.length() == 4)
            {
                CJ_C = cm.key();
                CJ_I = temp[0];
                CJ_V = temp[1];
                CJ_X = temp[2];
                CJ_Y = temp[3];
                QByteArray sd;
                sd.append("AF_CLASS");
                //索引
                if(CJ_I == "FREQ")
                {
                    sd.append(AF_I_FREQ);
                }
                //自变量
                if(CJ_V == "DAC")
                {
                    sd.append(AF_V_DAC);
                    openloop_MaxVariable = 4095;
                    ui->openloop_maxvarible->setText(QString("MaxV:%1").arg(openloop_MaxVariable));
                    ui->horizontalSlider_set_Variable->setMinimum(0);
                    ui->horizontalSlider_set_Variable->setMaximum(openloop_MaxVariable);
                }
                else if(CJ_V == "TIME")
                {
                    sd.append(AF_V_TIME);
                }
                else if(CJ_V == "PSdBm")
                {
                    sd.append(AF_V_PSdBm);
                    openloop_MaxVariable = 0;
                    ui->openloop_maxvarible->setText(QString("MaxV:%1").arg(openloop_MaxVariable));
                    ui->horizontalSlider_set_Variable->setMinimum(-30);
                    ui->horizontalSlider_set_Variable->setMaximum(openloop_MaxVariable);
                }
                else
                {
                    showmsg("Varible erro");
                }
                //因变量X
                if(CJ_X == "ADC")
                {
                    sd.append(AF_X_ADC);
                }
                else if(CJ_X == "POWER")
                {
                    sd.append(AF_X_POWER);
                }
                //因变量Y
                if(CJ_Y == "POWER")
                {
                    sd.append(AF_Y_POWER);
                }
                else if(CJ_Y == "POWERF")
                {
                    sd.append(AF_Y_POWERF);
                }
                else if(CJ_Y == "DAC")
                {
                    sd.append(AF_Y_DAC);
                }
                sd.append("SET")     ;
                cfsendcon(sd);
                ui->LED_lineclassok->setStyleSheet("background-color: rgb(0, 0, 0);");
                //ui->AF_nowIndex->setText("I:"+CJ_I);
                //ui->AF_nowVariable->setText("V:"+CJ_V);
                //ui->AF_nowX->setText("X:"+CJ_X);
                //ui->AF_nowY->setText("Y:"+CJ_Y);
                //ui->AF_LineClass->setText("LineClass:"+CJ_C);
            }
        }
    }
    if(CJ_C != "" && CJ_V != ""
            && (CJ_X == "POWER" || CJ_X == "POWERF")
            && (CJ_Y == "POWER" || CJ_Y == "POWERF"))
    {
        powerasked = 1;
    }
}
void curve_fitting:: show_LineDatas(QString Proname, uint16_t classID, uint16_t indexID)
{
    //展示数据
    ui->linedatas->clear();
    if(!CFProDatas.contains(Proname))
    {
        showmsg("no data about" + Proname);
        return;
    }
    lineClassTemp.clear();
    lineClassTemp = CFProDatas.value(Proname);
    if(!lineClassTemp.contains(classID))
    {
        showmsg("no data about" + classID);
        return;
    }
    LineIndexTemp.clear();
    LineIndexTemp = lineClassTemp.value(classID);
    if(!LineIndexTemp.contains(indexID))
    {
        showmsg("no data about" + indexID);
        return;
    }
    QString showdata;
    showdata.append("Proname:" + Proname + "\n");
    showdata.append(QString("LineCla:%1\n").arg(classID));
    showdata.append(QString("LineInd:%1\n").arg(indexID));
    finalDataTemp = LineIndexTemp.value(indexID);
    uint16_t tmV_data = finalDataTemp.mVdata;
    uint32_t taddr   = finalDataTemp.addr;
    showdata.append(QString("mV_data:%1\n").arg(tmV_data));
    showdata.append(QString("DataAdr:%1\n").arg(taddr));
    QMapIterator<uint16_t, uint16_t> di(finalDataTemp.KVS);
    while(di.hasNext())
    {
        di.next();
        showdata.append(QString("%1\t%2\n").arg(di.key()).arg(di.value()));
    }
    ui->linedatas->setText(showdata);
    ui->widget_cuvers->drawOnePoint(indexID, finalDataTemp);
    //将数据值下发到单片机
    CJ_IV = indexID;
}


void curve_fitting::on_list_projects_currentRowChanged(int currentRow)
{
    int size = ui->list_projects->count();
    if(ui->list_projects->item(currentRow)->text() != NULL
            && ui->list_projects->item(currentRow)->text() != "")
    {
        QString choiced = ui->list_projects->item(currentRow)->text();
        if(nowCFProject != choiced)
        {
            nowCFProject = choiced;
            showmsg(QString("PRO:%1,%2").arg(currentRow).arg(nowCFProject));
            ui->choiced_project->setText("project:" + nowCFProject);
            show_LineClass(nowCFProject);
            cpuused_fname = nowCFProject;
            AF_LineFile.setFileName(QString(WorkSpace + "\\" + nowCFProject + ".txt").replace("/", "\\"));
            if(!AF_LineFile.open(QIODevice::ReadOnly | QIODevice::Unbuffered ))
            {
                return;
            }
            cpuused_fname = nowCFProject;
            DownDataSize = AF_LineFile.size();
            if(CUPtype == WS_F103)
            {
                CPU_used = ((DownDataSize * 1.0) / CPU_MS_WSF103) * 100;
            }
            else if(CUPtype == WS_F429)
            {
                CPU_used = ((DownDataSize * 1.0) / CPU_MS_WSF429) * 100;
            }
            AF_LineFile.close();
            update();
        }
    }
}
void curve_fitting::on_list_lineclass_currentRowChanged(int currentRow)
{
    int size = ui->list_lineclass->count();
    if(ui->list_lineclass->item(currentRow)->text() != NULL
            && ui->list_lineclass->item(currentRow)->text() != "")
    {
        QString choiced = ui->list_lineclass->item(currentRow)->text();
        if(nowlineClass != choiced)
        {
            nowlineClass = choiced;
            showmsg(QString("CLA:%1").arg(currentRow));
            nowlineClass = ui->list_lineclass->item(currentRow)->text();
            ui->choiced_lineclass->setText("LineClass:" + nowlineClass);
            show_LineIndex(nowCFProject, nowlineClass.toInt());
        }
    }
}

void curve_fitting::on_list_lineindex_currentRowChanged(int currentRow)
{
    if(ui->list_lineindex->item(currentRow)->text() != NULL
            && ui->list_lineindex->item(currentRow)->text() != "")
    {
        QString choiced = ui->list_lineindex->item(currentRow)->text();
        if(nowIndex != choiced)
        {
            showmsg(QString("IND:%1").arg(currentRow));
            nowIndex = choiced;
            ui->choiced_lineindex->setText("LineIndex:" + nowIndex);
            show_LineDatas(nowCFProject, nowlineClass.toInt(), nowIndex.toInt());
            //MainWindow::nowMainWindow->pm_setfreq(nowIndex.toDouble(), MainWindow::nowMainWindow->returnchmsg(MainWindow::nowchanal));
        }
    }
}

void curve_fitting::on_CF_Button_NewLineClass_clicked()
{
    if(nowCFProject != NULL && nowCFProject != "")
    {
        CF_LineClassDialog* lineclassDialog = new CF_LineClassDialog(this);
        QObject::connect(lineclassDialog, SIGNAL(LC_append(QString)), this, SLOT(LC_append(QString)));
        lineclassDialog->exec();
        lineclassDialog->close();
        QObject::disconnect(lineclassDialog, SIGNAL(LC_append(QString)), this, SLOT(LC_append(QString)));
    }
    else
    {
        showmsg("please Choice Project First");
    }
}
//ONEListwidget 第三步：处理数据在删除最后一项时的槽信号
void curve_fitting::oneListWidget_delate_last(QString name)
{
    //删除最后一项
    if(name == "list_projects")
    {
        disconnect(ui->list_projects, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_projects_currentRowChanged(int)));
        QListWidgetItem* item = ui->list_projects->currentItem();
        ui->list_projects->removeItemWidget(item);
        delete item;
        nowCFProject = "";
        connect(ui->list_projects, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_projects_currentRowChanged(int)));
        oneListWidget_dataChanged(name);
    }
    if(name == "list_lineclass")
    {
        disconnect(ui->list_lineclass, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_lineclass_currentRowChanged(int)));
        QListWidgetItem* item = ui->list_lineclass->currentItem();
        ui->list_lineclass->removeItemWidget(item);
        delete item;
        nowlineClass = "";
        connect(ui->list_lineclass, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_lineclass_currentRowChanged(int)));
        oneListWidget_dataChanged(name);
    }
    if(name == "list_lineindex")
    {
        disconnect(ui->list_lineindex, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_lineindex_currentRowChanged(int)));
        QListWidgetItem* item = ui->list_lineindex->currentItem();
        ui->list_lineindex->removeItemWidget(item);
        delete item;
        nowIndex = "";
        connect(ui->list_lineindex, SIGNAL(currentRowChanged(int)), this, SLOT(on_list_lineindex_currentRowChanged(int)));
        oneListWidget_dataChanged(name);
    }
}
//ONEListwidget 第四步：处理数据因为操作变化的槽信号
void curve_fitting::oneListWidget_dataChanged(QString name)
{
    if(name == "list_projects")
    {
        setCFProjects();
    }
    if(name == "list_lineclass")
    {
        setCFLineClass();
    }
    if(name == "list_lineindex")
    {
        setCFLineIndex();
    }
}

void curve_fitting::setCFProjects()
{
    if(ui->list_projects->count() == CFProDatas.size()) //重命名了
    {
        CFProDatasi = CFProDatas.begin();
        while(CFProDatasi != CFProDatas.end())
        {
            QString cpdc = CFProDatasi.key();
            if(cpdc != "" && !ui->list_projects->contains_text(cpdc))
            {
                lineClassTemp = CFProDatas.value(cpdc);
                QString currentString = ui->list_projects->currentItem()->text();
                nowCFProject_insert(currentString, lineClassTemp);
                CFProDatas.remove(cpdc);
                CF_ReName(WorkSpace, cpdc, currentString);
                on_list_projects_currentRowChanged(ui->list_projects->currentRow());
                CF_SaveData(currentString);
                break;
            }
            CFProDatasi++;
        }
    }
}
void curve_fitting:: setCFLineClass()
{
    //只支持删除了
    if(nowCFProject != "" && nowCFProject != NULL)
    {
        lineClassTemp = CFProDatas.value(nowCFProject);
        lineClassTempi = lineClassTemp.begin();
        while(lineClassTempi != lineClassTemp.end())
        {
            uint16_t icpdc = lineClassTempi.key();
            QString cpdc = QString("%1").arg(icpdc);
            if(cpdc != "" && !ui->list_lineclass->contains_text(cpdc))
            {
                ui->linedatas->clear();
                list_clear_index();
                lineClassTemp.remove(icpdc);
                nowCFProject_insert(nowCFProject, lineClassTemp);
                CF_SaveData(nowCFProject);
                break;
            }
            lineClassTempi++;
        }
    }
}
void curve_fitting:: setCFLineIndex()
{
    //只支持删除了
    if(nowCFProject != "" && nowCFProject != NULL)
    {
        lineClassTemp = CFProDatas.value(nowCFProject);
        if(nowlineClass != "" && nowlineClass != NULL && lineClassTemp.contains(nowlineClass.toInt()))
        {
            LineIndexTemp = lineClassTemp.value(nowlineClass.toInt());
            LineIndexTempi = LineIndexTemp.begin();
            while(LineIndexTempi != LineIndexTemp.end())
            {
                uint16_t icpdc = LineIndexTempi.key();
                QString cpdc = QString("%1").arg(icpdc);
                if(cpdc != "" && !ui->list_lineindex->contains_text(cpdc))
                {
                    ui->linedatas->clear();
                    LineIndexTemp.remove(icpdc);
                    lineClassTemp.insert(nowlineClass.toInt(), LineIndexTemp);
                    nowCFProject_insert(nowCFProject, lineClassTemp);
                    CF_SaveData(nowCFProject);
                    break;
                }
                LineIndexTempi++;
            }
        }
    }
}


void curve_fitting:: LC_append(QString append)//从窗口那边过来的
{
    if(nowCFProject != NULL && nowCFProject != "" && append != NULL && append.split(",").length() == 5)
    {
        int ID = QString(append.split(",")[0]).toInt();
        CF_append_LineClass(ID);
    }
    else if(nowCFProject == NULL || nowCFProject == "")
    {
        showmsg("please Choice Project First");
    }
}

//void curve_fitting::on_list_projects_itemChanged(QListWidgetItem *item)
//{
//      showmsg("asdadasdsad");
//}

void curve_fitting::on_CF_Button_NewLineIndex_clicked()
{
    if(nowCFProject != "" && nowCFProject != NULL,
            nowlineClass != "" && nowlineClass != NULL)
    {
        bool ok;
        int v = QInputDialog::getInt(this, tr("New Index"),
                                     tr("value:"), NULL, 0, 65536, 1, &ok);
        if (ok)
        {
            CF_NewIndex(v);
        }
        else
        {
            showmsg("Input Index erro");
        }
    }
}

void curve_fitting::on_CF_SaveDatas_clicked()//保存当前选中工程的所有数据
{
    if(nowCFProject != "" && nowCFProject != NULL)
    {
        CF_SaveData(nowCFProject);
    }
}

void curve_fitting::on_showmsg_clear_clicked()
{
    ui->CF_Showmsg->clear();
}
void curve_fitting::analysis_CF_Context(QString proname, uint16_t lineclass, QString context) //兼容同类多解析
{
    int DataIterator = 0, fi = 0, oneceok = 0;
    uint16_t templineclass = 0;
    uint16_t templineindex = 0;
    uint16_t tempmV_data = 0;
    QMap<uint16_t, uint16_t> temp_KV;
    disable_saveUI();
    QStringList textlist = context.split("\n");
    //初校验
    if(textlist.length() < 4)
    {
        showmsg("Format error 0");
        enable_saveUI();
        return;
    }
    QString temptxt;
    temptxt = textlist[0];
    if(!CFProDatas.contains(proname))
    {
        showmsg(QString("Format error no Project about %1").arg(proname));
        enable_saveUI();
        return;
    }
    if(!temptxt.contains("Proname:"))
    {
        showmsg("Format error 1");
        enable_saveUI();
        return;
    }
    temptxt.replace("Proname:", "");
    if(temptxt != proname)
    {
        showmsg("Format error 3");
        enable_saveUI();
        return;
    }
    lineClassTemp = CFProDatas.value(proname);
    if(!lineClassTemp.contains(lineclass))
    {
        showmsg(QString("Format error no lineclass about %1").arg(lineclass));
        enable_saveUI();
        return;
    }
    LineIndexTemp = lineClassTemp.value(lineclass);
    if(textlist.last() == "")
    {
        textlist.removeLast();
    }
    //解码校验 成功结束条件 下一次的开始 以及到数据结尾
    for(int i = 0; i < textlist.length(); i++)
    {
        temptxt = textlist[i];
        if(temptxt.contains("Proname:"))
        {
            temptxt.replace("Proname:", "");
            if(temptxt == proname) //解析开始
            {
                if(oneceok == 1) //结束上一帧数据解析
                {
                    oneceok = 0;
                    clearFinalDatas();
                    finalDataTemp.mVdata = tempmV_data;
                    finalDataTemp.KVS = temp_KV;
                    LineIndexTemp.insert(templineindex, finalDataTemp);
                }
                fi = i;
                DataIterator = 1;
                templineclass = 0;
                templineindex = 0;
                tempmV_data = 0;
                temp_KV.clear();
            }
            else
            {
                showmsg("Format error 4");
                enable_saveUI();
                return;
            }
        }
        else if(DataIterator == 1)
        {
            temptxt = textlist[i];
            switch(i - fi)
            {
                case 0:
                    break;
                case 1:
                    if(!temptxt.contains("LineCla:"))
                    {
                        showmsg("Format error C");
                        enable_saveUI();
                        return;
                    }
                    else
                    {
                        temptxt.replace("LineCla:", "");
                        if(temptxt == "")
                        {
                            showmsg("Format error C1");
                            enable_saveUI();
                            return;
                        }
                        else
                        {
                            templineclass = temptxt.toInt();
                            if(templineclass != lineclass)
                            {
                                showmsg(QString("lineclass tID %1 != %2").arg(templineclass).arg(lineclass));
                                enable_saveUI();
                                return;
                            }
                        }
                    }
                    break;
                case 2:
                    if(!temptxt.contains("LineInd:"))
                    {
                        showmsg("Format error I");
                        enable_saveUI();
                        return;
                    }
                    else
                    {
                        temptxt.replace("LineInd:", "");
                        if(temptxt == "")
                        {
                            showmsg("Format error I1");
                            enable_saveUI();
                            return;
                        }
                        else
                        {
                            templineindex = temptxt.toInt();
                        }
                    }
                    break;
                case 3:
                    if(!temptxt.contains("mV_data:"))
                    {
                        showmsg("Format error M");
                        enable_saveUI();
                        return;
                    }
                    else
                    {
                        temptxt.replace("mV_data:", "");
                        if(temptxt == "")
                        {
                            showmsg("Format error M1");
                            enable_saveUI();
                            return;
                        }
                        else
                        {
                            tempmV_data = temptxt.toInt();
                        }
                    }
                    break;
                case 4:
                    if(!temptxt.contains("DataAdr:"))
                    {
                        showmsg("Format error AD");
                        enable_saveUI();
                        return;
                    }
                    oneceok = 1; //绝对的存在一条数据了
                    break;
                default:  //其余的是数据
                    if(!temptxt.contains("\t"))
                    {
                        showmsg("Format error DATA");
                        enable_saveUI();
                        return;
                    }
                    if(temptxt.split("\t").length() != 2)
                    {
                        showmsg("Format error DATAL");
                        enable_saveUI();
                        return;
                    }
                    QString tk = temptxt.split("\t")[0],
                            tv = temptxt.split("\t")[1];
                    temp_KV.insert((uint16_t)tk.toInt(), (uint16_t)tv.toInt());
                    break;
            }
        }
    }
    //数据解析完成后
    if(oneceok == 1) //结束上一帧数据解析
    {
        oneceok = 0;
        clearFinalDatas();
        finalDataTemp.mVdata = tempmV_data;
        finalDataTemp.KVS = temp_KV;
        LineIndexTemp.insert(templineindex, finalDataTemp);
    }
    else
    {
        showmsg("Final Erro!");
        enable_saveUI();
        return;
    }
    lineClassTemp.insert(lineclass, LineIndexTemp);
    nowCFProject_insert(proname, lineClassTemp);
    CF_SaveData(proname);
    enable_saveUI();
}
void curve_fitting::on_irrigation_clicked()
{
    if(nowCFProject != "" && nowCFProject != NULL
            && nowlineClass != "" && nowlineClass != NULL
            && nowIndex != "" && nowIndex != NULL)
    {
        QMessageBox customMsgBox;
        customMsgBox.setWindowTitle(QString("irrigation datas to %1,%2,%3?").arg(nowCFProject).arg(nowlineClass).arg(nowIndex));
        QPushButton* lockButton = customMsgBox.addButton("YES", QMessageBox::ActionRole);
        QPushButton* cancelButton = customMsgBox.addButton(QMessageBox::Cancel);
        customMsgBox.exec();
        if (customMsgBox.clickedButton() == lockButton)
        {
            //解析文本空间的文本
            QString editContext = ui->linedatas->toPlainText();
            analysis_CF_Context(nowCFProject, (uint16_t)nowlineClass.toInt(), editContext);
        }
    }
}
//曲线操作导致数据变化
void curve_fitting::on_curver_dataChanged(uint16_t index, FinalDatas ft)
{
    if(index == nowIndex.toInt())
    {
        CFProDatas[nowCFProject][nowlineClass.toInt()][index] = ft;
        show_LineDatas(nowCFProject, nowlineClass.toInt(), nowIndex.toInt());
    }
}
void curve_fitting::on_CureWindow_currentChanged(int index)
{
    //0 ALL 1 single
    curvShowMode = index;
}

void curve_fitting::setGetherUI(QString gtui)
{
    if(gtui == "OpenLoopTest")
    {
        if(!close_Started)
        {
            ui->widget_openloop->setVisible(true);
            ui->widget_closeloop->setVisible(false);
        }
        else
        {
            ui->AF_Choice_Mode->setCurrentText("AutoFitting");
            showmsg("pleace stop AutoFitting first! ");
        }
    }
    if(gtui == "AutoFitting")
    {
        ui->widget_openloop->setVisible(false);
        ui->widget_closeloop->setVisible(true);
    }
}

void curve_fitting::on_AF_Choice_Mode_currentIndexChanged(const QString& arg1)
{
    if(arg1 == "OpenLoopTest")
    {
        setGetherUI("OpenLoopTest");
    }
    else if(arg1 == "AutoFitting")
    {
        setGetherUI("AutoFitting");
    }
}

void curve_fitting::on_openloop_set_maxvarible_returnPressed()
{
    if(ui->openloop_set_maxvarible->text().toInt() > 10)
    {
        openloop_MaxVariable = ui->openloop_set_maxvarible->text().toInt();
        ui->openloop_maxvarible->setText(QString("MaxV:%1").arg(openloop_MaxVariable));
        ui->horizontalSlider_set_Variable->setMaximum(openloop_MaxVariable);
    }
}

void curve_fitting::on_horizontalSlider_set_Variable_valueChanged(int value)
{
    if(openloop_PowerON && openloop_RFON)
    {
        //发送自变量数据到目标机器
        if(CJ_C != "" && CJ_I != "" && CJ_V != "" && CJ_X != "" && CJ_Y != "")
        {
            if(nowIndex.toInt() == CJ_IV)
            {
                showmsg(QString("sendIV: %1 %2").arg(CJ_IV).arg(value));
                ui->LED_linevariable_ok->setStyleSheet("background-color: rgb(0, 0, 0);");
                ui->LED_lineindex_ok->setStyleSheet("background-color: rgb(0, 0, 0);");
                QByteArray sd;
                sd.append("AF_IV");
                // QByteArray data;
                sd.append((CJ_IV >> 8) & 0xff);
                sd.append((CJ_IV) & 0xff);
                sd.append((value >> 8) & 0xff);
                sd.append((value) & 0xff);
                //sd.append(ByteArrayToHexString(data));
                sd.append("VALUE");
                cfsendcon(sd);
            }
        }
    }
}

void curve_fitting::on_openloop_set_variable_returnPressed()
{
    if(ui->openloop_set_variable->text().toInt() < openloop_MaxVariable)
    {
        ui->horizontalSlider_set_Variable->setValue(ui->openloop_set_variable->text().toInt());
    }
}

void curve_fitting::on_openloop_set_variable_sub_clicked()
{
    if(ui->openloop_set_variable_step->text() != "")
    {
        int step = ui->openloop_set_variable_step->text().toInt();
        if(step > 0 && step < openloop_MaxVariable)
        {
            int v = ui->horizontalSlider_set_Variable->value();
            if(v < step)
            {
                v = 0;
            }
            else
            {
                v = v - step;
            }
            ui->horizontalSlider_set_Variable->setValue(v);
        }
    }
}

void curve_fitting::on_openloop_set_variable_add_clicked()
{
    if(ui->openloop_set_variable_step->text() != "")
    {
        int step = ui->openloop_set_variable_step->text().toInt();
        if(step > 0 && step < openloop_MaxVariable)
        {
            int v = ui->horizontalSlider_set_Variable->value();
            if(v + step > openloop_MaxVariable)
            {
                v = openloop_MaxVariable;
            }
            else
            {
                v = v + step;
            }
            ui->horizontalSlider_set_Variable->setValue(v);
        }
    }
}

void curve_fitting::on_openloop_save_onedata_clicked()
{
    if(CJ_C != "" && CJ_I != "" && CJ_V != "" && CJ_X != "" && CJ_Y != "")
    {
        CF_appenLineData(CJ_XV, CJ_YV);
        if(inlinerecord)
        {
            datarecord_base::nowdatarecord_base->recorddata_one();
        }
    }
}

void curve_fitting::on_AF_OpenLoop_POWERSW_clicked()
{
    if(CJ_C != "" && CJ_I != "" && CJ_V != "" && CJ_X != "" && CJ_Y != "")
    {
        ui->horizontalSlider_set_Variable->setValue(0);
        if(ui->AF_OpenLoop_POWERSW->text() == "POWERON")
        {
            cfsendcon("AF_POWER_ON1");
        }
        else
        {
            cfsendcon("AF_POWER_ON0");//发送电源开等待返回结果
        }
    }
}

void curve_fitting::on_AF_OpenLoop_RFSW_clicked()
{
    if(CJ_C != "" && CJ_I != "" && CJ_V != "" && CJ_X != "" && CJ_Y != "")
    {
        ui->horizontalSlider_set_Variable->setValue(0);
        if(ui->AF_OpenLoop_RFSW->text() == "RFON")
        {
            cfsendcon("AF_RF_ON1");
        }
        else
        {
            cfsendcon("AF_RF_ON0");//发送电源开等待返回结果
        }
    }
}

void curve_fitting::on_AF_askonceXY_clicked()
{
    if(openloop_PowerON && openloop_RFON)
    {
        //发送自变量数据到目标机器
        if(CJ_C != "" && CJ_I != "" && CJ_V != "" && CJ_X != "" && CJ_Y != "")
        {
            cfsendcon("AF_XY");
            ui->LED_askXY_ok->setStyleSheet("background-color: rgb(0, 0, 0);");
        }
    }
}

void curve_fitting::on_AF_STLineUPLoad_clicked()
{
    ui->progressBar_AFUpload->setValue(0);
    AF_updataFile.setFileName(QString(WorkSpace + "\\" + "LoadData" + ".txt").replace("/", "\\"));
    if(AF_updataFile.isOpen())
    {
        AF_updataFile.close();
    }
    AF_UPDdataTlen = 0;
    QDir dir;
    if(!dir.exists(WorkSpace))
    {
        dir.mkdir(WorkSpace);
    }
    if(!AF_updataFile.open(QIODevice::WriteOnly))//不存在会自动创建 追加写入
    {
        if(!AF_updataFile.open(QIODevice::WriteOnly ))
        {
            showmsg("加载文件打开失败");
            return;
        }
    }
    AF_updataFile.write("");
    AF_updataFile.close();
    if(!AF_updataFile.open(QIODevice::Append))
    {
        showmsg("加载文件追加打开失败");
        return;
    }
    if(con2stm:: UDP_CON == 1 || con2stm::TCP_CON == 1 || con2stm::USART_CON == 1)
    {
        cfsendcon("AF_UPLOAD"); //数据上载
    }
    else
    {
        MainWindow::nowMainWindow->callcon2stm();
    }
}



QByteArray curve_fitting::AF_getProData(int pro)
{
    if(pro == 0)
    {
        QByteArray prodata;
        prodata.append("AF_DOWNLOAD_START");
        AF_flen = 0;
        return prodata;
    }
    else if(!AF_Downloaddatastream->atEnd())
    {
        char arry[AF_DownEachSize];
        uint16_t a = AF_Downloaddatastream->readRawData(arry, AF_DownEachSize);
        AF_flen = AF_flen + a;
        ui->progressBar_AFDownload->setValue(100 * ((AF_flen * 1.0) / LineFileDataSize));
        QByteArray Ddatas;
        Ddatas.append('A');
        Ddatas.append('F');
        Ddatas.append(a >> 8);
        Ddatas.append(a & 0xff);
        for(int i = 0; i < a; i++)
        {
            Ddatas.append(arry[i]);
        }
        Ddatas.append('D');
        return Ddatas;
    }
    else
    {
        if(AF_flen == LineFileDataSize) {}
        //包尾
        QByteArray Ddatas;
        Ddatas.append('A');
        Ddatas.append('F');
        Ddatas.append(AF_flen >> 24);
        Ddatas.append(AF_flen >> 16);
        Ddatas.append(AF_flen >> 8);
        Ddatas.append(AF_flen >> 0);
        Ddatas.append('O');
        Ddatas.append('D');
        Ddatas.append('S');
        Ddatas.append('T');
        Ddatas.append('O');
        Ddatas.append('P');
        showmsg(QString("DownLoad SEND OVER!"));
        AF_LineFile.close();
        return Ddatas;
    }
}

void curve_fitting::on_AF_STLineDownLoad_clicked()
{
    //获取文件
    int result_choice = 0;
    if(nowCFProject != NULL)
    {
        QMessageBox mcwindow(this);              //创建对话框
        mcwindow.setWindowTitle("Download Data");
        mcwindow.setText(QString("Do you want Download %1").arg(nowCFProject));
        mcwindow.setIcon(QMessageBox::Warning);  //设置对话框的图标
        //设置对话框的按钮
        mcwindow.setStandardButtons( QMessageBox::YesToAll | QMessageBox::Cancel | QMessageBox::Ok );
        mcwindow.setButtonText(QMessageBox::YesToAll, "Yes");
        mcwindow.setButtonText(QMessageBox::Ok, "another");
        mcwindow.setButtonText(QMessageBox::Cancel, "exit");
        switch(mcwindow.exec())
        {
            case QMessageBox::Ok:
                result_choice = 1;
                break;
            case QMessageBox::YesToAll :
                result_choice = 2;
                break;
            case QMessageBox::Cancel :
                result_choice = 3;
                return;
        }
    }
    QString f;
    if(result_choice == 1 || result_choice == 0)//没有选中的工程
    {
        f = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("main bin file"), qApp->applicationDirPath(),
                                         QString::fromLocal8Bit("bin File(*.txt)"));//新建文件打开窗口
        cpuused_fname = f;
    }
    else if(result_choice == 2)
    {
        f = QString(WorkSpace + "\\" + nowCFProject + ".txt").replace("/", "\\");
        cpuused_fname = nowCFProject;
    }
    else
    {
        return;
    }
    AF_LineFile. setFileName(f);
//    if(!AF_LineFile.open(QIODevice::ReadOnly|QIODevice::Unbuffered ))
//    {
//        return;
//    }
    DownDataSize = AF_LineFile.size();
    if(CUPtype == WS_F103)
    {
        CPU_used = ((DownDataSize * 1.0) / CPU_MS_WSF103) * 100;
    }
    else if(CUPtype == WS_F429)
    {
        CPU_used = ((DownDataSize * 1.0) / CPU_MS_WSF429) * 100;
    }
    update();
    if(CPU_used > 92)
    {
        QMessageBox::about(NULL, "erro", "CPU Used >92% cannot download please modify Data!");
        return;
    }
    if(con2stm:: UDP_CON == 1 || con2stm::TCP_CON == 1 || con2stm::USART_CON == 1)
    {
        if(AF_LineFile.open(QIODevice::ReadOnly | QIODevice::Unbuffered))
        {
            LineFileDataSize = AF_LineFile.size();
            if(LineFileDataSize > 66)
            {
                AF_Downloaddatastream = new QDataStream(&AF_LineFile) ;
                showmsg(QString(": %1 \r\n SIZE: %2 ").arg(f).arg(LineFileDataSize));
                AF_DownloadPro = 0;
                cfsendcon(AF_getProData(AF_DownloadPro));
            }
            else
            {
                showmsg(QString("APP LEN ERR SIZE<66:%1").arg(LineFileDataSize));
            }
        }
    }
    else
    {
        MainWindow::nowMainWindow->callcon2stm();
    }
}
void curve_fitting:: AF_DownLoad_Stop()
{
    AF_DownloadPro = 0;
    ui->progressBar_AFDownload->setValue(0);
    if(AF_LineFile.isOpen())
    {
        AF_LineFile.close();
    }
}



void curve_fitting::on_AF_closeloop_indexStart_set_returnPressed()
{
    if(ui->AF_closeloop_indexStart_set->text() != "")
    {
        AF_close_Index_start = ui->AF_closeloop_indexStart_set->text().toInt();
        ui->AF_closeloop_indexStart->setText(QString("%1").arg(AF_close_Index_start));
    }
}

void curve_fitting::on_AF_closeloop_indexStop_set_returnPressed()
{
    if(ui->AF_closeloop_indexStop_set->text() != "")
    {
        AF_close_Index_stop = ui->AF_closeloop_indexStop_set->text().toInt();
        ui->AF_closeloop_indexStop->setText(QString("%1").arg(AF_close_Index_stop));
    }
}

void curve_fitting::on_AF_closeloop_CutNum_set_returnPressed()
{
    if(ui->AF_closeloop_CutNum_set->text() != "")
    {
        AF_close_Index_Cutnum = ui->AF_closeloop_CutNum_set->text().toInt();
        ui->AF_closeloop_CutNum->setText(QString("%1").arg(AF_close_Index_Cutnum));
    }
}
void curve_fitting::on_AF_closeloop_variableStart_set_returnPressed()
{
    if(ui->AF_closeloop_variableStart_set->text() != "")
    {
        AF_close_VStart = ui->AF_closeloop_variableStart_set->text().toInt();
        ui->AF_closeloop_variableStart->setText(QString("%1").arg(AF_close_VStart));
    }
}

void curve_fitting::on_AF_closeloop_variableStop_set_returnPressed()
{
    if(ui->AF_closeloop_variableStop_set->text() != "")
    {
        AF_close_VStop = ui->AF_closeloop_variableStop_set->text().toInt();
        ui->AF_closeloop_variableStop->setText(QString("%1").arg(AF_close_VStop));
    }
}

void curve_fitting::on_AF_closeloop_DataNum_set_returnPressed()
{
    if(ui->AF_closeloop_DataNum_set->text() != "")
    {
        AF_close_Datanum = ui->AF_closeloop_DataNum_set->text().toInt();
        ui->AF_closeloop_DataNum->setText(QString("%1").arg(AF_close_Datanum));
    }
}

void curve_fitting::on_AF_closeloop_OKCount_set_returnPressed()
{
    if(ui->AF_closeloop_OKCount_set->text() != "")
    {
        AF_close_OKCount = ui->AF_closeloop_OKCount_set->text().toInt();
        ui->AF_closeloop_OKCount->setText(QString("%1").arg(AF_close_OKCount));
    }
}

void curve_fitting::on_AF_closeloop_KeepVar_set_returnPressed()
{
    if(ui->AF_closeloop_KeepVar_set->text() != "")
    {
        AF_close_Var = ui->AF_closeloop_KeepVar_set->text().toInt();
        ui->AF_closeloop_KeepVar->setText(QString("%1").arg(AF_close_Var));
    }
}

void curve_fitting::on_AF_closeloop_getValueMode_choice_currentTextChanged(const QString& arg1)
{
    //if(ui->AF_closeloop_OKCount_set->text()!="")
    //{
    //   AF_close_OKCount= ui->AF_closeloop_OKCount_set->text().toInt();
    //   ui->AF_closeloop_OKCount->setText(AF_close_OKCount);
    //}
}
void curve_fitting::AF_AC_Stop()
{
    ui->progressBar_AC_CPro->setValue(0);
    ui->progressBar_AC_TPro->setValue(0);
    cfsendcon("AF_POWER_ON0");
    //QThread::sleep(50);
    cfsendcon("AF_RF_ON0");
    // QThread::sleep(50);
    cfsendcon("AF_POWER_ON0");
    // QThread::sleep(50);
    AF_START = false;
    if(ui->AF_StartAF->text() != "ACStart")
    {
        ui->AF_StartAF->setText("ACStart");
    }
}
void curve_fitting::AF_AC()
{
    if(AF_START)
    {
        //如果存在则覆盖 如果不存在则新建添加
        clearFinalDatas();
        CFProDatas[nowCFProject][nowlineClass.toInt()].insert(CJ_IV, finalDataTemp);
        for(int i = 0; i < ui->list_lineindex->count(); i++)
        {
            if(ui->list_lineindex->item(i)->text() == QString("%1").arg(CJ_IV))
            {
                ui->list_lineindex->setCurrentRow(i);
//                if(CJ_I=="FREQ")
//                {
//                    MainWindow::nowMainWindow->pm_setfreq(CJ_I.toDouble(),MainWindow::nowchanal);
//                }
                on_horizontalSlider_set_Variable_valueChanged(CJ_VV);
                return;
            }
        }
        QListWidgetItem* newitem = new QListWidgetItem(QString("%1").arg(CJ_IV));
        ui->list_lineindex->addItem(newitem);
        ui->list_lineindex->setCurrentItem(newitem);
        on_horizontalSlider_set_Variable_valueChanged(CJ_VV);
    }
}

void curve_fitting::on_AF_StartAF_clicked()
{
//    ui->progressBar_AC_CPro->setValue(0);
//    ui->progressBar_AC_TPro->setValue(0);
//    if(ui->AF_StartAF->text() == "ACStart")
//    {
//        //如果使用到了功率计 那必须连接上功率计后才能用
//        if(CJ_X == "POWER" || CJ_Y == "POWER" || CJ_Y == "POWERF")
//        {
//            if( MainWindow::PM_CON == 0)
//            {
//                showmsg("AF_erro:connect Powermeter first!");
//                return;
//            }
//        }
//        AF_close_Index_start = ui->AF_closeloop_indexStart_set->text().toInt();
//        ui->AF_closeloop_indexStart->setText(QString("%1").arg(AF_close_Index_start));
//        AF_close_Index_stop = ui->AF_closeloop_indexStop_set->text().toInt();
//        ui->AF_closeloop_indexStop->setText(QString("%1").arg(AF_close_Index_stop));
//        AF_close_Index_Cutnum = ui->AF_closeloop_CutNum_set->text().toInt();
//        ui->AF_closeloop_CutNum->setText(QString("%1").arg(AF_close_Index_Cutnum));
//        AF_close_VStart = ui->AF_closeloop_variableStart_set->text().toInt();
//        ui->AF_closeloop_variableStart->setText(QString("%1").arg(AF_close_VStart));
//        AF_close_VStop = ui->AF_closeloop_variableStop_set->text().toInt();
//        ui->AF_closeloop_variableStop->setText(QString("%1").arg(AF_close_VStop));
//        AF_close_Datanum = ui->AF_closeloop_DataNum_set->text().toInt();
//        ui->AF_closeloop_DataNum->setText(QString("%1").arg(AF_close_Datanum));
//        AF_close_OKCount = ui->AF_closeloop_OKCount_set->text().toInt();
//        ui->AF_closeloop_OKCount->setText(QString("%1").arg(AF_close_OKCount));
//        AF_close_Var = ui->AF_closeloop_KeepVar_set->text().toInt();
//        ui->AF_closeloop_KeepVar->setText(QString("%1").arg(AF_close_Var));
//        AF_close_MaxPower = ui->AF_closeloop_Max_Power_set->text().toInt();
//        ui->AF_closeloop_Max_Power->setText(QString("%1").arg(AF_close_MaxPower));
//        if(ui->LED_lineclassok->styleSheet() != "background-color: rgb(0, 0, 0);" && (CJ_C == "" || nowlineClass == ""))
//        {
//            showmsg("AF ERRO :nowlineClass == NULL");
//            return;
//        }
//        if(AF_close_MaxPower == 0) //如果使用到了MaxPower
//        {
//            showmsg("AF ERRO :AF_close_MaxPower == 0");
//            return;
//        }
//        if(AF_close_Index_Cutnum == 0)
//        {
//            showmsg("AF ERRO :Cutnum == 0");
//            return;
//        }
//        if(AF_close_Datanum < 10)
//        {
//            showmsg("AF ERRO :AF_close_Datanum == 0");
//            return;
//        }
//        if(AF_close_OKCount == 0)
//        {
//            showmsg("AF ERRO :OKnum == 0");
//            return;
//        }
//        if(AF_close_Var == 0)
//        {
//            showmsg("AF ERRO :Var == 0");
//            return;
//        }
//        AF_close_V_setp = (AF_close_VStop - AF_close_VStart) / AF_close_Datanum;
//        if(AF_close_V_setp < 1)
//        {
//            showmsg("AF ERRO :EachValue <1");
//            return;
//        }
//        AF_close_Index_setp = (AF_close_Index_stop - AF_close_Index_start) / AF_close_Index_Cutnum;
//        if(AF_close_Index_setp < 1)
//        {
//            AF_close_Index_setp = 1;
//        }
//        //开始自校准
//        CJ_IV = AF_close_Index_start;
//        CJ_VV = AF_close_VStart;
//        AF_START = true;
//        AF_AC();
//    }
//    else
//    {
//        AF_AC_Stop();
//    }
}


void curve_fitting::on_comboBox_CPUtype_choiced_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            CUPtype = WS_F103;
            break;
        case 1:
            CUPtype = WS_F429;
            break;
        default:
            break;
    }
    if(CUPtype == WS_F103)
    {
        CPU_used = ((DownDataSize * 1.0) / CPU_MS_WSF103) * 100;
    }
    else if(CUPtype == WS_F429)
    {
        CPU_used = ((DownDataSize * 1.0) / CPU_MS_WSF429) * 100;
    }
    update();
}

void curve_fitting::on_AF_con2stm_clicked()
{
    MainWindow::nowMainWindow->callcon2stm();
}

void curve_fitting::on_AF_recorddata_clicked()
{
    MainWindow::nowMainWindow->callrecorddata();
}

void curve_fitting::on_AF_closeloop_Max_Power_set_returnPressed()
{
}

void curve_fitting::on_InlineRecord_clicked()
{
    if(ui->InlineRecord->text() == "InlineRecord")
    {
        ui->InlineRecord->setText("dislineRecord");
        inlinerecord = true;
    }
    else
    {
        ui->InlineRecord->setText("InlineRecord");
        inlinerecord = false;
    }
}
