#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "powermeter.h"
//刷新率控制
//刷新控制
int MainWindow::ana_reftime=400;
int MainWindow::getanareftime(){
    return MainWindow::ana_reftime;
}
void MainWindow::setana_reftime(int reftime){
    MainWindow::ana_reftime=reftime;

}


viclass *anavi;
heart *ana_queryheart;
int ana_initing=0,ana_initflag=0; //在初始化刷新的时候不能调用实时刷新 否则会冲突 再刷新的时候不能再调用刷新
QString Style_ana_on="background-color: rgb(85, 255, 0);";
QString ana_trace_color1="<font color=\"#F4EE50\">";
QString ana_trace_color2="<font color=\"#95F7E8\">";
QString ana_trace_color3="<font color=\"#FCACC9\">";
QString ana_trace_color4="<font color=\"#ACFFEB\">";
QString ana_trace_color5="<font color=\"#E0B46D\">";
QString ana_trace_color6="<font color=\"#B56182\">";
QString ana_trace_color7="<font color=\"#A1FFFF\">";
QString ana_trace_color8="<font color=\"#C67B9A\">";
QString ana_trace_color9="<font color=\"#D6FFD3\">";
enum anastindex{
   ANA_PAGE_MARKER= 0,
   ANA_PAGE_MEAS,
   ANA_PAGE_FORMAT,
   ANA_PAGE_SCALE,
   ANA_PAGE_DISPLAY,
   ANA_PAGE_AVG,
   ANA_PAGE_CAL,
   ANA_PAGE_FREQ,
};
void MainWindow::ana_init(){
    anavi=new viclass();
    ana_queryheart=new heart(DOANA);
    QObject::connect(ana_queryheart,SIGNAL(maindoana(int)), this, SLOT(dorefrashana(int)));
    ana_queryheart->start();
    //datainit

}

void MainWindow::ana_showmsg(QString msg){
    ui->ana_msg->setText(msg);
}
QString getshowline(int tracenum,QString context,QString mode){
    QString show="";
    switch (tracenum) {
    case 1:
        show.append( QString("%1%2").arg(ana_trace_color1).arg(context));
        break;
    case 2:
        show.append( QString("%1%2").arg(ana_trace_color2).arg(context));
         break;
    case 3:
        show.append( QString("%1%2").arg(ana_trace_color3).arg(context));
         break;
    case 4:
        show.append( QString("%1%2").arg(ana_trace_color4).arg(context));
         break;
    case 5:
        show.append( QString("%1%2").arg(ana_trace_color5).arg(context));
         break;
    case 6:
        show.append( QString("%1%2").arg(ana_trace_color6).arg(context));
         break;
    case 7:
        show.append( QString("%1%2").arg(ana_trace_color7).arg(context));
         break;
    case 8:
        show.append( QString("%1%2").arg(ana_trace_color8).arg(context));
         break;
    case 9:
        show.append( QString("%1%2").arg(ana_trace_color9).arg(context));
         break;
    default:

         break;
    }
    if(mode=="MLOG"){
    show.append(" dB");
    }
    if(mode=="PHAS"){
    show.append(" °");
    }
    if(mode=="GDEL"){
    show.append(" ps");
    }

    show.append("</font><br>");
    return show;
    }
void sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}
//同步转异步操作
//void MainWindow::doreadana(int w,QString askfor,QString value){

//    if(w==DOANA){
//    if(askfor==":CALC1:PAR:COUN?\n"){

//    }

//    }
//}



