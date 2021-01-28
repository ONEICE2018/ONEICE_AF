#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define MAX_SCPI_LEN    255
#define DEFAULT_TMO     5000

#include <QMainWindow>
#include <QThread>
#include "qextserialport.h"
#include "win_qextserialport.h"
#include "qextserialenumerator.h"
#include "qextserialport.h"
#include "qextserialbase.h"
#include <QtSerialPort>
//安捷伦相关
#include <QtNetwork>
#include <QWebSocket>
#include <QLibrary>
//#include <visa.h>
//子文件
#include "powermeter.h"
//拖动滚动
#include <QMouseEvent>
#include <QScrollBar>
//子页面
#include "datarecord_base.h"
#include "curve_fitting.h"
#include "con2stm.h"

#define DOFINDSERIAL   1
#define DOREFRASHDC    2
#define DOPollingread  3
#define DOPOWERMETER   4
#define DOPOWERSOURCE   5
#define DOSPECTROGRAPH  6
#define DOANA           7
#define DOACDATA        8
#define DOREFSTMSERIALPORT 9
#define DOREFSTMPOWER      10
class viclass : public QObject
{
    Q_OBJECT
public:
    viclass(void)
    {
    }

//    ViStatus viconsend(ViSession vi, ViConstString writeFmt)
//    {
//        if(vilife())
//        {
//            vinRetStatus =  viPrintf(vi, writeFmt);
//            return vinRetStatus;
//        }
//        else
//        {
//            return NULL;
//        }
//    }
//    char* vimyread(ViSession vi, ViConstString writeFmt)
//    {
//        if(vilife())
//        {
//            vinRetStatus = viQueryf( vi, writeFmt, "%s", virdBuff);
//            if(vinRetStatus == 0)
//            {
//                return virdBuff;
//            }
//            else return NULL;
//        }
//        else return NULL;
//    }
//    void viclose(void)
//    {
//        viClose( vipInstrHandle);
//        viClose( virmSession);
//        CONSTATE = false;
//    }
//    //vi leave or life 0 leave 1 life
//    bool vilife()
//    {
//        return CONSTATE;
//    }
//    ViStatus viconopen(QString myaddr)
//    {
//        vinRetStatus = viOpenDefaultRM(&virmSession);
//        QString getaddr = myaddr;
//        char*  addr;
//        QByteArray ba = getaddr.toLatin1(); // must
//        addr = ba.data();
//        vinRetStatus = viOpen( virmSession, addr, VI_NULL, VI_NULL, &vipInstrHandle);
//        if(vinRetStatus != 0)
//        {
//            return vinRetStatus;
//            CONSTATE = false;
//        }
//        else
//        {
//            /* set visa Format */
//            vinRetStatus = viSetAttribute( vipInstrHandle, VI_ATTR_TMO_VALUE, DEFAULT_TMO);
//            vinRetStatus = viSetAttribute( vipInstrHandle, VI_ATTR_SUPPRESS_END_EN, VI_FALSE);
//            vinRetStatus = viSetAttribute( vipInstrHandle, VI_ATTR_SEND_END_EN, VI_FALSE);
//            //viconsend(vipInstrHandle,":CALCulate:MARKer1:STATe 0");
//            CONSTATE = true;
//            return 0;
//        }
//    }
//public:
//    bool CONSTATE = false;
//    /* Visa Session*/
//    ViStatus  vinRetStatus;
//    ViSession virmSession = 0;
//    ViSession vipInstrHandle;
//    //heart *vi_queryheart;
//    char viwrBuff[MAX_SCPI_LEN];
//    char virdBuff[MAX_SCPI_LEN];
//    int  visaouttime = 0;

////signals:
////    void maindo(int);

////private:



};

enum submitkey
{
    submit_remote = 0,
    submit_local,
    submit_ALLOut_ON,
    submit_ALLOut_OFF,
    submit_CH1Vset,
    submit_CH1Iset,
    submit_CH1Out,
    submit_CH1Vstep,
    submit_CH1Vstep_u,
    submit_CH1Vstep_d,
    submit_CH1Istep,
    submit_CH1Istep_u,
    submit_CH1Istep_d,
    submit_CH2Vset,
    submit_CH2Iset,
    submit_CH2Out,
    submit_CH2Vstep,
    submit_CH2Vstep_u,
    submit_CH2Vstep_d,
    submit_CH2Istep,
    submit_CH2Istep_u,
    submit_CH2Istep_d,
    submit_CH3Vset,
    submit_CH3Iset,
    submit_CH3Out,
    submit_CH3Vstep,
    submit_CH3Vstep_u,
    submit_CH3Vstep_d,
    submit_CH3Istep,
    submit_CH3Istep_u,
    submit_CH3Istep_d
};

