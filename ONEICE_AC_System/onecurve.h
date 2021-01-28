#ifndef ONECURVE_H
#define ONECURVE_H
#include "QThread"
#include <QWidget>
#include <QResizeEvent>
#include <QPainter>
struct FinalDatas
{
   uint32_t addr;
   uint16_t mVdata;
   QMap<uint16_t,uint16_t> KVS;
};
#include "curve_fitting.h"
class Drowcuvertime : public QThread
{
    Q_OBJECT
public:
    explicit Drowcuvertime(QObject *parent = 0);
//signals:
protected:
    void run();

        };
namespace Ui {
class OneCurve;
}

class OneCurve : public QWidget
{
    Q_OBJECT

public:
    explicit OneCurve(QWidget *parent = 0);
    ~OneCurve();

    void refcurve();
    //绘制一个点
    void drawOnePoint(uint16_t index,FinalDatas MAD);
    void drawMorePoint(uint16_t index,FinalDatas MAD);
    QMap<uint16_t,FinalDatas> ONELines;
signals:
    void on_OneCurver_Datahchanged(uint16_t,FinalDatas);
protected:
    //将matlab窗口设置为随着窗口变化而变化
//    virtual void resizeEvent(QResizeEvent *event) override;
//    void paintEvent(QPaintEvent *event);
     bool eventFilter(QObject *watched, QEvent *event) ;//用过这个来绘制子控件的图形

private slots:
     void on_refernceline_clicked();

     void on_axis_lockY_clicked();

     void on_axis_lockX_clicked();

     void on_list_curves_currentRowChanged(int currentRow);

     void list_curves_datachanged(QString uiname);

     void list_curves_lastonedelat(QString uiname);

     void on_lineEdit_maxY_returnPressed();

     void on_lineEdit_maxX_returnPressed();

     void on_curveZero_clicked();

     void ON_list_currentClicked(int row);
     void on_edit_mvdata_returnPressed();

     void on_edit_X_returnPressed();

     void on_edit_Y_returnPressed();

private:
    Ui::OneCurve *ui;

    void init();
    void drowCurves();
    int Max_X=4095;
    int Max_Y=16000;
    int getX(int x);
    int getY(int y);
    int view_W;
    int view_H;
    int disX=1;//坐标轴距离边框的距离
    int disY=1;//坐标轴距离边框的距离
    QPoint endpos;

    bool drowxyline=false;//是否在本帧画参考线
    bool Axis_x_lock=false;
    bool Axis_y_lock=false;
    void controller_Keep_Stat();
    const int MENUHIGHT=30;

    //数据分辨率
    int axis_start_x=0;
    int axis_start_y=0;
    int axis_stop_x=0;
    int axis_stop_y=0;
    #define MIN_X_SPAN 40
    #define MIN_Y_SPAN 40
    #define MAX_X_SPAN 65535
    #define MAX_Y_SPAN 65535

    //键盘组合键
    bool ctrl_OK=false;
    bool R_OK=false;
    bool C_OK=false;
    bool Z_OK=false;
    QPoint cmove_start;//移动图像的起始坐标
    QPoint cchange_start;
    QPoint mspoint;//专门用于拖拽事件

    bool mouse_left_OK=false;
    bool mouse_right_OK=false;

    int limit_X;
    int limit_Y;
    bool ChoiceP_OK=false;
    bool PointChoiced=false;
    QPoint ClikPoint   ;
    QPoint ChoicedPoint;//被选中的点 自动吸附到附近存在的点
    int Choicedindex=0;
    bool lineChoiced=false;

    void show_lineEditer(bool showxy,bool show);//showxy 是否使能选中点的值

};


#endif // ONECURVE_H