void MainWindow::ana_init_freq()
{
    //起始频率
    QString askfor=QString("SENSe:FREQuency:STAR?\n");
    char * backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));
    if(backdata!=NULL){
     QString freqstart= unit_freq(vigetdouble(QString(QLatin1String(backdata))));
     ui->ana_freqstart->setText(QString("START:%1").arg(freqstart));
     ui->ana_value_freq_start->setText(QString("START:%1").arg(freqstart));
     MainWindow::syncdata_ana("ana_freqstart",QString(QLatin1String(backdata)));
    }
    //带宽
     askfor=QString(":SENSe:FREQuency:SPAN?\n");
     backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));
    if(backdata!=NULL){
     QString freqspan= unit_freq(vigetdouble(QString(QLatin1String(backdata))));
     ui->ana_freqspan->setText(QString("SPAN:%1").arg(freqspan));
     ui->ana_value_span->setText(QString("SPAN:%1").arg(freqspan));
     MainWindow::syncdata_ana("ana_freqspan",QString(QLatin1String(backdata)));
    }
    //中心频率
    askfor=QString("SENS:FREQ:CENT?\n");
    backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));
    if(backdata!=NULL){
     QString freqcenter= unit_freq(vigetdouble(QString(QLatin1String(backdata))));
     ui->ana_freqcenter->setText(QString("CENTER:%1").arg(freqcenter));
     ui->ana_value_freq_center->setText(QString("CENTER:%1").arg(freqcenter));
     MainWindow::syncdata_ana("ana_freqcenter",QString(QLatin1String(backdata)));
    }
    //截止频率

    askfor=QString("SENS:FREQ:STOP?\n");
    backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));
    if(backdata!=NULL){
     QString freqstop= unit_freq(vigetdouble(QString(QLatin1String(backdata))));
     ui->ana_freqstop->setText(QString("STOP:%1").arg(freqstop));
     ui->ana_value_freq_stop->setText(QString("STOP:%1").arg(freqstop));
     MainWindow::syncdata_ana("ana_freqstop",QString(QLatin1String(backdata)));
    }

}
void MainWindow::ana_conuiinit(){
    //tr1 S11 MLOG ####Mhz ######
    if(anavi->vilife()){
       //1读取显示在屏幕上的trace数
       char * backdata=anavi->vimyread(anavi->vipInstrHandle,":CALC1:PAR:COUN?\n");//次数可能有异常
      if(backdata!=NULL){
       int anatracenum=QString(QLatin1String(backdata)).toInt();
       if(anatracenum<10&&anatracenum>0)//支持9种模式
       {
          setana_showtracenum(anatracenum); //把值存储起来后面操作使用
          ui->ana_tracenum->setText(QString("TRACENUM:%1").arg(anatracenum));
          //初始化刷新项下
          if(ana_initing==1){
              ana_init_freq();
          }

          //实时刷新项下
          QString showcontext="";
          for(int i=1;i<=anatracenum;i++){
              QString askfor="";

              if(ana_initing==1){
                  //2读取每个显示的trace的模式 最终每个marker的对应值以追后的trace 为准 如果显示的trace模式相同的话
                  askfor=QString(":CALC:PAR%1:DEF?\n").arg(i); //S11.....
                  backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));
                  if(backdata!=NULL){
                      QString tracemode=QString(QLatin1String(backdata));
                      ana_meas_map.insert(i,tracemode);
                      //2.5读取每个trace的监听模式 MLOG
                      askfor=QString("CALCulate:TRACe%1:FORMat?\n").arg(i); //MLOG Phas.....
                      if((backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor)))!=NULL)
                      {
                          QString  tracelismode=QString(QLatin1String(backdata));
                          ana_tracemode_map.insert(i,tracelismode);
                      }else return;
                  }else return;

             }
              //3读取marker的状态
              for(int j=1;j<=9;j++){
                   QString showline="";
                   QPushButton *mkerbt =this->findChild<QPushButton*>(QString("ana_maker%1sw").arg(j));
                   if(ana_initing==1){//获取每个maker的状态
                       askfor=QString("CALCulate:MARKer%1:STATe?\n").arg(j);
                       backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));
                       if(backdata!=NULL){
                           QString anamst1=QString(QLatin1String(backdata));
                           if(anamst1=="1"){
                               mkerbt->setStyleSheet(Style_ana_on);
                            }else{
                               mkerbt->setStyleSheet("");
                            }
                       }else{return;}
                   }
                   if(mkerbt->styleSheet()==Style_ana_on){
                       //4读取开启的marker 在对应trace中的响应值
                       askfor=QString("CALCulate:TRACe%1:MARKer%2:DATA?\n").arg(i).arg(j);
                       backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));
                       //5解析并显示
                       if(backdata!=NULL){
                       //"+8.89104516919E-001,+0.00000000000E+000,+4.74000000000E+008"
                       QString anamnf=QString(QLatin1String(backdata));
                                 if(anamnf.split(",").size()==3){
                                  QString mmvalue=anamnf.split(",")[0];
                                  QString mfreq=anamnf.split(",")[2];
                                  if(getana_nowtrace()==i){
                                     showline.append("△");
                                  }
                                  showline.append(
                                      QString("Tr%1 Mk%2 %3 %4 %5 %6").arg(i)
                                                                      .arg(j)
                                                                      .arg(ana_meas_map.value(i))
                                                                      .arg(ana_tracemode_map.value(i))
                                                                      .arg(unit_freq(mfreq.toDouble()))
                                                                      .arg(vigetdouble(mmvalue))
                                                     );
                                  MainWindow::syncdata_ana(QString("Tr%1->MK%2").arg(i).arg(j),QString("%1").arg(vigetdouble(mmvalue)));
                                 // ana_showmarkerlist(j,mmvalue,tracemode);
                              if(showline!=""){
                                  showcontext.append(getshowline(i,showline,ana_tracemode_map.value(i)));
                                  }
                                  }
                         }else{return;}
                 }
             }

       }

       if(ana_initing==1){
          ana_initing=0;
       }

       if(showcontext!="")
              ui->ana_trtabletext->setText(showcontext);

              //ana_conuiinit();//回调刷新
       }

       anavi->visaouttime=0;
       }else{
            anavi->visaouttime++;
        if(anavi->visaouttime>2){
            anavi->visaouttime=0;
            anavi->viclose();
            ana_showmsg("con2 powersource outtime");
            ui->con2ana->setText("CON");
            return;
        }
       }



    }


}