//vi同步读取异步化
//class vi2tread : public QThread
//{
//    Q_OBJECT
//public:
//    explicit vi2tread(QObject *parent = 0);
//    vi2tread(int dowhat,viclass *mvi,ViConstString writeFmt){
//       this->dowhat= dowhat;
//       this->mvi=mvi;
//       this->writeFmt=writeFmt;
//    }
//static int maindobusy;

//signals:
//    void vi2treaddo   (int ,QString,QString); //哪里问的 问的内容是 问的结果是
//    void vi2treaddopm (int ,QString,QString);
//    void vi2treaddops (int ,QString,QString);
//    void vi2treaddosp (int ,QString,QString);
//    void vi2treaddoana(int ,QString,QString);
//    void vi2treaddoacd(int ,QString,QString);
//protected:
//    void run();
//private:
//    int dowhat;
//    viclass *mvi;
//    ViConstString writeFmt;
//    void disable(){
//        this->quit();
//        this->wait();
//    }
//        };


class heart : public QThread
{
    Q_OBJECT
public:
    explicit heart(QObject* parent = 0);
    heart(int dowhat)
    {
        this->dowhat = dowhat;
    }
    static int maindobusy;

signals:
    void maindo(int);
    void maindopm(int);
    void maindops(int);
    void maindosp(int);
    void maindoana(int);
    void maindoacd(int);
    void condorefserial(int);
    void cfdorefpower(int);
protected:
    void run();
private:
    int dowhat;
};

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();
    void setseriallist();

    Win_QextSerialPort* DCserial;                //端口,用来描述具体的一个端口
    QextSerialEnumerator* enumerator;    //在线串口统计类
    void DCconsend(QString conString);
    void DCshowmsg(QString msg);

    //刷新率控制
    static int ana_reftime;//用于控制刷新率
    static int getanareftime();
    static void setana_reftime(int reftime);

    static int pm_reftime;//用于控制刷新率
    static int getpmreftime();
    static void setpm_reftime(int reftime);

    //数据中心
    static QMap<QString, QString> DCdata; //直流电源数据
    static QMap<QString, QString> PSdata; //功率源数据
    static QMap<QString, QString> SPdata; //频谱仪数据
    static QMap<QString, QString> PMdata; //功率计
    static QMap<QString, QString> ANAdata; //网分
    static QMap<QString, QMap<QString, QString>> CENTERdata; //数据源 <数据名称 数据值>
    void datainit_all();
    static int nowchanal;//功率计的单位
    //数据同步
    void syncdata_dc(QString name, QString value);
    void syncdata_ps(QString name, QString value);
    void syncdata_sp(QString name, QString value);
    void syncdata_pm(QString name, QString value);
    void syncdata_ana(QString name, QString value);
    //功能界面
    static int goto_once_dkb        ;
    static int goto_once_cf         ;
    static int goto_once_con2stm    ;
    static int PM_CON               ;           //功率计连接只是状态
    static MainWindow* nowMainWindow;
    void callcon2stm()   ;
    void callrecorddata();
    void callcurvefitting();
    //功率计
    void pm_showmsg(QString msg);
    void pm_init();
    void pm_choicech(int ch);
    void pm_setfreq(double freq, QString uint);
    QString returnchmsg(int ch);
    void pm_conuiinit();


protected:
    void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent* event);
signals:
    void cfrepower(int);
