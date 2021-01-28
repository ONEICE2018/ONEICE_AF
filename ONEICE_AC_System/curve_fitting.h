#ifndef CURVE_FITTING_H
#define CURVE_FITTING_H

#include <QWidget>
#include "qcustomplot.h"
#include "axistag.h"
#include "mainwindow.h"
#include "con2stm.h"
#include <QStringListModel>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <onelistwidget.h>
#include <cf_lineclassdialog.h>

//文件操作
#include <qfile>
#include <QTextStream>
#include <QDir>
#include <onecurve.h>


namespace Ui
{
    class curve_fitting;
}
class heart;

class curve_fitting : public QWidget
{
    Q_OBJECT
public:
    explicit curve_fitting(QWidget* parent = 0);
    ~curve_fitting();
    static curve_fitting* cf;
    static int powerasked;
    QList<QString> getCFProjects() const;

    void setCFProjects() ;
    void setCFLineClass();
    void setCFLineIndex();

    typedef enum               //索引规定
    {
        AF_I_FREQ = 0
    } AF_Index_enum;

    typedef enum              //自变量规定
    {
        AF_V_DAC = 0,
        AF_V_TIME,
        AF_V_PSdBm
    } AF_Variable_enum;

    typedef enum              //因变量输入量规定
    {
        AF_X_ADC = 0,
        AF_X_POWER,
        AF_X_DAC
    } AF_X_enum;

    typedef enum              //因变量输入量规定
    {
        AF_Y_POWER = 0,
        AF_Y_POWERF,
        AF_Y_DAC,
        AF_Y_ADC
    } AF_Y_enum;


    typedef struct
    {
        AF_Index_enum    AF_I;
        AF_Variable_enum AF_V;
        AF_X_enum        AF_X;
        AF_Y_enum        AF_Y;

    } AF_Class_struct;

protected:
    void closeEvent(QCloseEvent* event);
    bool eventFilter(QObject* watched, QEvent* event) ;
signals:
    void cfdatasend(int, QByteArray);

private slots:
    void on_curver_dataChanged(uint16_t index, FinalDatas ft);
    void LC_append(QString append);
    void refpowerdata(int w);
    void oneListWidget_dataChanged(QString name);
    void oneListWidget_delate_last(QString name);
    void stm_acdataget(int w, QByteArray data);
    void on_CF_Button_NewProject_clicked();

    void on_list_projects_currentRowChanged(int currentRow);

    void on_CF_Button_NewLineClass_clicked();

    // void on_list_projects_itemChanged(QListWidgetItem *item);

    void on_CF_Button_NewLineIndex_clicked();

    void on_list_lineclass_currentRowChanged(int currentRow);

    void on_list_lineindex_currentRowChanged(int currentRow);

    void on_CF_SaveDatas_clicked();

    void on_showmsg_clear_clicked();

    void on_irrigation_clicked();

    void on_CureWindow_currentChanged(int index);

    void on_AF_Choice_Mode_currentIndexChanged(const QString& arg1);

    void on_openloop_set_maxvarible_returnPressed();

    void on_horizontalSlider_set_Variable_valueChanged(int value);

    void on_openloop_set_variable_returnPressed();

    void on_openloop_set_variable_sub_clicked();

    void on_openloop_set_variable_add_clicked();

    void on_openloop_save_onedata_clicked();

    void on_AF_OpenLoop_POWERSW_clicked();

    void on_AF_OpenLoop_RFSW_clicked();

    void on_AF_askonceXY_clicked();

    void on_AF_STLineUPLoad_clicked();

    void on_AF_STLineDownLoad_clicked();

    void on_AF_StartAF_clicked();

    void on_AF_closeloop_indexStart_set_returnPressed();

    void on_AF_closeloop_indexStop_set_returnPressed();

    void on_AF_closeloop_variableStart_set_returnPressed();

    void on_AF_closeloop_variableStop_set_returnPressed();

    void on_AF_closeloop_OKCount_set_returnPressed();

    void on_AF_closeloop_getValueMode_choice_currentTextChanged(const QString& arg1);

    void on_AF_closeloop_DataNum_set_returnPressed();

    void on_AF_closeloop_CutNum_set_returnPressed();

    void on_AF_closeloop_KeepVar_set_returnPressed();

    void on_comboBox_CPUtype_choiced_currentIndexChanged(int index);

    void on_AF_con2stm_clicked();

    void on_AF_recorddata_clicked();

    void on_AF_closeloop_Max_Power_set_returnPressed();

    void on_InlineRecord_clicked();

    void on_OneListWidget_Project_ItemClicked(int row);
    void on_OneListWidget_LineClass_ItemClicked(int row);
    void on_OneListWidget_Index_ItemClicked(int row);

private:
    Ui::curve_fitting* ui;