void MainWindow::ana_choice_meas(QString ch){
    if(ch=="S11"){
       ui->ana_meas_s11->setStyleSheet(Style_ana_on);
       ui->ana_meas_s12->setStyleSheet("");
       ui->ana_meas_s21->setStyleSheet("");
       ui->ana_meas_s22->setStyleSheet("");
    }
    if(ch=="S12"){
       ui->ana_meas_s12->setStyleSheet(Style_ana_on);
       ui->ana_meas_s11->setStyleSheet("");
       ui->ana_meas_s21->setStyleSheet("");
       ui->ana_meas_s22->setStyleSheet("");
    }
    if(ch=="S21"){
       ui->ana_meas_s21->setStyleSheet(Style_ana_on);
       ui->ana_meas_s12->setStyleSheet("");
       ui->ana_meas_s11->setStyleSheet("");
       ui->ana_meas_s22->setStyleSheet("");
    }
    if(ch=="S22"){
       ui->ana_meas_s22->setStyleSheet(Style_ana_on);
       ui->ana_meas_s12->setStyleSheet("");
       ui->ana_meas_s21->setStyleSheet("");
       ui->ana_meas_s11->setStyleSheet("");
    }


}
void MainWindow::setana_nowtrace(int nowtrace){
    QString askfor=QString("CALC:PAR%1:SEL\n").arg(nowtrace);
    if(anavi->viconsend(anavi-> vipInstrHandle ,QString2char(askfor))==0){//这里可能会使程序卡顿 注意必须加\n
        ana_nowtrace=nowtrace;

        askfor=QString(":CALC:PAR%1:DEF?\n").arg(getana_nowtrace());      //获取当前trace的meas
         char * backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));//次数可能有异常
         if(backdata!=NULL){
            QString trmeas= QString(QLatin1String(backdata));
            if(trmeas=="S11"||trmeas=="S12"||trmeas=="S21"||trmeas=="S22"){
             ana_meas_map.insert(getana_nowtrace(),trmeas);
             ui->ana_trace_meas->setText(QString("now Tr%1 %2").arg(getana_nowtrace()).arg(ana_meas_map.value(getana_nowtrace())));
             ana_choice_meas(trmeas);
            }
         }
         //获取当前trace的监听模式 MLOG
          askfor=QString("CALCulate:TRACe%1:FORMat?\n").arg(getana_nowtrace()); //MLOG Phas.....
          QString tracelismode="";
          if((backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor)))!=NULL)
          {
              tracelismode=QString(QLatin1String(backdata));

          }
          if(tracelismode=="MLOG"||tracelismode=="PHAS"||tracelismode=="SWR"){
              ana_tracemode_map.insert(getana_nowtrace(),tracelismode);
              ui->ana_fomat_combox->setCurrentText(tracelismode);
          }

}
}
void MainWindow::setana_showtracenum(int tracenum){
     ana_showtracenum=tracenum;
}
void MainWindow::ana_showmarkerlist(int markernumber, QString value, QString mode){
}