private slots:

    //矢量网络分析仪
    //void dorefrashana(int w);
    //void doreadana(int w,QString askfor,QString value);
    //频谱仪
    // void dorefrashsp(int w);
    //信号源
    //void dorefrashps(int w);
    //功率计槽
    // void dorefrashpm(int w);
    //电源槽
    void dorefrashdc(int w);
    void letmedo(int w);
    void DCserialRead(int w);
    void on_con2dc_clicked();

    void on_dc_ch_ch1_clicked();

    void on_dc_ch1_sw_clicked();

    void on_dc_ch_ch2_clicked();

    void on_dc_ch2_sw_clicked();

    void on_dc_ch_ch3_clicked();

    void on_dc_ch3_sw_clicked();

    void on_dc_rl_clicked();

    void on_dc_singch_clicked();

    void on_dc_l_clicked();

    void on_dc_ch1_vset_clicked();

    void on_dc_ch1_iset_clicked();

    void on_dc_ch2_vset_clicked();

    void on_dc_ch2_iset_clicked();

    void on_dc_ch3_vset_clicked();

    void on_dc_ch3_iset_clicked();

    void on_dc_ch1_vstep_up_clicked();

    void on_dc_ch1_vstep_clicked();

    void on_dc_ch1_vstep_down_clicked();

    void on_dc_ch1_istep_up_clicked();

    void on_dc_ch1_istep_clicked();

    void on_dc_ch1_istep_down_clicked();

    void on_dc_ch2_vstep_up_clicked();

    void on_dc_ch2_vstep_clicked();

    void on_dc_ch2_vstep_down_clicked();

    void on_dc_ch2_istep_up_clicked();

    void on_dc_ch2_istep_clicked();

    void on_dc_ch2_istep_down_clicked();

    void on_dc_ch3_vstep_up_clicked();

    void on_dc_ch3_vstep_clicked();

    void on_dc_ch3_vstep_down_clicked();

    void on_dc_ch3_istep_up_clicked();

    void on_dc_ch3_istep_clicked();

    void on_dc_ch3_istep_down_clicked();

    void on_dc_allon_clicked();

    void on_dc_alloff_clicked();

    void on_dc_ch1_edit_vset_returnPressed();

    void on_dc_ch1_edit_iset_returnPressed();

    void on_dc_ch2_edit_vset_returnPressed();

    void on_dc_ch2_edit_iset_returnPressed();

    void on_dc_ch3_edit_vset_returnPressed();

    void on_dc_ch3_edit_iset_returnPressed();

    void on_dc_ch1_edit_vstep_returnPressed();

    void on_dc_ch1_edit_istep_returnPressed();


    void on_dc_ch2_edit_istep_returnPressed();

    void on_dc_ch3_edit_vstep_returnPressed();

    void on_dc_ch3_edit_istep_returnPressed();

    void on_dc_ch2_edit_vstep_returnPressed();

//    void on_con2powermeter_clicked();

//    void on_pm_uperunit_currentTextChanged(const QString& arg1);

//    void on_pm_lowerunit_currentTextChanged(const QString& arg1);

//    void on_pm_ch_ghz_clicked();

//    void on_pm_ch_mhz_clicked();

//    void on_pm_ch_khz_clicked();

//    void on_pm_eidt_freq_returnPressed();

////    void on_pm_freq_down_clicked();

////    void on_pm_freq_up_clicked();

//    void on_pm_edit_addr_returnPressed();

//    void on_pm_offsetsw_clicked();

//    void on_pm_edit_offset_returnPressed();

//    void on_con2powersource_clicked();

//    void on_ps_modesw_clicked();

//    void on_ps_rfsw_clicked();

//    void on_ps_edit_addr_returnPressed();

//    void on_ps_plussw_clicked();

//    void on_ps_modech_currentTextChanged(const QString& arg1);

//    void on_ps_edit_width_returnPressed();

//    void on_ps_edit_period_returnPressed();

//    void on_ps_edit_delay_returnPressed();

//    void on_ps_edit_freq_returnPressed();

//    void on_ps_edit_pwd_returnPressed();

//    void on_ps_freq_unit_currentTextChanged(const QString& arg1);

//    void on_ps_unit_width_currentTextChanged(const QString& arg1);

//    void on_ps_unit_period_currentTextChanged(const QString& arg1);

//    void on_ps_unit_delay_currentTextChanged(const QString& arg1);

//    void on_con2spectrograph_clicked();

//    void on_sp_unit_currentTextChanged(const QString& arg1);

//    void on_sp_edit_addr_returnPressed();

//    void on_sp_marker_1_clicked();

//    void on_sp_marker_2_clicked();

//    void on_sp_marker_3_clicked();

//    void on_sp_marker_4_clicked();

//    void on_sp_marker_5_clicked();

//    void on_sp_marker_6_clicked();

//    void on_sp_marker_7_clicked();

//    void on_sp_marker_8_clicked();

//    void on_sp_marker_9_clicked();

//    void on_sp_marker_10_clicked();

//    void on_sp_marker_11_clicked();

//    void on_sp_marker_12_clicked();

//    void on_sp_m1_edit_fv_returnPressed();

//    void on_sp_m2_edit_fv_returnPressed();

//    void on_sp_m3_edit_fv_returnPressed();

