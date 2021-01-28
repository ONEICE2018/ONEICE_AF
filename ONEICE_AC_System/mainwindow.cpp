#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "onecurve.h"
#define MAINDOBUSY 1
#define MAINDOFREE 0
datarecord_base *gotodkb;
curve_fitting *gotocf;
con2stm *gotocon2stm;

MainWindow *MainWindow::nowMainWindow=nullptr;
int MainWindow::goto_once_cf=0;
int MainWindow::goto_once_con2stm=0;
void  MainWindow::callcon2stm()
{
    on_con2stm_clicked();
}
void  MainWindow::callrecorddata()
{
    on_goto_uirecorddata_clicked();
}
void MainWindow::on_con2stm_clicked()
{

    if(goto_once_con2stm==0){
        goto_once_con2stm=1;

        gotocon2stm =new con2stm();
        gotocon2stm->show();
    }else{
        if (gotocon2stm->isMinimized())
        {
            gotocon2stm->showNormal();
        }
        //设置窗口置顶
        ::SetWindowPos(HWND(gotocon2stm->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        ::SetWindowPos(HWND(gotocon2stm->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

       gotocon2stm->show();
       gotocon2stm->activateWindow();
    }

}
void MainWindow::on_goto_uirecorddata_clicked()
{

    if(goto_once_dkb==0){
        goto_once_dkb=1;

        gotodkb =new datarecord_base();
        gotodkb->show();
    }else{
        if (gotodkb->isMinimized())
        {
            gotodkb->showNormal();
        }
        //设置窗口置顶
        ::SetWindowPos(HWND(gotodkb->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        ::SetWindowPos(HWND(gotodkb->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

       gotodkb->show();
       gotodkb->activateWindow();
    }
}
void MainWindow::on_ac_curve_fitting_clicked()
{
    if(goto_once_cf==0){
        goto_once_cf=1;

        gotocf =new curve_fitting();
        QObject::connect(this,SIGNAL(cfrepower(int)), gotocf, SLOT(refpowerdata(int)));
        gotocf->show();
    }else{
        if (gotocf->isMinimized())
        {
            gotocf->showNormal();
        }
        //设置窗口置顶
        ::SetWindowPos(HWND(gotocf->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        ::SetWindowPos(HWND(gotocf->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

       gotocf->show();
       gotocf->activateWindow();
    }
}

int heart::maindobusy=MAINDOFREE;
heart *refrashDC;
int quitrefrashDC=0;
int MainWindow::goto_once_dkb=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
   {
    ui->setupUi(this);
    nowMainWindow=this;
    //combox添加子项方法   QIcon   icon;
    //    icon.addFile(":/images/icons/aim.ico");
    //    ui->comboBox->clear(); //清除列表
    //    for (int i=0;i<20;i++)
    //        ui->comboBox->addItem(icon,QString::asprintf("Item %d",i)); //带图标
    //        //ui->comboBox->addItem(QString::asprintf("Item %d",i)); //不带图标

    heart *findserial;
    findserial=new heart(DOFINDSERIAL);
    findserial->start();
    QObject::connect(findserial,SIGNAL(maindo(int)), this, SLOT(letmedo(int)));
    enumerator = new QextSerialEnumerator();
    initdatas();
    pm_init();
    ps_init();
    sp_init();
    ana_init();
    mainUiInit();
    datainit_all();
    //OneCurve *testoc=new OneCurve();
    //testoc->show();
}
QString border_pm="#widget_pm{border-image: url(:/imgsource/N1911A.png);}";
QString border_ps="#widget_ps{border-image: url(:/imgsource/N5182A.png);}";
QString border_sp="#widget_sp{border-image: url(:/imgsource/N90XX.png);}";
QString border_dcp="#widget_dcp{border-image: url(:/imgsource/DC power.png);}";
QString border_ana="#widget_ana{border-image: url(:/imgsource/E5071.png);}";
void MainWindow::mainUiInit(){
    ui->widget_pm->setStyleSheet(border_pm);
    ui->widget_ps->setStyleSheet(border_ps);
    ui->widget_sp->setStyleSheet(border_sp);
    ui->widget_dcp->setStyleSheet(border_dcp);
    ui->widget_ana->setStyleSheet(border_ana);
}

bool            m_bMousePressed;
QPoint          m_PressPosition;


void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (!m_bMousePressed)
    {
        return;
    }

    QPoint currentPt = e->pos();

    int ye = m_PressPosition.y() - currentPt.y();
    int xe = m_PressPosition.x() - currentPt.x();
    ui->mainscrollarea->horizontalScrollBar()->setValue(ui->mainscrollarea->horizontalScrollBar()->value()+xe);
    ui->mainscrollarea->verticalScrollBar()->setValue(ui->mainscrollarea->verticalScrollBar()->value()+ye);
    m_PressPosition = currentPt;
}
void MainWindow::closeEvent(QCloseEvent *event){
    if(goto_once_dkb==1){
    MainWindow::goto_once_dkb=0;
        gotodkb->close();
    }
    if(goto_once_cf==1){
    MainWindow::goto_once_cf=0;
        gotocf->close();
    }

    if(goto_once_con2stm==1){
    MainWindow::goto_once_con2stm=0;
        gotocon2stm->close();
    }

}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    m_bMousePressed = true;

    m_PressPosition = e->pos();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    m_bMousePressed = false;
    m_PressPosition.setX(0);
    m_PressPosition.setY(0);
}




void MainWindow::initdatas(){
    DCenablemap.insert(submit_ALLOut_ON ,false);
    DCenablemap.insert(submit_ALLOut_OFF,false);
    DCenablemap.insert(submit_CH1Iset   ,false);
    DCenablemap.insert(submit_CH1Istep  ,false);
    DCenablemap.insert(submit_CH1Out    ,false);
    DCenablemap.insert(submit_CH1Vset   ,false);
    DCenablemap.insert(submit_CH1Vstep  ,false);
    DCenablemap.insert(submit_CH1Vstep_u,false);
    DCenablemap.insert(submit_CH1Vstep_d,false);
    DCenablemap.insert(submit_CH2Iset   ,false);
    DCenablemap.insert(submit_CH2Istep  ,false);
    DCenablemap.insert(submit_CH2Istep_u,false);
    DCenablemap.insert(submit_CH2Istep_d,false);
    DCenablemap.insert(submit_CH2Out    ,false);
    DCenablemap.insert(submit_CH2Vset   ,false);
    DCenablemap.insert(submit_CH2Vstep  ,false);
    DCenablemap.insert(submit_CH2Vstep_u,false);
    DCenablemap.insert(submit_CH2Vstep_d,false);
    DCenablemap.insert(submit_CH3Iset   ,false);
    DCenablemap.insert(submit_CH3Istep  ,false);
    DCenablemap.insert(submit_CH3Istep_u,false);
    DCenablemap.insert(submit_CH3Istep_d,false);
    DCenablemap.insert(submit_CH3Out    ,false);
    DCenablemap.insert(submit_CH3Vset   ,false);
    DCenablemap.insert(submit_CH3Vstep  ,false);
    DCenablemap.insert(submit_CH3Vstep  ,false);
    DCenablemap.insert(submit_CH3Vstep_u,false);
    DCenablemap.insert(submit_CH3Vstep_d,false);
    DCenablemap.insert(submit_remote    ,false);
    DCenablemap.insert(submit_local     ,false);

    DCenablemap.insert(submit_ALLOut_ON ,0);
    DCenablemap.insert(submit_ALLOut_OFF,0);
    DCenablemap.insert(submit_CH1Iset   ,0);
    DCenablemap.insert(submit_CH1Istep  ,0);
    DCenablemap.insert(submit_CH1Out    ,0);
    DCenablemap.insert(submit_CH1Vset   ,0);
    DCenablemap.insert(submit_CH1Vstep  ,0);
    DCenablemap.insert(submit_CH1Vstep_u,0);
    DCenablemap.insert(submit_CH1Vstep_d,0);
    DCenablemap.insert(submit_CH2Iset   ,0);
    DCenablemap.insert(submit_CH2Istep  ,0);
    DCenablemap.insert(submit_CH2Istep_u,0);
    DCenablemap.insert(submit_CH2Istep_d,0);
    DCenablemap.insert(submit_CH2Out    ,0);
    DCenablemap.insert(submit_CH2Vset   ,0);
    DCenablemap.insert(submit_CH2Vstep  ,0);
    DCenablemap.insert(submit_CH2Vstep_u,0);
    DCenablemap.insert(submit_CH2Vstep_d,0);
    DCenablemap.insert(submit_CH3Iset   ,0);
    DCenablemap.insert(submit_CH3Istep  ,0);
    DCenablemap.insert(submit_CH3Istep_u,0);
    DCenablemap.insert(submit_CH3Istep_d,0);
    DCenablemap.insert(submit_CH3Out    ,0);
    DCenablemap.insert(submit_CH3Vset   ,0);
    DCenablemap.insert(submit_CH3Vstep  ,0);
    DCenablemap.insert(submit_CH3Vstep  ,0);
    DCenablemap.insert(submit_CH3Vstep_u,0);
    DCenablemap.insert(submit_CH3Vstep_d,0);
    DCenablemap.insert(submit_remote    ,0);
    DCenablemap.insert(submit_local     ,0);

}

void MainWindow::letmedo(int w){
   heart::maindobusy=MAINDOBUSY;
   MainWindow::setseriallist();
   heart::maindobusy=MAINDOFREE;
}

MainWindow::~MainWindow()
{
    delete ui;
}

//vi2tread::vi2tread(QObject *parent) :
//    QThread(parent)
//{

//}
//void vi2tread::run(){
//      QString value="";
//      value=this->mvi->vimyread(this->mvi->vipInstrHandle,this->writeFmt);
//      if(value!=NULL){
//          emit this->vi2treaddo(DOANA,QString(this->writeFmt),value);
//          this->mvi->visaouttime=0;
//      }
//      else{
//        this->mvi->visaouttime++;
//          if(this->mvi->visaouttime>2){
//              this->mvi->visaouttime=0;
//              this->mvi->viclose();
//              return;

//          }
//      }
//      this->disable();

//}

heart::heart(QObject *parent) :
    QThread(parent)
{

}
void heart::run(){
    switch (dowhat) {
    case DOFINDSERIAL:
        while(1)
        {
            this->msleep(500);
            while(maindobusy==MAINDOBUSY)//获取触发权限
            {
                this->msleep(50);
            }
            emit maindo(DOFINDSERIAL);
        }
        break;
    case DOREFRASHDC:
        while(quitrefrashDC==0)
        {
            this->msleep(5);
            emit maindo(DOREFRASHDC);
            this->msleep(55);
            emit maindo(DOPollingread);
        }
        break;
    case DOPOWERMETER:
        while(1){
            this->msleep(MainWindow::getpmreftime());
            emit maindopm(DOPOWERMETER);
        }
        break;
    case DOPOWERSOURCE:
        while(1){
            this->msleep(300);
            emit maindops(DOPOWERSOURCE);
        }
        break;
    case DOSPECTROGRAPH:
        while(1){
            this->msleep(100);
            emit maindosp(DOSPECTROGRAPH);
        }
        break;
    case DOANA:
        while(1){
            this->msleep(MainWindow::getanareftime());
            emit maindoana(DOANA);
        }
        break;
    case DOACDATA:
        while(1){
            this->msleep(200);
            emit maindoacd(DOACDATA);
        }
        break;
    case DOREFSTMSERIALPORT:
        while(1){
            this->msleep(300);
            emit condorefserial(DOREFSTMSERIALPORT);
        }
        break;
    case DOREFSTMPOWER:
        while(1){
            this->msleep(200);
            emit cfdorefpower(DOREFSTMPOWER);
        }
        break;
    default:
        break;
    }
}

/*直流电源*/
int refstep=0;//访问进度
int DCSUBED=0;//提交写指令 使能
submitkey SUBEDNUM=submit_local;
int DCREADING=0;
int DCREADINGLOST=0;
int DCLASTCONNUM=0;
QString Style_choiced="background-color: rgb(85, 255, 0);";
QString Style_switchon="background-color: rgb(0, 85, 255);";
QString Style_submiting="background-color: rgb(255, 255, 0);";
QString Style_submited="";
QString Style_dcpconneted="background-color: rgb(255, 255, 255);";
QString Style_dcpdisconneted="background-color: rgb(88, 88, 88);";
QString border_dcp_connected="#widget_dcp{border-image: url(:/imgsource/DC poweron.png);}";
QString border_dcp_disconnected="#widget_dcp{border-image: url(:/imgsource/DC power.png);}";
void MainWindow::dcsubmit_clear(){
  dcsubmit(false,  submit_remote,     "0");
  dcsubmit(false,  submit_local,      "0");
  dcsubmit(false,  submit_ALLOut_ON,  "0");
  dcsubmit(false,  submit_ALLOut_OFF, "0");
  dcsubmit(false,  submit_CH1Vset,    "0");
  dcsubmit(false,  submit_CH1Iset,    "0");
  dcsubmit(false,  submit_CH1Out,     "0");
  dcsubmit(false,  submit_CH1Vstep,   "0");
  dcsubmit(false,  submit_CH1Vstep_u, "0");
  dcsubmit(false,  submit_CH1Vstep_d, "0");
  dcsubmit(false,  submit_CH1Istep,   "0");
  dcsubmit(false,  submit_CH1Istep_u, "0");
  dcsubmit(false,  submit_CH1Istep_d, "0");
  dcsubmit(false,  submit_CH2Vset,    "0");
  dcsubmit(false,  submit_CH2Iset,    "0");
  dcsubmit(false,  submit_CH2Out,     "0");
  dcsubmit(false,  submit_CH2Vstep,   "0");
  dcsubmit(false,  submit_CH2Vstep_u, "0");
  dcsubmit(false,  submit_CH2Vstep_d, "0");
  dcsubmit(false,  submit_CH2Istep,   "0");
  dcsubmit(false,  submit_CH2Istep_u, "0");
  dcsubmit(false,  submit_CH2Istep_d, "0");
  dcsubmit(false,  submit_CH3Vset,    "0");
  dcsubmit(false,  submit_CH3Iset,    "0");
  dcsubmit(false,  submit_CH3Out,     "0");
  dcsubmit(false,  submit_CH3Vstep,   "0");
  dcsubmit(false,  submit_CH3Vstep_u, "0");
  dcsubmit(false,  submit_CH3Vstep_d, "0");
  dcsubmit(false,  submit_CH3Istep,   "0");
  dcsubmit(false,  submit_CH3Istep_u, "0");
  dcsubmit(false,  submit_CH3Istep_d, "0");

}

void MainWindow::dcsubmit(bool enable, submitkey what, QString value){//提交任务管理器
   bool submitok=false;

    if(enable){

        //判断能够生效的指令
        if(
           what==submit_remote     ||
           what==submit_local      ||
           what==submit_ALLOut_ON  ||
           what==submit_ALLOut_OFF)
          {
           submitok=true;
          }
        if(DCCH1enable){
          if(
             what==submit_CH1Vset    ||
             what==submit_CH1Iset    ||
             what==submit_CH1Out     ||
             what==submit_CH1Vstep   ||
             what==submit_CH1Vstep_u ||
             what==submit_CH1Vstep_d ||
             what==submit_CH1Istep   ||
             what==submit_CH1Istep_u ||
             what==submit_CH1Istep_d
             ){
              submitok=true;
             }
        }
        if(DCCH2enable){
            if(
               what==submit_CH2Vset    ||
               what==submit_CH2Iset    ||
               what==submit_CH2Out     ||
               what==submit_CH2Vstep   ||
               what==submit_CH2Vstep_u ||
               what==submit_CH2Vstep_d ||
               what==submit_CH2Istep   ||
               what==submit_CH2Istep_u ||
               what==submit_CH2Istep_d
               ){
                submitok=true;
               }
        }
        if(DCCH3enable){
            if(
               what==submit_CH3Vset    ||
               what==submit_CH3Iset    ||
               what==submit_CH3Out     ||
               what==submit_CH3Vstep   ||
               what==submit_CH3Vstep_u ||
               what==submit_CH3Vstep_d ||
               what==submit_CH3Istep   ||
               what==submit_CH3Istep_u ||
               what==submit_CH3Istep_d
               ){
                submitok=true;
               }
        }
        if(DCSINGLECHANNEL){
            if(
               what==submit_CH1Vset    ||
               what==submit_CH1Iset    ||
               what==submit_CH1Vstep   ||
               what==submit_CH1Vstep_u ||
               what==submit_CH1Vstep_d ||
               what==submit_CH1Istep   ||
               what==submit_CH1Istep_u ||
               what==submit_CH1Istep_d
               ){
                submitok=true;
               }
        }

        if(!(submitok&&value!=NULL&&value!="")){
           submitok=false;
           DCshowmsg("set faled value");

        }

       if(submitok){
           if(what!=submit_local&&what!=submit_remote){
           DCSUBED=1;
           SUBEDNUM=what;
           }

       switch(what){
           case submit_CH1Out:
               ui->dc_ch1_sw->setStyleSheet(Style_submiting);
           break;
           case submit_CH2Out:
               ui->dc_ch2_sw->setStyleSheet(Style_submiting);
           break;
           case submit_CH3Out:
               ui->dc_ch3_sw->setStyleSheet(Style_submiting);
           break;

           case submit_CH1Vset:
               ui->dc_ch1_edit_vset->setStyleSheet(Style_submiting);
           break;
           case submit_CH2Vset:
               ui->dc_ch2_edit_vset->setStyleSheet(Style_submiting);
           break;
           case submit_CH3Vset:
               ui->dc_ch3_edit_vset->setStyleSheet(Style_submiting);
           break;

           case submit_CH1Iset:
               ui->dc_ch1_edit_iset->setStyleSheet(Style_submiting);
           break;
           case submit_CH2Iset:
               ui->dc_ch2_edit_iset->setStyleSheet(Style_submiting);
           break;
           case submit_CH3Iset:
               ui->dc_ch3_edit_iset->setStyleSheet(Style_submiting);
           break;

          case submit_CH1Vstep:
              ui->dc_ch1_edit_vstep->setStyleSheet(Style_submiting);
          break;
          case submit_CH2Vstep:
              ui->dc_ch2_edit_vstep->setStyleSheet(Style_submiting);
          break;
          case submit_CH3Vstep:
              ui->dc_ch3_edit_vstep->setStyleSheet(Style_submiting);
          break;

          case submit_CH1Vstep_u:
              ui->dc_ch1_vstep_up->setStyleSheet(Style_submiting);
          break;
          case submit_CH2Vstep_u:
              ui->dc_ch2_vstep_up->setStyleSheet(Style_submiting);
          break;
          case submit_CH3Vstep_u:
              ui->dc_ch3_vstep_up->setStyleSheet(Style_submiting);
          break;

          case submit_CH1Vstep_d:
              ui->dc_ch1_vstep_down->setStyleSheet(Style_submiting);
          break;
          case submit_CH2Vstep_d:
              ui->dc_ch2_vstep_down->setStyleSheet(Style_submiting);
          break;
          case submit_CH3Vstep_d:
              ui->dc_ch3_vstep_down->setStyleSheet(Style_submiting);
          break;

           case submit_CH1Istep:
               ui->dc_ch1_edit_istep->setStyleSheet(Style_submiting);
           break;
           case submit_CH2Istep:
               ui->dc_ch2_edit_istep->setStyleSheet(Style_submiting);
           break;
           case submit_CH3Istep:
               ui->dc_ch3_edit_istep->setStyleSheet(Style_submiting);
           break;

           case submit_CH1Istep_u:
               ui->dc_ch1_istep_up->setStyleSheet(Style_submiting);
           break;
           case submit_CH2Istep_u:
               ui->dc_ch2_istep_up->setStyleSheet(Style_submiting);
           break;
           case submit_CH3Istep_u:
               ui->dc_ch3_istep_up->setStyleSheet(Style_submiting);
           break;

           case submit_CH1Istep_d:
               ui->dc_ch1_istep_down->setStyleSheet(Style_submiting);
           break;
           case submit_CH2Istep_d:
               ui->dc_ch2_istep_down->setStyleSheet(Style_submiting);
           break;
           case submit_CH3Istep_d:
               ui->dc_ch3_istep_down->setStyleSheet(Style_submiting);
           break;

           case submit_remote:
               ui->dc_rl->setStyleSheet(Style_submiting);
           break;
           case submit_local:
               ui->dc_l->setStyleSheet(Style_submiting);
           break;

           case submit_ALLOut_ON:
               ui->dc_allon->setStyleSheet(Style_submiting);
           break;
           case submit_ALLOut_OFF:
               ui->dc_alloff->setStyleSheet(Style_submiting);
           break;
        }
    }
    }else{
        submitok=true;
        switch(what){
            case submit_CH1Out:
                ui->dc_ch1_sw->setStyleSheet(Style_submited);
            break;
            case submit_CH2Out:
                ui->dc_ch2_sw->setStyleSheet(Style_submited);
            break;
            case submit_CH3Out:
                ui->dc_ch3_sw->setStyleSheet(Style_submited);
            break;

            case submit_CH1Vset:
                ui->dc_ch1_edit_vset->setStyleSheet(Style_submited);
            break;
            case submit_CH2Vset:
                ui->dc_ch2_edit_vset->setStyleSheet(Style_submited);
            break;
            case submit_CH3Vset:
                ui->dc_ch3_edit_vset->setStyleSheet(Style_submited);
            break;

            case submit_CH1Iset:
                ui->dc_ch1_edit_iset->setStyleSheet(Style_submited);
            break;
            case submit_CH2Iset:
                ui->dc_ch2_edit_iset->setStyleSheet(Style_submited);
            break;
            case submit_CH3Iset:
                ui->dc_ch3_edit_iset->setStyleSheet(Style_submited);
            break;

           case submit_CH1Vstep:
               ui->dc_ch1_edit_vstep->setStyleSheet(Style_submited);
           break;
           case submit_CH2Vstep:
               ui->dc_ch2_edit_vstep->setStyleSheet(Style_submited);
           break;
           case submit_CH3Vstep:
               ui->dc_ch3_edit_vstep->setStyleSheet(Style_submited);
           break;

           case submit_CH1Vstep_u:
               ui->dc_ch1_vstep_up->setStyleSheet(Style_submited);
           break;
           case submit_CH2Vstep_u:
               ui->dc_ch2_vstep_up->setStyleSheet(Style_submited);
           break;
           case submit_CH3Vstep_u:
               ui->dc_ch3_vstep_up->setStyleSheet(Style_submited);
           break;

           case submit_CH1Vstep_d:
               ui->dc_ch1_vstep_down->setStyleSheet(Style_submited);
           break;
           case submit_CH2Vstep_d:
               ui->dc_ch2_vstep_down->setStyleSheet(Style_submited);
           break;
           case submit_CH3Vstep_d:
               ui->dc_ch3_vstep_down->setStyleSheet(Style_submited);
           break;

            case submit_CH1Istep:
                ui->dc_ch1_edit_istep->setStyleSheet(Style_submited);
            break;
            case submit_CH2Istep:
                ui->dc_ch2_edit_istep->setStyleSheet(Style_submited);
            break;
            case submit_CH3Istep:
                ui->dc_ch3_edit_istep->setStyleSheet(Style_submited);
            break;

            case submit_CH1Istep_u:
                ui->dc_ch1_istep_up->setStyleSheet(Style_submited);
            break;
            case submit_CH2Istep_u:
                ui->dc_ch2_istep_up->setStyleSheet(Style_submited);
            break;
            case submit_CH3Istep_u:
                ui->dc_ch3_istep_up->setStyleSheet(Style_submited);
            break;

            case submit_CH1Istep_d:
                ui->dc_ch1_istep_down->setStyleSheet(Style_submited);
            break;
            case submit_CH2Istep_d:
                ui->dc_ch2_istep_down->setStyleSheet(Style_submited);
            break;
            case submit_CH3Istep_d:
                ui->dc_ch3_istep_down->setStyleSheet(Style_submited);
            break;

            case submit_remote:
                ui->dc_rl->setStyleSheet(Style_submited);
            break;
            case submit_local:
                ui->dc_l->setStyleSheet(Style_submited);
            break;

            case submit_ALLOut_ON:
                ui->dc_allon->setStyleSheet(Style_submited);
            break;
            case submit_ALLOut_OFF:
                ui->dc_alloff->setStyleSheet(Style_submited);
            break;


         }


    }

    if(submitok){
        DCenablemap.insert(what,enable);
        DCvaluemap.insert(what,value.toDouble());
    }
}
void MainWindow::dosubmit(){

    int area=DCinwhatarea(refstep);
    if(DCenablemap.value(submit_remote     )){ DCconsend("SYSTem:REMote"                                                                   ); dcsubmit(false,submit_remote     , "0"); QThread::msleep(5);}
    if(DCenablemap.value(submit_local      )){ DCconsend("SYSTem:LOCal"                                                                    ); dcsubmit(false,submit_local      , "0"); QThread::msleep(5);}
    if(DCenablemap.value(submit_ALLOut_ON  )){ DCconsend("OUTPut ON"                                                                       ); dcsubmit(false,submit_ALLOut_ON  , "0"); QThread::msleep(5);}
    if(DCenablemap.value(submit_ALLOut_OFF )){ DCconsend("OUTPut OFF"                                                                      ); dcsubmit(false,submit_ALLOut_OFF , "0"); QThread::msleep(5);}
    if(area==1)if(DCenablemap.value(submit_CH1Vset    )){
        DCconsend(QString("VOLTage %1V"       ).arg(DCvaluemap.value(submit_CH1Vset    ))); dcsubmit(false,submit_CH1Vset    , "0"); QThread::msleep(5);}
    if(area==1)if(DCenablemap.value(submit_CH1Iset    )){ DCconsend(QString("CURR %1A"          ).arg(DCvaluemap.value(submit_CH1Iset    ))); dcsubmit(false,submit_CH1Iset    , "0"); QThread::msleep(5);}
    if(area==1)if(DCenablemap.value(submit_CH1Out     )){ DCconsend(QString("CHANnel:OUTPut %1" ).arg(DCvaluemap.value(submit_CH1Out     ))); dcsubmit(false,submit_CH1Out     , "0"); QThread::msleep(5);}
    if(area==1)if(DCenablemap.value(submit_CH1Vstep   )){ DCconsend(QString("VOLTage:STEP %1V"  ).arg(DCvaluemap.value(submit_CH1Vstep   ))); dcsubmit(false,submit_CH1Vstep   , "0"); QThread::msleep(5);}
    if(area==1)if(DCenablemap.value(submit_CH1Vstep_u )){ DCconsend(QString("VOLTage:UP"        ).arg(DCvaluemap.value(submit_CH1Vstep_u ))); dcsubmit(false,submit_CH1Vstep_u , "0"); QThread::msleep(5);}
    if(area==1)if(DCenablemap.value(submit_CH1Vstep_d )){ DCconsend(QString("VOLTage:DOWN"      ).arg(DCvaluemap.value(submit_CH1Vstep_d ))); dcsubmit(false,submit_CH1Vstep_d , "0"); QThread::msleep(5);}
    if(area==1)if(DCenablemap.value(submit_CH1Istep   )){ DCconsend(QString("CURRent:STEP %1A"  ).arg(DCvaluemap.value(submit_CH1Istep   ))); dcsubmit(false,submit_CH1Istep   , "0"); QThread::msleep(5);}
    if(area==1)if(DCenablemap.value(submit_CH1Istep_u )){ DCconsend(QString("CURRent:UP"        ).arg(DCvaluemap.value(submit_CH1Istep_u ))); dcsubmit(false,submit_CH1Istep_u , "0"); QThread::msleep(5);}
    if(area==1)if(DCenablemap.value(submit_CH1Istep_d )){ DCconsend(QString("CURRent:DOWN"      ).arg(DCvaluemap.value(submit_CH1Istep_d ))); dcsubmit(false,submit_CH1Istep_d , "0"); QThread::msleep(5);}
    if(area==2)if(DCenablemap.value(submit_CH2Vset    )){
        DCconsend(QString("VOLTage %1V"       ).arg(DCvaluemap.value(submit_CH2Vset    ))); dcsubmit(false,submit_CH2Vset    , "0"); QThread::msleep(5);}
    if(area==2)if(DCenablemap.value(submit_CH2Iset    )){ DCconsend(QString("CURR %1A"          ).arg(DCvaluemap.value(submit_CH2Iset    ))); dcsubmit(false,submit_CH2Iset    , "0"); QThread::msleep(5);}
    if(area==2)if(DCenablemap.value(submit_CH2Out     )){ DCconsend(QString("CHANnel:OUTPut %1" ).arg(DCvaluemap.value(submit_CH2Out     ))); dcsubmit(false,submit_CH2Out     , "0"); QThread::msleep(5);}
    if(area==2)if(DCenablemap.value(submit_CH2Vstep   )){ DCconsend(QString("VOLTage:STEP %1V"  ).arg(DCvaluemap.value(submit_CH2Vstep   ))); dcsubmit(false,submit_CH2Vstep   , "0"); QThread::msleep(5);}
    if(area==2)if(DCenablemap.value(submit_CH2Vstep_u )){ DCconsend(QString("VOLTage:UP"        ).arg(DCvaluemap.value(submit_CH2Vstep_u ))); dcsubmit(false,submit_CH2Vstep_u , "0"); QThread::msleep(5);}
    if(area==2)if(DCenablemap.value(submit_CH2Vstep_d )){ DCconsend(QString("VOLTage:DOWN"      ).arg(DCvaluemap.value(submit_CH2Vstep_d ))); dcsubmit(false,submit_CH2Vstep_d , "0"); QThread::msleep(5);}
    if(area==2)if(DCenablemap.value(submit_CH2Istep   )){ DCconsend(QString("CURRent:STEP %1A"  ).arg(DCvaluemap.value(submit_CH2Istep   ))); dcsubmit(false,submit_CH2Istep   , "0"); QThread::msleep(5);}
    if(area==2)if(DCenablemap.value(submit_CH2Istep_u )){ DCconsend(QString("CURRent:UP"        ).arg(DCvaluemap.value(submit_CH2Istep_u ))); dcsubmit(false,submit_CH2Istep_u , "0"); QThread::msleep(5);}
    if(area==2)if(DCenablemap.value(submit_CH2Istep_d )){ DCconsend(QString("CURRent:DOWN"      ).arg(DCvaluemap.value(submit_CH2Istep_d ))); dcsubmit(false,submit_CH2Istep_d , "0"); QThread::msleep(5);}
    if(area==3)if(DCenablemap.value(submit_CH3Vset    )){ DCconsend(QString("VOLTage %1V"       ).arg(DCvaluemap.value(submit_CH3Vset    ))); dcsubmit(false,submit_CH3Vset    , "0"); QThread::msleep(5);}
    if(area==3)if(DCenablemap.value(submit_CH3Iset    )){ DCconsend(QString("CURR %1A"          ).arg(DCvaluemap.value(submit_CH3Iset    ))); dcsubmit(false,submit_CH3Iset    , "0"); QThread::msleep(5);}
    if(area==3)if(DCenablemap.value(submit_CH3Out     )){ DCconsend(QString("CHANnel:OUTPut %1" ).arg(DCvaluemap.value(submit_CH3Out     ))); dcsubmit(false,submit_CH3Out     , "0"); QThread::msleep(5);}
    if(area==3)if(DCenablemap.value(submit_CH3Vstep   )){ DCconsend(QString("VOLTage:STEP %1V"  ).arg(DCvaluemap.value(submit_CH3Vstep   ))); dcsubmit(false,submit_CH3Vstep   , "0"); QThread::msleep(5);}
    if(area==3)if(DCenablemap.value(submit_CH3Vstep_u )){ DCconsend(QString("VOLTage:UP"        ).arg(DCvaluemap.value(submit_CH3Vstep_u ))); dcsubmit(false,submit_CH3Vstep_u , "0"); QThread::msleep(5);}
    if(area==3)if(DCenablemap.value(submit_CH3Vstep_d )){ DCconsend(QString("VOLTage:DOWN"      ).arg(DCvaluemap.value(submit_CH3Vstep_d ))); dcsubmit(false,submit_CH3Vstep_d , "0"); QThread::msleep(5);}
    if(area==3)if(DCenablemap.value(submit_CH3Istep   )){ DCconsend(QString("CURRent:STEP %1A"  ).arg(DCvaluemap.value(submit_CH3Istep   ))); dcsubmit(false,submit_CH3Istep   , "0"); QThread::msleep(5);}
    if(area==3)if(DCenablemap.value(submit_CH3Istep_u )){ DCconsend(QString("CURRent:UP"        ).arg(DCvaluemap.value(submit_CH3Istep_u ))); dcsubmit(false,submit_CH3Istep_u , "0"); QThread::msleep(5);}
    if(area==3)if(DCenablemap.value(submit_CH3Istep_d )){ DCconsend(QString("CURRent:DOWN"      ).arg(DCvaluemap.value(submit_CH3Istep_d ))); dcsubmit(false,submit_CH3Istep_d , "0"); QThread::msleep(5);}
    if(DCSUBED==1&&!DCenablemap.value(SUBEDNUM)){
        DCSUBED=2;
    }

}
void MainWindow::getnextrefstep(){
//    if(DCSINGLECHANNEL){
//        DCLASTCONNUM=9;
//    }
//    if(DCCH1enable){
//     DCLASTCONNUM=9;
//    }
//    if(DCCH2enable){
//     DCLASTCONNUM=18;
//    }
//    if(DCCH3enable){
//     DCLASTCONNUM=27;
//    }
    DCLASTCONNUM=30;
    refstep++;
    while(!DCstepisinarea(refstep)){
      refstep++;
      if(refstep>DCLASTCONNUM){
          refstep=0;           //0是版本信息必定存在
          break;
      }
    }
    if(DCSUBED==2){
        refstep=dcsubchakstep(SUBEDNUM);

        DCSUBED=0;
    }

}
int MainWindow::dcsubchakstep(submitkey what){
    switch (what) {
    case submit_CH1Out:
        return 1;
    case submit_CH2Out:
        return 10;
    case submit_CH3Out:
        return 19;

    case submit_CH1Vset:
        return 3;
    case submit_CH2Vset:
        return  12;
    case submit_CH3Vset:
        return  21;

    case submit_CH1Iset:
        return 5;
    case submit_CH2Iset:
        return 14;
    case submit_CH3Iset:
        return 23;

    case submit_CH1Vstep:
        return 4;
    case submit_CH2Vstep:
        return 13;
    case submit_CH3Vstep:
        return 22;

    case submit_CH1Vstep_u:
        return 3;
    case submit_CH2Vstep_u:
        return 12;
    case submit_CH3Vstep_u:
        return 21;

    case submit_CH1Vstep_d:
        return 3;
    case submit_CH2Vstep_d:
        return 12;
    case submit_CH3Vstep_d:
        return 21;

    case submit_CH1Istep:
        return 6;
    case submit_CH2Istep:
        return 15;
    case submit_CH3Istep:
        return 24;

    case submit_CH1Istep_u:
        return 5;
    case submit_CH2Istep_u:
        return 14;
    case submit_CH3Istep_u:
        return 23;

    case submit_CH1Istep_d:
        return 5;
    case submit_CH2Istep_d:
        return 14;
    case submit_CH3Istep_d:
        return 23;

    case submit_remote:
       return refstep;
    case submit_local:
       return refstep;

    case submit_ALLOut_ON:
          return 0;
    case submit_ALLOut_OFF:

          return 0;
    }
    return refstep;//那就照常查询


}
int MainWindow::DCinwhatarea(int step){
    if(DCCH1enable&&step>=1&&step<=9-3){
        return 1;
    }
    if(DCCH2enable&&step>=10&&step<=18-3){
        return 2;
    }
    if(DCCH3enable&&step>=19&&step<=27-3){
        return 3;
    }
    if(DCSINGLECHANNEL&&step>1&&step<=9-3){
        return 1;
    }

}
void MainWindow::DCrefrash(int step,QString msg){
    msg=msg.split("\n")[0];

    DCREADING=2;
    DCREADINGLOST=0;
    switch (step) {
    case 0:     //电源设备参数刷新
        if(msg=="0"){
           ui->dc_alloff->setStyleSheet(Style_switchon);
           ui->dc_allon->setStyleSheet("");
        }else if(msg=="1"){
            ui->dc_alloff->setStyleSheet("");
           ui->dc_allon->setStyleSheet(Style_switchon);
        }
        getnextrefstep();
        break;
    case 1:     //通道1开关

       if(msg=="0"){
          ui->dc_ch1_sw->setStyleSheet("");
          ui->dc_ch1_sw->setText("ON");
       }else{
          ui->dc_ch1_sw->setStyleSheet(Style_switchon);
          ui->dc_ch1_sw->setText("OFF");
       }
        getnextrefstep();
        break;
    case 2:                //通道1电压上限
        ui->dc_ch1_Vlimit->setText(QString("Vmax:%1V").arg(msg));
        getnextrefstep();
        break;
    case 3:                //通道1电压设定
        ui->dc_ch1_vset->setText(QString("Vset:%1V").arg(msg));
        MainWindow::syncdata_dc("dc_ch1_vset",QString("%1").arg(msg));
        getnextrefstep();  //通道1电压步进
        break;
    case 4:
        ui->dc_ch1_vstep->setText(QString("Vstep:%1V").arg(msg));
       getnextrefstep();
        break;
    case 5:                //电流设定
        ui->dc_ch1_iset->setText(QString("Iset:%1A").arg(msg));
         MainWindow::syncdata_dc("dc_ch1_iset",QString("%1").arg(msg));
        getnextrefstep();
        break;
    case 6:                //电流步进
       ui->dc_ch1_istep->setText(QString("Istep:%1A").arg(msg));
        getnextrefstep();
        break;
    case 7:               //通道1实际电压
       ui->dc_ch1_realV->setText(QString("%1V").arg(msg));
       getnextrefstep();
        break;
    case 8:               //通道1实际电流
        ui->dc_ch1_realI->setText(QString("%1A").arg(msg));
       getnextrefstep();
        break;
    case 9:
        ui->dc_ch1_realP->setText(QString("%1W").arg(msg));
        getnextrefstep();
        break;

    case 10:     //通道2开关

       if(msg=="0"){
          ui->dc_ch2_sw->setStyleSheet("");
          ui->dc_ch2_sw->setText("ON");
       }else{
          ui->dc_ch2_sw->setStyleSheet(Style_switchon);
          ui->dc_ch2_sw->setText("OFF");
       }
        getnextrefstep();
        break;
    case 11:                //通道1电压上限
        ui->dc_ch2_Vlimit->setText(QString("Vmax:%1V").arg(msg));
        getnextrefstep();
        break;
    case 12:                //通道1电压设定
        ui->dc_ch2_vset->setText(QString("Vset:%1V").arg(msg));
        MainWindow::syncdata_dc("dc_ch2_vset",QString("%1").arg(msg));
        getnextrefstep();  //通道1电压步进
        break;
    case 13:                //通道1电流设定
        ui->dc_ch2_vstep->setText(QString("Vstep:%1V").arg(msg));
       getnextrefstep();
        break;
    case 14:                //电流设定
        ui->dc_ch2_iset->setText(QString("Iset:%1A").arg(msg));
        MainWindow::syncdata_dc("dc_ch2_iset",QString("%1").arg(msg));
        getnextrefstep();
        break;
    case 15:                //电流步进
       ui->dc_ch2_istep->setText(QString("Istep:%1A").arg(msg));
        getnextrefstep();
        break;
    case 16:               //通道1实际电压
       ui->dc_ch2_realV->setText(QString("%1V").arg(msg));
       getnextrefstep();
        break;
    case 17:               //通道1实际电流
        ui->dc_ch2_realI->setText(QString("%1A").arg(msg));
       getnextrefstep();
        break;
    case 18:
        ui->dc_ch2_realP->setText(QString("%1W").arg(msg));
        getnextrefstep();
        break;

    case 19:     //通道3开关

       if(msg=="0"){
          ui->dc_ch3_sw->setStyleSheet("");
          ui->dc_ch3_sw->setText("ON");
       }else{
          ui->dc_ch3_sw->setStyleSheet(Style_switchon);
          ui->dc_ch3_sw->setText("OFF");
       }
        getnextrefstep();
        break;
    case 20:                //通道1电压上限
        ui->dc_ch3_Vlimit->setText(QString("Vmax:%1V").arg(msg));
        getnextrefstep();
        break;
    case 21:                //通道1电压设定
        ui->dc_ch3_vset->setText(QString("Vset:%1V").arg(msg));
         MainWindow::syncdata_dc("dc_ch3_vset",QString("%1").arg(msg));
        getnextrefstep();  //通道1电压步进
        break;
    case 22:                //通道1电流设定
        ui->dc_ch3_vstep->setText(QString("Vstep:%1V").arg(msg));
        getnextrefstep();
        break;
    case 23:                //电流设定
        ui->dc_ch3_iset->setText(QString("Iset:%1A").arg(msg));
         MainWindow::syncdata_dc("dc_ch3_iset",QString("%1").arg(msg));
        getnextrefstep();
        break;
    case 24:                //电流步进
       ui->dc_ch3_istep->setText(QString("Istep:%1A").arg(msg));
        getnextrefstep();
        break;
    case 25:               //通道1实际电压
       ui->dc_ch3_realV->setText(QString("%1V").arg(msg));
       getnextrefstep();
        break;
    case 26:               //通道1实际电流
        ui->dc_ch3_realI->setText(QString("%1A").arg(msg));
       getnextrefstep();
        break;
    case 27:
        ui->dc_ch3_realP->setText(QString("%1W").arg(msg));
        getnextrefstep();
        break;
    case 28://所有通道电压输出
        switch (msg.split(",").length()) {
        case 0:
        case 1://0和1都是单通道的现象 所有0不要break
            if(msg!=NULL&&msg!=""){
              ui->dc_ch1_realV->setText(QString("%1V").arg(msg));

            }
            break;
        case 2:
            if(msg!=NULL&&msg!=""){
              ui->dc_ch1_realV->setText(QString("%1V").arg(msg.split(",")[0]));

              ui->dc_ch2_realV->setText(QString("%1V").arg(msg.split(",")[1]));

            }
            break;
        case 3:
            if(msg!=NULL&&msg!=""){
              ui->dc_ch1_realV->setText(QString("%1V").arg(msg.split(",")[0]));
              ui->dc_ch2_realV->setText(QString("%1V").arg(msg.split(",")[1]));
              ui->dc_ch3_realV->setText(QString("%1V").arg(msg.split(",")[2]));
              MainWindow::syncdata_dc("dc_ch1_realV",QString("%1").arg(msg.split(",")[0]));
              MainWindow::syncdata_dc("dc_ch2_realV",QString("%1").arg(msg.split(",")[1]));
              MainWindow::syncdata_dc("dc_ch3_realV",QString("%1").arg(msg.split(",")[2]));
            }
            break;
        default:
            break;
        }
        getnextrefstep();
        break;
    case 29://所有通道电流输出
        switch (msg.split(",").length()) {
        case 0:
        case 1://0和1都是单通道的现象 所有0不要break
            if(msg!=NULL&&msg!=""){
              ui->dc_ch1_realI->setText(QString("%1A").arg(msg));
            }
            break;
        case 2:
            if(msg!=NULL&&msg!=""){
              ui->dc_ch1_realI->setText(QString("%1A").arg(msg.split(",")[0]));
              ui->dc_ch2_realI->setText(QString("%1A").arg(msg.split(",")[1]));
            }
            break;
        case 3:
            if(msg!=NULL&&msg!=""){
              ui->dc_ch1_realI->setText(QString("%1A").arg(msg.split(",")[0]));
              ui->dc_ch2_realI->setText(QString("%1A").arg(msg.split(",")[1]));
              ui->dc_ch3_realI->setText(QString("%1A").arg(msg.split(",")[2]));
              MainWindow::syncdata_dc("dc_ch1_realI",QString("%1").arg(msg.split(",")[0]));
              MainWindow::syncdata_dc("dc_ch2_realI",QString("%1").arg(msg.split(",")[1]));
              MainWindow::syncdata_dc("dc_ch3_realI",QString("%1").arg(msg.split(",")[2]));
            }
            break;
        default:
            break;
        }
        getnextrefstep();
        break;
    case 30://所有通道功率输出
        switch (msg.split(",").length()) {
        case 0:
        case 1://0和1都是单通道的现象 所有0不要break
            if(msg!=NULL&&msg!=""){
              ui->dc_ch1_realP->setText(QString("%1W").arg(msg.split(",")[0]));
            }
            break;
        case 2:
            if(msg!=NULL&&msg!=""){
              ui->dc_ch1_realP->setText(QString("%1W").arg(msg.split(",")[0]));
              ui->dc_ch2_realP->setText(QString("%1W").arg(msg.split(",")[1]));
            }
            break;
        case 3:
            if(msg!=NULL&&msg!=""){
              ui->dc_ch1_realP->setText(QString("%1W").arg(msg.split(",")[0]));
              ui->dc_ch2_realP->setText(QString("%1W").arg(msg.split(",")[1]));
              ui->dc_ch3_realP->setText(QString("%1W").arg(msg.split(",")[2]));
              MainWindow::syncdata_dc("dc_ch1_realP",QString("%1").arg(msg.split(",")[0]));
              MainWindow::syncdata_dc("dc_ch2_realP",QString("%1").arg(msg.split(",")[1]));
              MainWindow::syncdata_dc("dc_ch3_realP",QString("%1").arg(msg.split(",")[2]));
            }
            break;
        default:
            break;
        }
        getnextrefstep();
        break;
    default :
        getnextrefstep();
        break;
    }
    DCREADING=0;

}
bool MainWindow::DCstepisinarea(int step){
    if(DCCH1enable&&step>=1&&step<=9-3){
        return true;
    }
    if(DCCH2enable&&step>=10&&step<=18-3){
        return true;
    }
    if(DCCH3enable&&step>=19&&step<=27-3){
        return true;
    }
    if(DCSINGLECHANNEL&&step>1&&step<=9-3){
        return true;
    }
    if(step>=28&&step<=30){
        return true;

    }
    return false;
}
void MainWindow::DCrefrashCh(int step){

    if(DCstepisinarea(step)&&step<28){
   //1-9 1通道 10-18 2通道 19-27 3通道
    if(step>0){
        int chstep=step%9;

           switch (chstep){
               case 0:
                //   DCconsend("MEASure:POWer?");   //通道实际功率
               break;
               case 1:
               if(!DCSINGLECHANNEL)
               {
                   DCconsend(QString("INSTrument CH%1").arg((step/9)+1));      //选择通道1
                   QThread::msleep(5);
                   DCconsend("CHANnel:OUTPut?");  //通道开关状态
               }
               break;
               case 2:
                   DCconsend("VOLTage:LIMIT?");  //通道电压上限 once
               break;
               case 3:
                   DCconsend("VOLTage?");        //通道电压设定
               break;
               case 4:
                   DCconsend("VOLTage:STEP?");    //通道电压步进
               break;
               case 5:
                   DCconsend("CURR?");            //通道电流设定
               break;
               case 6:
                   DCconsend("CURRent:STEP?");    //通道电流步进
               break;
               case 7:
                //  DCconsend("MEAS:VOLT?");       //通道实际电压
               break;
               case 8:
                 //  DCconsend("MEASure:CURRent?"); //通道实际电流
               break;
           }

    }

    }
    dosubmit();


}
//第一次来是问 第二次来是获取问的答案
void MainWindow::dorefrashdc(int w){
          if(w==DOREFRASHDC&&ui->con2dc->text()=="DISCON"){
         if(DCREADING==0){
             switch (refstep){
                 case 0://读取总电源开关状态
                     DCconsend("OUTPut?");
                 break;
                 case 28: //所有通道实际输出电压
                 if(!DCSINGLECHANNEL){
                     DCconsend("MEASure:VOLTage:ALL?");
                 }else
                     DCconsend("MEAS:VOLT?");
                 break;
                 case 29: //所有通道实际输出电流
                 if(!DCSINGLECHANNEL){
                     DCconsend("MEASure:CURRent:ALL?");
                 }else
                     DCconsend("MEASure:CURRent?");
                 break;
                 case 30: //所有通道实际功率
                 if(!DCSINGLECHANNEL){
                     DCconsend("MEASure:POWer:ALL?");
                 }else
                     DCconsend("MEASure:POWer?");
                 break;
             }
             DCrefrashCh(refstep);
             DCREADING=1;
             DCREADINGLOST=0;

         }else{
             if(DCREADING!=2){
              DCREADINGLOST++;
             if(DCREADINGLOST>=20){
               DCREADING=0;//读取失败
               if(DCREADING!=2){
               getnextrefstep();
               }
             }
             }else{
               DCREADINGLOST=0;
             }
         }
          }

}
QString nowcon="";
void MainWindow::setseriallist(){
    QList<QString> newportlist;
//    QList<QextPortInfo> port=enumerator->getPorts();
//    foreach(const QextPortInfo &info,enumerator->getPorts())
//    {
//            newportlist.append(info.portName);

//    }
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
            newportlist.append(info.portName());

    }

   if(ui->con2dc->text()=="CON"){
    if(newportlist.count()!=ui->combobox_DCserialport->count()){
      ui->combobox_DCserialport->clear();
      ui->combobox_DCserialport->addItems(newportlist);
    }
   }else{

       if(!newportlist.contains(nowcon)){

           QObject::disconnect(DCserial, SIGNAL(Win_QextSerialPort::readyRead()), this,SLOT(DCserialRead()));
           //关闭串口
           //DCserial->close();
           DCserial->disconnect();
           nowcon="NULL";
           ui->con2dc->setText("CON");

           ui->widget_dcp->setStyleSheet(border_dcp_disconnected);


       }


   }

}

void MainWindow::on_con2dc_clicked()
{
    if(ui->con2dc->text()==tr("CON"))
    {

        //portSettings = { BAUD57600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 2 };
        //设置串口名
        DCserial=new Win_QextSerialPort(ui->combobox_DCserialport->currentText(),QextSerialBase::EventDriven);
        //打开串口
        if(!DCserial->open(QIODevice::ReadWrite))
        {
            QMessageBox::about(NULL, "msg", "can not open serial port！");
            return;
        }
        dcsubmit_clear();
        // DCserial= new Win_QextSerialPort(QextSerialPort::EventDriven,this);
        DCserial->setPortName(ui->combobox_DCserialport->currentText());
        //设置波特率
        DCserial->setBaudRate(BAUD115200);
        //设置数据位数
        DCserial->setDataBits(DATA_8);
        //设置奇偶校验
        DCserial->setParity(PAR_NONE);
        //设置停止位
        DCserial->setStopBits(STOP_1);
        //设置流控制
        DCserial->setFlowControl(FLOW_OFF);



        quitrefrashDC=0;
        refrashDC=new  heart(DOREFRASHDC);
        refrashDC->start();
        QObject::connect(refrashDC,SIGNAL(maindo(int)), this, SLOT(dorefrashdc(int)));
        QObject::connect(refrashDC,SIGNAL(maindo(int)), this, SLOT(DCserialRead(int)));

        ui->con2dc->setText("DISCON");
      // bool conok= QObject::connect(DCserial, SIGNAL(readyRead()), this,SLOT(DCserialRead()));
      //  connect(port, SIGNAL(readyRead()), this,SLOT(readLineData()));
        nowcon=ui->combobox_DCserialport->currentText();

        }
        else
        {
        // QObject::disconnect(DCserial, SIGNAL(Win_QextSerialPort::readyRead()), this,SLOT(DCserialRead()));
        // 关闭串口
        // DCserial->clear();
        dcsubmit_clear();
        DCserial->close();
        nowcon="NULL";
        quitrefrashDC=1;
        QObject::disconnect(refrashDC,SIGNAL(maindo(int)), this, SLOT(dorefrashdc(int)));
        ui->con2dc->setText("CON");
         ui->widget_dcp->setStyleSheet(border_dcp_disconnected);
        }

}
//DC发送
void MainWindow::DCconsend(QString conString){
    conString.append("\n");
    QByteArray conbyts=conString.toLatin1().data();
    if(ui->con2dc->text()==tr("DISCON")){
        DCserial->write(conbyts,conbyts.length());
    }else{
        DCshowmsg("not con");
    }

}
//DC接收
void MainWindow::DCserialRead(int w){
    if(ui->con2dc->text()=="DISCON"&&w==DOPollingread){
QByteArray DCback=DCserial->readAll();
if(DCback.length()>0){
    if(ui->widget_dcp->styleSheet()==border_dcp_disconnected){
        ui->widget_dcp->setStyleSheet(border_dcp_connected);
    }
    DCrefrash(refstep,QString(DCback));
}
//DCshowmsg(QString(DCback));
    }
}
void MainWindow::DCshowmsg(QString msg){
   QString smsg=ui->DCmsg->toPlainText();
   smsg.append(QString("%1\n").arg(msg));
   if(smsg.length()>1000){
   smsg.clear();
   }
   ui->DCmsg->setText(smsg);
}

void MainWindow::on_dc_ch_ch1_clicked()
{
    if(!DCSINGLECHANNEL){
        dcsubmit_clear();
    if(ui->dc_ch_ch1->styleSheet()!=Style_choiced)
    {
        choicech(1);
    }else{
        ui->dc_ch_ch1->setStyleSheet("");
        DCCH1enable=false;
    }

    }
}

void MainWindow::on_dc_ch1_sw_clicked()
{
     choicech(1);
    if(DCCH1enable){
        if(ui->dc_ch1_sw->text()=="ON"){
           dcsubmit(true,submit_CH1Out,"1");
        }else{
           dcsubmit(true,submit_CH1Out,"0");
        }
    }



}

void MainWindow::on_dc_ch_ch2_clicked()
{
    if(!DCSINGLECHANNEL){
        dcsubmit_clear();
    if(ui->dc_ch_ch2->styleSheet()!=Style_choiced)
    {
        choicech(2);
    }else{
        ui->dc_ch_ch2->setStyleSheet("");
        DCCH2enable=false;
    }
    }
}

void MainWindow::on_dc_ch2_sw_clicked()
{
    choicech(2);
    if(DCCH2enable){
        if(ui->dc_ch2_sw->text()=="ON"){
           dcsubmit(true,submit_CH2Out,"1");
        }else{
           dcsubmit(true,submit_CH2Out,"0");
        }
    }


}

void MainWindow::on_dc_ch_ch3_clicked()
{
    if(!DCSINGLECHANNEL){

    if(ui->dc_ch_ch3->styleSheet()!=Style_choiced)
    {
       choicech(3);
    }else{
        ui->dc_ch_ch3->setStyleSheet("");
        DCCH3enable=false;
    }
    }
}

void MainWindow::on_dc_ch3_sw_clicked()
{
     choicech(3);
    if(DCCH3enable){
        if(ui->dc_ch3_sw->text()=="ON"){
           dcsubmit(true,submit_CH3Out,"1");
        }else{
           dcsubmit(true,submit_CH3Out,"0");
        }


    }

}

void MainWindow::on_dc_rl_clicked()
{
     dcsubmit(true,submit_remote,"1");
}
void MainWindow::on_dc_l_clicked()
{
     dcsubmit(true,submit_local,"1");
}

void MainWindow::on_dc_singch_clicked()
{
     dcsubmit_clear();
     if(ui->dc_singch->styleSheet()!=Style_choiced)
     {
         ui->dc_singch->setStyleSheet(Style_choiced);
         DCSINGLECHANNEL=true;
         ui->dc_ch_ch1->setStyleSheet("");
         DCCH1enable=false;
         ui->dc_ch_ch2->setStyleSheet("");
         DCCH2enable=false;
         ui->dc_ch_ch3->setStyleSheet("");
         DCCH3enable=false;

         ui->dc_ch1_sw->setEnabled(false);
     }else{
         ui->dc_singch->setStyleSheet("");
         DCSINGLECHANNEL=false;

         ui->dc_ch1_sw->setEnabled(true);
     }
}
void MainWindow::choicech(int CH){
    if(!DCSINGLECHANNEL){

        switch (CH) {
        case 1:
            if(!DCCH1enable)
            dcsubmit_clear();
            ui->dc_ch_ch1->setStyleSheet(Style_choiced);
            DCCH1enable=true;
            ui->dc_singch->setStyleSheet("");
            ui->dc_ch_ch2->setStyleSheet("");
            ui->dc_ch_ch3->setStyleSheet("");
            DCSINGLECHANNEL=false;
            DCCH2enable=false;
            DCCH3enable=false;
            break;
        case 2:
            if(!DCCH2enable)
            dcsubmit_clear();
            ui->dc_ch_ch2->setStyleSheet(Style_choiced);
            DCCH2enable=true;
            ui->dc_singch->setStyleSheet("");
            ui->dc_ch_ch1->setStyleSheet("");
            ui->dc_ch_ch3->setStyleSheet("");
            DCSINGLECHANNEL=false;
            DCCH1enable=false;
            DCCH3enable=false;
            break;
        case 3:
            if(!DCCH3enable)
            dcsubmit_clear();
            ui->dc_ch_ch3->setStyleSheet(Style_choiced);
            DCCH3enable=true;
            ui->dc_singch->setStyleSheet("");
            ui->dc_ch_ch1->setStyleSheet("");
            ui->dc_ch_ch2->setStyleSheet("");
            DCSINGLECHANNEL=false;
            DCCH1enable=false;
            DCCH2enable=false;
            break;
        }

    }

}


void MainWindow::on_dc_ch1_vset_clicked()
{
    choicech(1);
    dcsubmit(true,submit_CH1Vset,ui->dc_ch1_edit_vset->text());

}

void MainWindow::on_dc_ch1_iset_clicked()
{
    choicech(1);
     dcsubmit(true,submit_CH1Iset,ui->dc_ch1_edit_iset->text());

}

void MainWindow::on_dc_ch2_vset_clicked()
{
    choicech(2);
    QString value=ui->dc_ch2_edit_vset->text();
     dcsubmit(true,submit_CH2Vset,value);
}

void MainWindow::on_dc_ch2_iset_clicked()
{    choicech(2);
     dcsubmit(true,submit_CH2Iset,ui->dc_ch2_edit_iset->text());
}

void MainWindow::on_dc_ch3_vset_clicked()
{
    choicech(3);
     dcsubmit(true,submit_CH3Vset,ui->dc_ch3_edit_vset->text());
}

void MainWindow::on_dc_ch3_iset_clicked()
{
    choicech(3);
     dcsubmit(true,submit_CH3Iset,ui->dc_ch3_edit_iset->text());
}

void MainWindow::on_dc_ch1_vstep_up_clicked()
{choicech(1);
     dcsubmit(true,submit_CH1Vstep_u,"1");
}

void MainWindow::on_dc_ch1_vstep_clicked()
{choicech(1);
     dcsubmit(true,submit_CH1Vstep,ui->dc_ch1_edit_vstep->text());
}

void MainWindow::on_dc_ch1_vstep_down_clicked()
{choicech(1);
     dcsubmit(true,submit_CH1Vstep_d,"1");
}


void MainWindow::on_dc_ch1_istep_up_clicked()
{choicech(1);
       dcsubmit(true,submit_CH1Istep_u,"1");
}

void MainWindow::on_dc_ch1_istep_clicked()
{choicech(1);
     dcsubmit(true,submit_CH1Istep,ui->dc_ch1_edit_istep->text());
}

void MainWindow::on_dc_ch1_istep_down_clicked()
{choicech(1);
     dcsubmit(true,submit_CH1Istep_d,"1");
}

void MainWindow::on_dc_ch2_vstep_up_clicked()
{choicech(2);
     dcsubmit(true,submit_CH2Vstep_u,"1");
}

void MainWindow::on_dc_ch2_vstep_clicked()
{choicech(2);
    dcsubmit(true,submit_CH2Vstep,ui->dc_ch2_edit_vstep->text());
}

void MainWindow::on_dc_ch2_vstep_down_clicked()
{choicech(2);
    dcsubmit(true,submit_CH2Vstep_d,"1");
}

void MainWindow::on_dc_ch2_istep_up_clicked()
{choicech(2);
    dcsubmit(true,submit_CH2Istep_u,"1");
}

void MainWindow::on_dc_ch2_istep_clicked()
{   choicech(2);
    dcsubmit(true,submit_CH2Istep,ui->dc_ch2_edit_istep->text());
}

void MainWindow::on_dc_ch2_istep_down_clicked()
{choicech(2);
    dcsubmit(true,submit_CH2Istep_d,"1");
}

void MainWindow::on_dc_ch3_vstep_up_clicked()
{choicech(3);
    dcsubmit(true,submit_CH3Vstep_u,"1");
}

void MainWindow::on_dc_ch3_vstep_clicked()
{  choicech(3);
    dcsubmit(true,submit_CH3Vstep,ui->dc_ch3_edit_vstep->text());
}

void MainWindow::on_dc_ch3_vstep_down_clicked()
{choicech(3);
    dcsubmit(true,submit_CH3Vstep_d,"1");
}

void MainWindow::on_dc_ch3_istep_up_clicked()
{choicech(3);
    dcsubmit(true,submit_CH3Istep_u,"1");
}

void MainWindow::on_dc_ch3_istep_clicked()
{choicech(3);
    dcsubmit(true,submit_CH3Istep,ui->dc_ch3_edit_istep->text());
}

void MainWindow::on_dc_ch3_istep_down_clicked()
{choicech(3);
    dcsubmit(true,submit_CH3Istep_d,"1");
}

void MainWindow::on_dc_allon_clicked()
{
 dcsubmit(true,submit_ALLOut_ON,"1");
}

void MainWindow::on_dc_alloff_clicked()
{
 dcsubmit(true,submit_ALLOut_OFF,"1");
}

void MainWindow::on_dc_ch1_edit_vset_returnPressed()
{
    on_dc_ch1_vset_clicked();
}

void MainWindow::on_dc_ch1_edit_iset_returnPressed()
{
    on_dc_ch1_iset_clicked();
}

void MainWindow::on_dc_ch2_edit_vset_returnPressed()
{
    on_dc_ch2_vset_clicked();
}

void MainWindow::on_dc_ch2_edit_iset_returnPressed()
{
    on_dc_ch2_iset_clicked();
}

void MainWindow::on_dc_ch3_edit_vset_returnPressed()
{
    on_dc_ch3_vset_clicked();
}

void MainWindow::on_dc_ch3_edit_iset_returnPressed()
{
    on_dc_ch3_iset_clicked();
}

void MainWindow::on_dc_ch1_edit_vstep_returnPressed()
{
    on_dc_ch1_vstep_clicked();
}

void MainWindow::on_dc_ch1_edit_istep_returnPressed()
{
    on_dc_ch1_istep_clicked();
}

void MainWindow::on_dc_ch2_edit_vstep_returnPressed()
{
    on_dc_ch2_vstep_clicked();
}


void MainWindow::on_dc_ch2_edit_istep_returnPressed()
{
    on_dc_ch2_istep_clicked();
}

void MainWindow::on_dc_ch3_edit_vstep_returnPressed()
{
    on_dc_ch3_vstep_clicked();
}

void MainWindow::on_dc_ch3_edit_istep_returnPressed()
{
    on_dc_ch3_istep_clicked();
}