void MainWindow::dorefrashana(int w){
    if(w==DOANA){
        if(ana_initing==1){
        ana_initflag=0;
        }
        ana_initflag++;
        if(ana_initflag>10){//延时*10 刷新一次
            ana_initing=1;//全体刷新
            ana_initflag=0;
        }
       ana_conuiinit();
    }
}
void MainWindow::on_con2ana_clicked()
{

if(ui->con2ana->text()=="CON"){
        if(ui->ana_edit_addr->text()!=NULL&&ui->ana_edit_addr->text()!=""){
            if(anavi->viconopen(ui->ana_edit_addr->text())==0){
               ui->con2ana->setText("DISCON");
                //连上后选择当前通道为trace1
                 setana_nowtrace(1);
                 ana_initing=1;
                 ana_conuiinit();
            }else{
              ana_showmsg("cannot con2ana");
              ui->con2ana->setText("CON");
            }
        }
    }else{
         anavi->viclose();
         ui->con2ana->setText("CON");
    }
}

void MainWindow::on_ana_edit_addr_returnPressed()
{
    on_con2ana_clicked();
}




void MainWindow::on_ana_channelprev_clicked()
{

}

void MainWindow::on_ana_channelnext_clicked()
{

}

void MainWindow::on_ana_traceprev_clicked()
{
    if(getana_nowtrace()>1){
        setana_nowtrace(getana_nowtrace()-1);
    }else{
        //否则设置为最后一个
        setana_nowtrace(getana_showtracenum());
    }
}

void MainWindow::on_ana_tracenext_clicked()
{
    if(getana_nowtrace()<getana_showtracenum()){
        setana_nowtrace(getana_nowtrace()+1);
    }else{
        //否则设置为第一个
        setana_nowtrace(1);
    }

}

void MainWindow::on_ana_channelmax_clicked()
{

}

void MainWindow::on_ana_tracemax_clicked()
{

}

void MainWindow::on_ana_meas_clicked()
{
ui->ana_stackedWidget->setCurrentIndex(ANA_PAGE_MEAS);
}

void MainWindow::on_ana_format_clicked()
{


}

void MainWindow::on_ana_scale_clicked()
{

}

void MainWindow::on_ana_display_clicked()
{
      ui->ana_stackedWidget->setCurrentIndex(ANA_PAGE_DISPLAY);
      ui->ana_edit_numoftrace->setFocus();
}

void MainWindow::on_ana_avg_clicked()
{

}