//    void on_sp_m4_edit_fv_returnPressed();

//    void on_sp_m5_edit_fv_returnPressed();

//    void on_sp_m6_edit_fv_returnPressed();

//    void on_sp_m7_edit_fv_returnPressed();

//    void on_sp_m8_edit_fv_returnPressed();

//    void on_sp_m9_edit_fv_returnPressed();

//    void on_sp_m10_edit_fv_returnPressed();

//    void on_sp_m11_edit_fv_returnPressed();

//    void on_sp_m12_edit_fv_returnPressed();

//    void on_sp_edit_refleve_returnPressed();

//    void on_sp_edit_freq_start_returnPressed();

//    void on_sp_edit_freq_stop_returnPressed();

//    void on_sp_edit_freq_centre_returnPressed();

//    void on_sp_edit_freq_span_returnPressed();

//    void on_sp_edit_offset_returnPressed();

//    void on_sp_trigdelay_edit_returnPressed();

//    void on_sp_triger_mode_currentTextChanged(const QString& arg1);

//    void on_sp_triger_delaysw_clicked();

//    void on_sp_edit_sweeptime_edit_returnPressed();

//    void on_con2ana_clicked();

//    void on_ana_edit_addr_returnPressed();



//    void on_ana_start_clicked();

//    void on_ana_channelprev_clicked();

//    void on_ana_channelnext_clicked();

//    void on_ana_traceprev_clicked();

//    void on_ana_tracenext_clicked();

//    void on_ana_channelmax_clicked();

//    void on_ana_tracemax_clicked();

//    void on_ana_meas_clicked();

//    void on_ana_format_clicked();

//    void on_ana_scale_clicked();

//    void on_ana_display_clicked();

//    void on_ana_avg_clicked();

//    void on_ana_cal_clicked();

//    void on_ana_stop_clicked();

//    void on_ana_center_clicked();

//    void on_ana_span_clicked();

//    void on_ana_sweepsetup_clicked();

//    void on_ana_trigger_clicked();

//    void on_ana_marker_clicked();

//    void on_ana_markersearch_clicked();

//    void on_ana_markerfctn_clicked();

//    void on_ana_analysis_clicked();

//    void on_ana_edit_numoftrace_returnPressed();

//    void on_ana_edit_freq_start_returnPressed();

//    void on_ana_edit_span_returnPressed();

//    void on_ana_edit_freq_center_returnPressed();

//    void on_ana_edit_freq_stop_returnPressed();

//    void on_ana_maker1sw_clicked();

//    void on_ana_marker1_freqedit_returnPressed();


//    void on_ana_meas_s11_clicked();

//    void on_ana_meas_s12_clicked();

//    void on_ana_meas_s21_clicked();

//    void on_ana_meas_s22_clicked();

//    void on_ana_maker2sw_clicked();

//    void on_ana_maker3sw_clicked();

//    void on_ana_maker4sw_clicked();

//    void on_ana_maker5sw_clicked();

//    void on_ana_maker6sw_clicked();

//    void on_ana_maker7sw_clicked();

//    void on_ana_maker8sw_clicked();

//    void on_ana_maker9sw_clicked();

//    void on_ana_marker2_freqedit_returnPressed();

//    void on_ana_marker3_freqedit_returnPressed();

//    void on_ana_marker4_freqedit_returnPressed();

//    void on_ana_marker5_freqedit_returnPressed();

//    void on_ana_marker6_freqedit_returnPressed();

//    void on_ana_marker7_freqedit_returnPressed();

//    void on_ana_marker8_freqedit_returnPressed();

//    void on_ana_marker9_freqedit_returnPressed();

//    void on_ana_fomat_combox_currentTextChanged(const QString& arg1);

//    void on_ana_edit_reftime_returnPressed();

//    void on_goto_uirecorddata_clicked();

    void on_ac_curve_fitting_clicked();

    void on_con2stm_clicked();

//    void on_edit_pm_rt_returnPressed();