    int curvShowMode = 1; //默认是单个显示
    void showmsg(QString msg);
    void cfsendcon(QByteArray data);
    heart* refpower;
    int CPUusage_byte = 0;
    //画内存占用率
    bool Downfilechioced;
    uint32_t DownDataSize = 0;
    float CPU_used = 0;
    QString cpuused_fname = "";
    typedef  enum
    {
        WS_F103 = 0,
        WS_F429
    } CPUTYPE;
    CPUTYPE CUPtype = WS_F103;
    const uint32_t CPU_MS_WSF103 = 0x10000;
    const uint32_t CPU_MS_WSF429 = 0x20000;
    //采集相关
    QString CJ_C, CJ_I, CJ_V, CJ_X, CJ_Y; //数据采集对应的 曲线种类名 索引名 自变量名 X Y
    int CJ_IV, CJ_VV, CJ_XV, CJ_YV     ; //
    uint16_t ST_X = 0, ST_Y = 0          ; //芯片那边读回来的 X Y
    void setGetherUI(QString gtui)  ;//开环测试还是 闭环采样
    bool close_Started = false;
    bool inlinerecord = false;
    //开环测试
    int  openloop_MaxVariable = 4095;
    bool openloop_PowerON = false, openloop_RFON = false;
    //闭环测试
    bool EV_X_OK = false; //每一次获取X成功当XY同时成功时进行数据处理与记录
    bool EV_Y_OK = false; //每一次获取Y成功当XY同时成功时进行数据处理与记录
    void AF_chuli_xy();
    uint16_t AF_close_MaxPower     = 0,   //最大功率输出 在是用了功率计的情况下
             AF_close_Index_start  = 0,   //起始索引
             AF_close_Index_stop   = 0,   //终止索引
             AF_close_Index_Cutnum = 0,   //索引分段数（曲线条数）
             AF_close_Index_setp = 0,
             AF_close_VStart       = 0,   //自变量起点
             AF_close_VStop        = 2000,//自变量终点
             AF_close_V_setp = 0,
             AF_close_Datanum      = 80,  //每条曲线的数据量
             AF_close_OKCount      = 5,
             AF_close_Var          = 1,   //保持的偏差的阈值
             AF_now_OKCount        = 0    ; //当前保持次数
    uint8_t AF_close_ValuegetMode  = 0    ; //采集数据的模式
    bool AF_START = false                  ;
    uint16_t AF_old_Y = 0                  ;
    void AF_AC()                  ;
    void AF_AC_Stop();
    //数据传输
    QFile AF_LineFile;
    uint32_t LineFileDataSize = 0, AF_flen = 0; //初始文件长度 动态结果文件长度
    QDataStream* AF_Downloaddatastream;
    int AF_DownEachSize = 128; //每次下载字节数 128
    void AF_DownLoad_Stop();
    QByteArray AF_getProData(int pro);
    int AF_DownloadPro = 0;
    //数据上载
    uint32_t AF_UPDdataTlen = 0;
    QFile   AF_updataFile;
    QString nowloadname = "";
    //工程名字   //曲线种类码  { //曲线定位码 //key //value //首地址 }
    QMap<QString,              //不重复的名称
         QMap<uint16_t,        //每个名称对应不重复的ID 不同名称可以由相同的ID
         QMap<uint16_t,   //每个ID对应不重复的Index
         FinalDatas>>> CFProDatas;
    QMap<QString,              //不重复的名称
         QMap<uint16_t,        //每个名称对应不重复的ID 不同名称可以由相同的ID
         QMap<uint16_t,   //每个ID对应不重复的Index
         FinalDatas>>>::iterator CFProDatasi;
    QMap<uint16_t,              //每个名称对应不重复的ID 不同名称可以由相同的ID
         QMap<uint16_t,         //每个ID对应不重复的Index
         FinalDatas>>    lineClassTemp;
    QMap<uint16_t,              //每个名称对应不重复的ID 不同名称可以由相同的ID
         QMap<uint16_t,         //每个ID对应不重复的Index
         FinalDatas>>::iterator lineClassTempi;

    QMap<uint16_t,              //每个ID对应不重复的Index
         FinalDatas>          LineIndexTemp;
    QMap<uint16_t,              //每个ID对应不重复的Index
         FinalDatas>::iterator    LineIndexTempi;
    FinalDatas finalDataTemp;

    void clearFinalDatas()
    {
        finalDataTemp.addr = 0;
        finalDataTemp.KVS.clear();
        finalDataTemp.mVdata = 1500;
    }
    QByteArray qbyteArray_u16(uint16_t u16)
    {
        static QByteArray back_u16;
        back_u16.clear();
        back_u16.append(u16 >> 8 & 0xff);
        back_u16.append(u16 & 0xff);
        return back_u16;
    }
    QByteArray qbyteArray_u32(uint32_t u32)
    {
        static QByteArray back_u32;
        back_u32.clear();
        back_u32.append((u32 >> 24) & 0xff);
        back_u32.append((u32 >> 16) & 0xff);
        back_u32.append((u32 >> 8 ) & 0xff);
        back_u32.append(u32 & 0xff);
        return back_u32;
    }