void MainWindow::on_ana_cal_clicked()
{

}
void MainWindow::on_ana_start_clicked()
{
     ui->ana_stackedWidget->setCurrentIndex(ANA_PAGE_FREQ);
       ui->ana_edit_freq_start->setFocus();

}
void MainWindow::on_ana_stop_clicked()
{
     ui->ana_stackedWidget->setCurrentIndex(ANA_PAGE_FREQ);
       ui->ana_edit_freq_stop->setFocus();
}

void MainWindow::on_ana_center_clicked()
{
     ui->ana_stackedWidget->setCurrentIndex(ANA_PAGE_FREQ);
       ui->ana_edit_freq_center->setFocus();
}

void MainWindow::on_ana_span_clicked()
{
     ui->ana_stackedWidget->setCurrentIndex(ANA_PAGE_FREQ);
     ui->ana_edit_span->setFocus();
}
void MainWindow::on_ana_edit_freq_start_returnPressed()
{
    if(anavi->vilife()){
        QString as=ui->ana_edit_freq_start->text();
        if(as!=""&&as!=NULL)
        {
            if(as.toDouble()>0){

              QString askfor=QString("SENSe:FREQuency:STAR %1\n").arg(getnumfreq(as,ui->ana_freq_unit->currentText()));
              if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){

                  ana_init_freq();

              }

            }

        }

    }
}

void MainWindow::on_ana_edit_span_returnPressed()
{
    if(anavi->vilife()){
        QString as=ui->ana_edit_span->text();
        if(as!=""&&as!=NULL)
        {
            if(as.toDouble()>0||as=="0"){
                QString fqs=getnumfreq(as,ui->ana_freq_unit->currentText());
                QString askfor;
                if(fqs!=NULL){
                     askfor=QString(":SENSe:FREQuency:SPAN %1\n").arg(fqs);

                }else if(as=="0"){
                     askfor=QString(":SENSe:FREQuency:SPAN 0\n");
                }
               if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){

                   ana_init_freq();


               }

        }

        }

    }
}

void MainWindow::on_ana_edit_freq_center_returnPressed()
{
    if(anavi->vilife()){
        QString as=ui->ana_edit_freq_center->text();
        if(as!=""&&as!=NULL)
        {
            if(as.toDouble()>0){

              QString askfor=QString("SENS:FREQ:CENT %1\n").arg(getnumfreq(as,ui->ana_freq_unit->currentText()));
              if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){

                  ana_init_freq();
              }

            }

        }

    }
}

void MainWindow::on_ana_edit_freq_stop_returnPressed()
{
    if(anavi->vilife()){
        QString as=ui->ana_edit_freq_stop->text();
        if(as!=""&&as!=NULL)
        {
            if(as.toDouble()>0){

              QString askfor=QString("SENS:FREQ:STOP %1\n").arg(getnumfreq(as,ui->ana_freq_unit->currentText()));
              if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){

                  ana_init_freq();

              }

            }

        }

    }

}


void MainWindow::on_ana_sweepsetup_clicked()
{

}

void MainWindow::on_ana_trigger_clicked()
{

}

void MainWindow::on_ana_marker_clicked()
{
   ui->ana_stackedWidget->setCurrentIndex(ANA_PAGE_MARKER);
}

void MainWindow::on_ana_markersearch_clicked()
{

}

void MainWindow::on_ana_markerfctn_clicked()
{

}

void MainWindow::on_ana_analysis_clicked()
{

}
void MainWindow::on_ana_edit_numoftrace_returnPressed()
{
    if(anavi->vilife()){
        if(ui->ana_edit_numoftrace->text()!=""&&ui->ana_edit_numoftrace->text()!=NULL){
           int tracenumset= ui->ana_edit_numoftrace->text().toInt();
           if(tracenumset>0&&tracenumset<10){
               if(getana_showtracenum()!=tracenumset){
                 QString askfor=QString(":CALC:PAR:COUN %1\n").arg(tracenumset);
                 if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){
                     char * backdata=anavi->vimyread(anavi->vipInstrHandle,":CALC1:PAR:COUN?\n");//次数可能有异常
                     if(backdata!=NULL){
                         int anatracenum=QString(QLatin1String(backdata)).toInt();
                         ui->ana_tracenum->setText(QString("TRACENUM:%1").arg(anatracenum));
                         setana_showtracenum(anatracenum);
                     }
                 }



               }

           }

        }
    }
}