private:
    Ui::MainWindow* ui;
    void mainUiInit();


    void DCrefrashCh(int step);
    bool DCstepisinarea(int step);
    void DCrefrash(int step, QString msg);
    void getnextrefstep(void);


    int DCCH1enable = 0; //通道1使能标志位
    int DCCH2enable = 0; //通道2使能标志位
    int DCCH3enable = 0; //通道3使能标志位
    int DCSINGLECHANNEL = 0; //通道3使能标志位
    void initdatas();
    QMap<submitkey, bool> DCenablemap; //指令提交使能
    QMap<submitkey, double> DCvaluemap; //提交指令的设定值
    void dcsubmit(bool enable, submitkey what, QString value);
    void dcsubmit_clear();
    void dosubmit();
    int  DCinwhatarea(int step);
    void choicech(int CH);
    int  dcsubchakstep(submitkey what);





    //功率源
    void ps_showmsg(QString msg);
    void ps_init();
    void ps_conuiinit();

    void ps_setfreq(double freq, QString uint);
    void ps_setpout(double pout);
    void ps_setwidth(double width, QString unit);
    void ps_setperiod(double period, QString unit);
    void ps_setdelay(double delay, QString unit);
    //频谱仪
    void sp_showmsg(QString msg);
    void sp_init();
    void sp_conuiinit();
    void sp_makersw(int w);
    //void sp_sendeddo(ViSession vi, ViConstString writeFmt);
    void sp_mainwindowfreqset(int ch);
    void sp_makerfreqset(int ch, QString value);
    //适量网络分析仪

    int ana_nowtrace = 1;
    int getana_nowtrace()
    {
        return ana_nowtrace;
    }
    void setana_nowtrace(int nowtrace);
    int ana_showtracenum = 0;
    int  getana_showtracenum()
    {
        return ana_showtracenum;
    }
    void setana_showtracenum(int tracenum);
    QMap<int, QString> ana_tracemode_map; //显示的每个trace对应的format  mag log.....
    QMap<int, QString> ana_meas_map;    //显示的每个参数对应的meas S11......
    void ana_showmsg(QString msg);
    void ana_init();
    void ana_conuiinit();
    //void ana_sendeddo(ViSession vi, ViConstString writeFmt);
    void ana_showmarkerlist(int markernumber, QString value, QString mode);
    void ana_marker_sw(int mknum, int swvalue);
    void ana_marker_freqset(int mknum, QString freqnumvalue);
    void ana_init_freq();
    void ana_choice_meas(QString ch);
    char* QString2char(QString string)
    {
        static char*  ch = NULL;
        static QByteArray ba = NULL; // must
        ba = string.toLatin1();
        ch = ba.data();
        return ch;
    }

    QString unit_freq(double freq)
    {
        QString back;
        if(freq >= 1000000000)
        {
            back.append(QString("%1").arg(freq / 1000000000.0));
            back.append("GHZ");
            return back;
        }
        else if(freq >= 1000000)
        {
            back.append(QString("%1").arg(freq / 1000000.0));
            back.append("MHZ");
            return back;
        }
        else if(freq >= 1000)
        {
            back.append(QString("%1").arg(freq / 1000.0));
            back.append("KHZ");
            return back;
        }
        else
        {
            back.append(QString("%1").arg(freq));
            back.append("HZ");
            return back;
        }
    }
    QString unit_time(double time)
    {
        QString back;
        if(time >= 1)
        {
            back.append(QString("%1").arg(time));
            back.append("s");
            return back;
        }
        else if(time >= 1 / 1000.0)
        {
            back.append(QString("%1").arg(time * 1000));
            back.append("ms");
            return back;
        }
        else if(time >= 1 / 1000000.0)
        {
            back.append(QString("%1").arg(time * 1000000));
            back.append("us");
            return back;
        }
        else
        {
            back.append(QString("%1").arg(time * 1000000000));
            back.append("ns");
            return back;
        }
    }
    double vigetdouble(QString data)
    {
        double  Freqvalue = QString(QString(data.split("\\")[0]).split("E")[0]).toDouble();
        double  xs = QString(QString(data.split("\\")[0]).split("E")[1]).toDouble();
        //if(Freqvalue>0){
        for(int i = 0; i < abs(xs); i++)
        {
            if(xs >= 0)
            {
                Freqvalue = Freqvalue * 10;
            }
            else
            {
                Freqvalue = Freqvalue / 10;
            }
        }
        //        }else{
        //            Freqvalue=0;
        //        }
        return Freqvalue;
    }
    QString getnumfreq(QString freq, QString unit)
    {
        double fv = freq.toDouble();
        if(fv > 0)
        {
            if(unit == "GHz")
            {
                return QString("%1").arg(fv * 1000000000);
            }
            else if(unit == "MHz")
            {
                return QString("%1").arg(fv * 1000000);
            }
            else if(unit == "KHz")
            {
                return QString("%1").arg(fv * 1000);
            }
        }
        else
            return NULL;
    }

};

#endif // MAINWINDOW_H