    QByteArray HexStringToByteArray(QString HexString)
    {
        bool ok;
        QByteArray ret;
        HexString = HexString.trimmed();
        HexString = HexString.simplified();
        QStringList sl = HexString.split(" ");
        foreach (QString s, sl)
        {
            if(!s.isEmpty())
            {
                char c = s.toInt(&ok, 16) & 0xFF;
                if(ok)
                {
                    ret.append(c);
                }
                else
                {
                    qDebug() << "非法的16进制字符：" << s;
                    QMessageBox::warning(0, tr("错误："), QString("非法的16进制字符: \"%1\"").arg(s));
                }
            }
        }
        qDebug() << ret;
        return ret;
    }
    QString ByteArrayToHexString(QByteArray data)
    {
        QString ret(data.toHex().toUpper())    ;
        int len = ret.length() / 2               ;
        qDebug() << len                          ;
        for(int i = 1; i < len; i++)
        {
            qDebug() << i;
            ret.insert(2 * i + i - 1, " ");
        }
        return ret;
    }



    //init
    void CF_init(void);
    void test_Data_init();
    //增
    void CF_NewProject(QString proname);
    void CF_append_LineClass(int ID);
    void CF_NewIndex(int value);
    void CF_appenLineData(int k, int v);
    //工程数据变化
    void nowCFProject_insert(QString proname,  QMap<uint16_t, QMap<uint16_t, FinalDatas>> data);
    //查找索引
    QString nowCFProject = NULL, nowlineClass = NULL, nowIndex = NULL;




    ///文件操作
    QString  WorkSpace;//=QCoreApplication::applicationDirPath()+"\\ONELINEWorkspace";
    //读取
    int CF_LoadProject(QString proname);
    int CF_Load(QString ws);
    int putload(const QString proname, const QByteArray& loaddata);
    void analysis_CF_Context(QString proname, uint16_t lineclass, QString context);
    bool CF_ReName(QString path, QString name, QString newname)
    {
        QFile f(QString(path + "\\" + name + ".bin").replace("/", "\\"));
        if(f.exists())
        {
            return  QFile::rename(QString(path + "\\" + name + ".bin").replace("/", "\\"), QString(path + "\\" + newname + ".bin").replace("/", "\\"));
        }
        return false;
    }
    //写入
    void disable_saveUI();
    void enable_saveUI();
    int CF_SaveData(QString proname);
    int CF_Save(QString path, QString name, QByteArray data) //存储过程中不能操作
    {
        QDir dir;
        if(!dir.exists(path))
        {
            dir.mkdir(path);
        }
        QFile f(QString(path + "\\" + name + ".bin").replace("/", "\\"));
        if(!f.open(QIODevice::WriteOnly ))//不存在会自动创建
        {
            return -1;
        }
        if(data.length() > 0)
        {
            f.write(data.data(), data.length());
        }
        f.close();
    }
    void list_clear_project();
    void list_clear_class();
    void list_clear_index();
    void show_LineClass(QString Proname);
    void show_LineIndex(QString Proname, uint16_t classID);
    void show_LineDatas(QString Proname, uint16_t classID, uint16_t indexID);
    //为了自己能用LineClassdata 不束缚于管理器
    /////////////////////////////////////
    void CF_Datainit_fixd4LineClass();
    QMap<QString, int> CF_init_ClassID;
    QString CF_init_LC_BasePath = QCoreApplication::applicationDirPath() + "\\LC_Data";
    QString CF_init_LC_filename = "LCDATA.bin";
    //读取
    int CF_LC_Load(QString path, QString name)
    {
        QFile f(QString(path + "\\" + name).replace("/", "\\"));
        if(!f.open(QIODevice::ReadWrite  | QIODevice::Text))
        {
            return 0;
        }
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
                if(!CF_init_ClassID.contains(temp))
                {
                    CF_init_ClassID.insert(temp, QString(line.split(",")[0]).toInt());
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
        return 1;
    }
    int CF_LC_Clear(QString path, QString name)
    {
        QDir dir;
        if(!dir.exists(path))
        {
            dir.mkdir(path);
        }
        QFile f(QString(path + "\\" + name).replace("/", "\\"));
        if(!f.open(QIODevice::WriteOnly | QIODevice::Text))//不存在会自动创建
        {
            return -1;
        }
        QTextStream txtOutput(&f);
        txtOutput << "";
        f.close();
    }
    //写入
    int CF_LC_Write_ONE(QString path, QString name, QString data)
    {
        QDir dir;
        if(!dir.exists(path))
        {
            dir.mkdir(path);
        }
        QFile f(QString(path + "\\" + name).replace("/", "\\"));
        if(!f.open(QIODevice::Append | QIODevice::Text))//不存在会自动创建
        {
            return -1;
        }
        QString Context = QString("%1\n").arg(data);
        QTextStream txtOutput(&f);
        txtOutput << Context;
        f.close();
    }
    //////////////////////



};


#endif // CURVE_FITTING_H