//mease篇
void MainWindow::on_ana_meas_s11_clicked()
{
    if(anavi->vilife()){
        QString askfor=QString(":CALC:PAR%1:DEF S11\n").arg(getana_nowtrace());
        if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){
           QString askfor=QString(":CALC:PAR%1:DEF?\n").arg(getana_nowtrace());
            char * backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));//次数可能有异常
            if(backdata!=NULL){
               QString trmeas= QString(QLatin1String(backdata));
               if(trmeas=="S11"||trmeas=="S12"||trmeas=="S21"||trmeas=="S22"){
                ana_meas_map.insert(getana_nowtrace(),trmeas);
                ui->ana_trace_meas->setText(QString("now Tr%1 %2").arg(getana_nowtrace()).arg(ana_meas_map.value(getana_nowtrace())));
                ana_choice_meas(trmeas);
               }
            }

        }
    }
}

void MainWindow::on_ana_meas_s12_clicked()
{
    if(anavi->vilife()){
        QString askfor=QString(":CALC:PAR%1:DEF S12\n").arg(getana_nowtrace());
        if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){
           QString askfor=QString(":CALC:PAR%1:DEF?\n").arg(getana_nowtrace());
            char * backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));//次数可能有异常
            if(backdata!=NULL){
               QString trmeas= QString(QLatin1String(backdata));
               if(trmeas=="S11"||trmeas=="S12"||trmeas=="S21"||trmeas=="S22"){
                ana_meas_map.insert(getana_nowtrace(),trmeas);
                ui->ana_trace_meas->setText(QString("now Tr%1 %2").arg(getana_nowtrace()).arg(ana_meas_map.value(getana_nowtrace())));
                ana_choice_meas(trmeas);
               }
            }

        }
    }
}

void MainWindow::on_ana_meas_s21_clicked()
{
    if(anavi->vilife()){
        QString askfor=QString(":CALC:PAR%1:DEF S21\n").arg(getana_nowtrace());
        if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){

           QString askfor=QString(":CALC:PAR%1:DEF?\n").arg(getana_nowtrace());
            char * backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));//次数可能有异常
            if(backdata!=NULL){
               QString trmeas= QString(QLatin1String(backdata));
               if(trmeas=="S11"||trmeas=="S12"||trmeas=="S21"||trmeas=="S22"){
                ana_meas_map.insert(getana_nowtrace(),trmeas);
                ui->ana_trace_meas->setText(QString("now Tr%1 %2").arg(getana_nowtrace()).arg(ana_meas_map.value(getana_nowtrace())));
                ana_choice_meas(trmeas);
               }
            }

        }
    }
}

void MainWindow::on_ana_meas_s22_clicked()
{
    if(anavi->vilife()){
        QString askfor=QString(":CALC:PAR%1:DEF S22\n").arg(getana_nowtrace());
        if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){

            QString askfor=QString(":CALC:PAR%1:DEF?\n").arg(getana_nowtrace());
            char * backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));//次数可能有异常
            if(backdata!=NULL){
               QString trmeas= QString(QLatin1String(backdata));
               if(trmeas=="S11"||trmeas=="S12"||trmeas=="S21"||trmeas=="S22"){
                ana_meas_map.insert(getana_nowtrace(),trmeas);
                ui->ana_trace_meas->setText(QString("now Tr%1 %2").arg(getana_nowtrace()).arg(ana_meas_map.value(getana_nowtrace())));
                ana_choice_meas(trmeas);
               }
            }

        }
    }
}

//marker 篇
void MainWindow::ana_marker_sw(int mknum,int swvalue){
    if(anavi->vilife()){

        QPushButton *mksw =this->findChild<QPushButton*>(QString("ana_maker%1sw").arg(mknum));
        QString askfor=QString("CALCulate:MARKer%1:STATe %2\n").arg(mknum).arg(swvalue);
        if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){

            askfor=QString("CALCulate:MARKer%1:STAT?\n").arg(mknum);
            char * backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor));
            if(backdata!=NULL){
            QString mkst= QString(QLatin1String(backdata));
            if(mkst=="1"){
              mksw->setStyleSheet(Style_ana_on);
            }else if(mkst=="0"){
              mksw->setStyleSheet("");
            }

            }

        }
    }

}
void MainWindow::on_ana_maker1sw_clicked()
{
    int sw=0;
    if(ui->ana_maker1sw->styleSheet()==Style_ana_on){
    sw=0;
    }else{
    sw=1;
    }
    ana_marker_sw(1,sw);
}


void MainWindow::on_ana_maker2sw_clicked()
{
    int sw=0;
    if(ui->ana_maker2sw->styleSheet()==Style_ana_on){
    sw=0;
    }else{
    sw=1;
    }
    ana_marker_sw(2,sw);

}

void MainWindow::on_ana_maker3sw_clicked()
{
    int sw=0;
    if(ui->ana_maker3sw->styleSheet()==Style_ana_on){
    sw=0;
    }else{
    sw=1;
    }
    ana_marker_sw(3,sw);
}

void MainWindow::on_ana_maker4sw_clicked()
{
    int sw=0;
    if(ui->ana_maker4sw->styleSheet()==Style_ana_on){
    sw=0;
    }else{
    sw=1;
    }
    ana_marker_sw(4,sw);
}

void MainWindow::on_ana_maker5sw_clicked()
{
    int sw=0;
    if(ui->ana_maker5sw->styleSheet()==Style_ana_on){
    sw=0;
    }else{
    sw=1;
    }
    ana_marker_sw(5,sw);
}

void MainWindow::on_ana_maker6sw_clicked()
{
    int sw=0;
    if(ui->ana_maker6sw->styleSheet()==Style_ana_on){
    sw=0;
    }else{
    sw=1;
    }
    ana_marker_sw(6,sw);
}

void MainWindow::on_ana_maker7sw_clicked()
{
    int sw=0;
    if(ui->ana_maker7sw->styleSheet()==Style_ana_on){
    sw=0;
    }else{
    sw=1;
    }
    ana_marker_sw(7,sw);
}

void MainWindow::on_ana_maker8sw_clicked()
{
    int sw=0;
    if(ui->ana_maker8sw->styleSheet()==Style_ana_on){
    sw=0;
    }else{
    sw=1;
    }
    ana_marker_sw(8,sw);
}

void MainWindow::on_ana_maker9sw_clicked()
{
    int sw=0;
    if(ui->ana_maker9sw->styleSheet()==Style_ana_on){
    sw=0;
    }else{
    sw=1;
    }
    ana_marker_sw(9,sw);
}
void MainWindow::ana_marker_freqset(int mknum, QString freqnumvalue)
{
    if(anavi->vilife()){

        QString askfor=QString("CALCulate:MARKer%1:X %2\n").arg(mknum).arg(freqnumvalue);
        if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){
            ana_showmsg(QString("MK%1 FREQ%2 OK!").arg(mknum).arg(freqnumvalue));
        }
    }

}
void MainWindow::on_ana_marker1_freqedit_returnPressed()
{
    QString mfvs=ui->ana_marker1_freqedit->text();
    if(mfvs!=NULL&&mfvs!=""){
        ana_marker_freqset(1,getnumfreq(mfvs,ui->ana_freq_unit->currentText()));
    }


}


void MainWindow::on_ana_marker2_freqedit_returnPressed()
{
    QString mfvs=ui->ana_marker2_freqedit->text();
    if(mfvs!=NULL&&mfvs!=""){
        ana_marker_freqset(2,getnumfreq(mfvs,ui->ana_freq_unit->currentText()));
    }

}

void MainWindow::on_ana_marker3_freqedit_returnPressed()
{
    QString mfvs=ui->ana_marker3_freqedit->text();
    if(mfvs!=NULL&&mfvs!=""){
        ana_marker_freqset(3,getnumfreq(mfvs,ui->ana_freq_unit->currentText()));
    }
}

void MainWindow::on_ana_marker4_freqedit_returnPressed()
{
    QString mfvs=ui->ana_marker4_freqedit->text();
    if(mfvs!=NULL&&mfvs!=""){
        ana_marker_freqset(4,getnumfreq(mfvs,ui->ana_freq_unit->currentText()));
    }
}

void MainWindow::on_ana_marker5_freqedit_returnPressed()
{
    QString mfvs=ui->ana_marker5_freqedit->text();
    if(mfvs!=NULL&&mfvs!=""){
        ana_marker_freqset(5,getnumfreq(mfvs,ui->ana_freq_unit->currentText()));
    }
}

void MainWindow::on_ana_marker6_freqedit_returnPressed()
{
    QString mfvs=ui->ana_marker6_freqedit->text();
    if(mfvs!=NULL&&mfvs!=""){
        ana_marker_freqset(6,getnumfreq(mfvs,ui->ana_freq_unit->currentText()));
    }
}

void MainWindow::on_ana_marker7_freqedit_returnPressed()
{
    QString mfvs=ui->ana_marker7_freqedit->text();
    if(mfvs!=NULL&&mfvs!=""){
        ana_marker_freqset(7,getnumfreq(mfvs,ui->ana_freq_unit->currentText()));
    }
}

void MainWindow::on_ana_marker8_freqedit_returnPressed()
{
    QString mfvs=ui->ana_marker8_freqedit->text();
    if(mfvs!=NULL&&mfvs!=""){
        ana_marker_freqset(8,getnumfreq(mfvs,ui->ana_freq_unit->currentText()));
    }
}

void MainWindow::on_ana_marker9_freqedit_returnPressed()
{
    QString mfvs=ui->ana_marker9_freqedit->text();
    if(mfvs!=NULL&&mfvs!=""){
        ana_marker_freqset(9,getnumfreq(mfvs,ui->ana_freq_unit->currentText()));
    }
}


void MainWindow::on_ana_fomat_combox_currentTextChanged(const QString &arg1)
{
    if(anavi->vilife()){
        //获取当前trace的监听模式 MLOG
        QString askfor=QString("CALCulate:TRACe%1:FORMat %2\n").arg(getana_nowtrace()).arg(arg1);
        if(anavi->viconsend(anavi->vipInstrHandle,QString2char(askfor))==0){
             askfor=QString("CALCulate:TRACe%1:FORMat?\n").arg(getana_nowtrace()); //MLOG Phas.....
             QString tracelismode="";
             char* backdata;
             if((backdata=anavi->vimyread(anavi->vipInstrHandle,QString2char(askfor)))!=NULL)
             {
                 tracelismode=QString(QLatin1String(backdata));

             }
             if(tracelismode=="MLOG"||tracelismode=="PHAS"||tracelismode=="SWR"){
                 ana_tracemode_map.insert(getana_nowtrace(),tracelismode);
                 ui->ana_fomat_combox->setCurrentText(tracelismode);
             }
        }
    }
}
//控制刷新率
void MainWindow::on_ana_edit_reftime_returnPressed()
{
    if(ui->ana_edit_reftime->text().toInt()>50){
      setana_reftime(ui->ana_edit_reftime->text().toInt());
      ui->ana_reftime->setText(QString("RT:%1ms").arg(getanareftime()));
    }
}
