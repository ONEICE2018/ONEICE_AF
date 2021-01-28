#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "powermeter.h"

int MainWindow::pm_reftime=300;//用于控制刷新率
 int MainWindow::getpmreftime(){
     return MainWindow::pm_reftime;
}
 void MainWindow::setpm_reftime(int reftime){
     if(reftime>=80){
        MainWindow::pm_reftime=reftime;
     }
}

void MainWindow::on_edit_pm_rt_returnPressed()
{
    if(ui->edit_pm_rt->text().toDouble()>20){
       setpm_reftime(ui->edit_pm_rt->text().toDouble());
       ui->pm_reftime->setText(QString("RT:%1ms").arg(getpmreftime()));
    }


}
 int MainWindow::PM_CON=0;
 /* Visa Session*/
 ViStatus  nRetStatus;
 ViSession rmSession = 0;
 ViSession pInstrHandle;
 heart *pm_queryheart;
 char wrBuff[MAX_SCPI_LEN];
 char rdBuff[MAX_SCPI_LEN];
 int  visaouttime=0;
 int  stm_ask_power=0;
 enum unitchanal
 {
     GHz=0,
     MHz,
     KHz
 };
double nowfreq=0;
QString Style_unitchoiced="background-color: rgba(30, 103, 13, 100);";
QString Style_pmconneted="background-color: rgb(255, 255, 255);";
QString Style_pmdisconneted="background-color: rgb(88, 88, 88);";
int MainWindow::nowchanal=MHz;
void MainWindow::pm_init(){
   pm_queryheart=new heart(DOPOWERMETER);
   QObject::connect(pm_queryheart,SIGNAL(maindopm(int)), this, SLOT(dorefrashpm(int)));
   pm_queryheart->start();
   pm_choicech(nowchanal);
}
void MainWindow::dorefrashpm(int w){
    if(w==DOPOWERMETER){
        if(PM_CON==1){
            nRetStatus = viQueryf( pInstrHandle, "FETC:POW:AC?\n", "%s", rdBuff);
            if(nRetStatus==0){
            QString uper=QString(QLatin1String(rdBuff));
            double  upervalue= QString(QString(uper.split("\\")[0]).split("E")[0]).toDouble();
            double xs= QString(QString(uper.split("\\")[0]).split("E")[1]).toDouble();
            if(upervalue>0){
                for(int i=0;i<abs(xs);i++){
                    if(xs>=0){
                        upervalue=upervalue*10;
                    }else{
                        upervalue=upervalue/10;
                    }
                }
            }else{
                upervalue=0;
            }
            ui->pm_upervalue->setText(QString("%1").arg(upervalue));
            MainWindow::syncdata_pm("pm_upervalue",QString("%1").arg(upervalue));
           // if(curve_fitting::powerasked==1)
            {
               //触发功率刷新
                emit cfrepower(DOREFSTMPOWER);
            }
            visaouttime=0;
            }else{
                visaouttime++;
                if(visaouttime>10){
                    visaouttime=0;//连接超时
                    ui->con2powermeter->setText("CON");
                    ui->pm_screen->setStyleSheet(Style_pmdisconneted);
                    pm_showmsg("connect kesight OVERtime ");
                    viClose( pInstrHandle);
                    viClose( rmSession);
                    PM_CON=0;
                }
              //  if(curve_fitting::powerasked==1)
                //{
               //     curve_fitting::powerasked=0;
               //     emit cfrepower(DOREFSTMPOWER);
               //  触发刷新并反馈失败

              //}

            }

            nRetStatus = viQueryf( pInstrHandle, "FETC2:POW:AC?\n", "%s", rdBuff);
            if(nRetStatus==0){
            QString lower=QString(QLatin1String(rdBuff));
            double  lowervalue= QString(QString(lower.split("\\")[0]).split("E")[0]).toDouble();
            double xs= QString(QString(lower.split("\\")[0]).split("E")[1]).toDouble();
    if(lowervalue>0){
                for(int i=0;i<abs(xs);i++){
                     if(xs>=0){
                    lowervalue=lowervalue*10;
                     }else{
                         lowervalue=lowervalue/10;
                     }
                }
    }else{
    lowervalue=0;
    }
            ui->pm_lowervalue->setText(QString("%1").arg(lowervalue));
             MainWindow::syncdata_pm("pm_lowervalue",QString("%1").arg(lowervalue));
            }
        }
    }
}
void MainWindow::on_con2powermeter_clicked()
{
        if(ui->con2powermeter->text()=="CON"){
        if(ui->pm_edit_addr->text()!=NULL&&ui->pm_edit_addr->text()!=""){
            /* open device and get visa Handle */
            nRetStatus = viOpenDefaultRM( &rmSession);
            QString getaddr=ui->pm_edit_addr->text();
            char*  addr;

            QByteArray ba = getaddr.toLatin1(); // must
            addr=ba.data();

            nRetStatus = viOpen( rmSession,addr , VI_NULL, VI_NULL, &pInstrHandle);
          if(nRetStatus!=0){
              PM_CON=0;
          pm_showmsg("cannot connect kesight ");
          }else{

            /* set visa Format */
            nRetStatus = viSetAttribute( pInstrHandle, VI_ATTR_TMO_VALUE, DEFAULT_TMO);
            nRetStatus = viSetAttribute( pInstrHandle, VI_ATTR_SUPPRESS_END_EN, VI_FALSE);
            nRetStatus = viSetAttribute( pInstrHandle, VI_ATTR_SEND_END_EN, VI_FALSE);
            PM_CON=1;
            pm_conuiinit();
            ui->con2powermeter->setText("DISCON");
            ui->pm_screen->setStyleSheet(Style_pmconneted);
              }
        }else{
           pm_showmsg("pm_addr edit bad");
        }

        }else{
            ui->con2powermeter->setText("CON");
            ui->pm_screen->setStyleSheet(Style_pmdisconneted);
            pm_showmsg("connect kesight over ");
            viClose( pInstrHandle);
            viClose( rmSession);
            PM_CON=0;
        }
}
void MainWindow::pm_showmsg(QString msg){
  ui->pm_msg->setText(msg);

}
void MainWindow::on_pm_uperunit_currentTextChanged(const QString &arg1)
{
    if(PM_CON==1){
        if(arg1=="DBM"){
         nRetStatus = viPrintf(pInstrHandle, "UNIT1:POWer DBM\n");
        }
        if(arg1=="W"){
         nRetStatus = viPrintf(pInstrHandle, "UNIT1:POWer W\n");
        }
        if(nRetStatus==0){
            nRetStatus = viQueryf( pInstrHandle, "UNIT1:POWer?\n", "%s", rdBuff);
            if(nRetStatus==0){
            QString uperunit=QString(QLatin1String(rdBuff));
            ui->pm_uperunit->setCurrentText(uperunit);
            }
        }
    }

}
void MainWindow::on_pm_lowerunit_currentTextChanged(const QString &arg1)
{
    if(PM_CON==1){
        if(arg1=="DBM"){
         nRetStatus = viPrintf(pInstrHandle, "UNIT2:POWer DBM\n");
        }
        if(arg1=="W"){
         nRetStatus = viPrintf(pInstrHandle, "UNIT2:POWer W\n");
        }
        if(nRetStatus==0){
            nRetStatus = viQueryf( pInstrHandle, "UNIT2:POWer?\n", "%s", rdBuff);
            if(nRetStatus==0){
            QString lowerunit=QString(QLatin1String(rdBuff));
            ui->pm_lowerunit->setCurrentText(lowerunit);
            }
        }
    }
}
void MainWindow::pm_choicech(int ch){
        nowchanal=ch;
        ui->pm_ch_ghz->setStyleSheet("");
        ui->pm_ch_mhz->setStyleSheet("");
        ui->pm_ch_khz->setStyleSheet("");
        switch (ch) {
        case GHz:
              ui->pm_ch_ghz->setStyleSheet(Style_unitchoiced);
            break;
        case MHz:
              ui->pm_ch_mhz->setStyleSheet(Style_unitchoiced);
            break;
        case KHz:
              ui->pm_ch_khz->setStyleSheet(Style_unitchoiced);
            break;


        }

}
void MainWindow::on_pm_ch_ghz_clicked()
{
    pm_choicech(GHz);
   on_pm_eidt_freq_returnPressed();
}

void MainWindow::on_pm_ch_mhz_clicked()
{
    pm_choicech(MHz);
 on_pm_eidt_freq_returnPressed();
}

void MainWindow::on_pm_ch_khz_clicked()
{
    pm_choicech(KHz);
  on_pm_eidt_freq_returnPressed();
}


QString MainWindow::returnchmsg(int ch){
    switch (ch) {
    case GHz:
           return "GHz";
    case MHz:
           return "MHz";
    case KHz:
           return "KHz";
    default :
       pm_choicech(MHz);
       return returnchmsg(nowchanal);
    }
}
void MainWindow::on_pm_eidt_freq_returnPressed()
{
 if(ui->pm_eidt_freq->text()!=NULL&&ui->pm_eidt_freq->text()!=""&&ui->pm_eidt_freq->text().toDouble()>0){
     pm_setfreq(ui->pm_eidt_freq->text().toDouble(),returnchmsg(nowchanal));
 }
}

//void MainWindow::on_pm_freq_down_clicked()
//{
//    if(ui->pm_freqstep->text()!=NULL&&ui->pm_freqstep->text()!=""&&ui->pm_freqstep->text().toDouble()>0){
//        if(nowfreq>ui->pm_freqstep->text().toDouble()){
//            pm_setfreq(nowfreq-ui->pm_freqstep->text().toDouble());
//        }
//    }
//}

//void MainWindow::on_pm_freq_up_clicked()
//{
//    if(ui->pm_freqstep->text()!=NULL&&ui->pm_freqstep->text()!=""&&ui->pm_freqstep->text().toDouble()>0){

//            pm_setfreq(nowfreq+ui->pm_freqstep->text().toDouble());

//    }
//}
void MainWindow::pm_setfreq(double freq,QString unit){
    if(PM_CON==1){
if(freq>0&&nowfreq!=freq){
     QString freqset=  QString("FREQuency %1%2\n").arg(freq).arg(unit);
     char*  ch;
     QByteArray ba = freqset.toLatin1(); // must
     ch=ba.data();
     nRetStatus = viPrintf(pInstrHandle, ch);
    }
    pm_conuiinit();
}

}
void MainWindow::on_pm_edit_addr_returnPressed()
{
    on_con2powermeter_clicked();
}
void MainWindow::pm_conuiinit(){
    if(PM_CON==1){
    nRetStatus = viQueryf( pInstrHandle, "UNIT1:POWer?\n", "%s", rdBuff);
    if(nRetStatus==0){
    QString uperunit=QString(QLatin1String(rdBuff));
    ui->pm_uperunit->setCurrentText(uperunit);
    }
    nRetStatus = viQueryf( pInstrHandle, "UNIT2:POWer?\n", "%s", rdBuff);
    if(nRetStatus==0){
    QString lowerunit=QString(QLatin1String(rdBuff));
    ui->pm_lowerunit->setCurrentText(lowerunit);
    }
    nRetStatus = viQueryf( pInstrHandle, "SENS1:CORR:GAIN2:STATe?\n", "%s", rdBuff);
    if(nRetStatus==0){
    QString offsetstat=QString(QLatin1String(rdBuff));
    if(offsetstat.toDouble()==1){
        ui->pm_offsetsw->setStyleSheet(Style_unitchoiced);
    }else{
        ui->pm_offsetsw->setStyleSheet("");
    }
    }
    nRetStatus = viQueryf( pInstrHandle, "SENS1:CORR:GAIN2?\n", "%s", rdBuff);
    if(nRetStatus==0){
    QString offset=QString(QLatin1String(rdBuff));
    double  offsetvalue= QString(QString(offset.split("\\")[0]).split("E")[0]).toDouble();
    double xs= QString(QString(offset.split("\\")[0]).split("E")[1]).toDouble();
    if(offsetvalue>0){
                for(int i=0;i<abs(xs);i++){
                     if(xs>=0){
                    offsetvalue=offsetvalue*10;
                     }else{
                         offsetvalue=offsetvalue/10;
                     }
                }
    }else{
    offsetvalue=0;
    }
    ui->pm_offsetvalue->setText(QString("OFFSET:%1db").arg(offsetvalue));
    MainWindow::syncdata_pm("pm_offsetvalue",QString("%1").arg(offsetvalue));
    }

    nRetStatus = viQueryf( pInstrHandle, "FREQuency?\n", "%s", rdBuff);
    if(nRetStatus==0){
    QString FREQ=QString(QLatin1String(rdBuff));
    double  Freqvalue= QString(QString(FREQ.split("\\")[0]).split("E")[0]).toDouble();
    double xs= QString(QString(FREQ.split("\\")[0]).split("E")[1]).toDouble();
    if(Freqvalue>0){
                for(int i=0;i<abs(xs);i++){
                     if(xs>=0){
                    Freqvalue=Freqvalue*10;
                     }else{
                         Freqvalue=Freqvalue/10;
                     }
                }
    }else{
        Freqvalue=0;
    }
    if(Freqvalue>=1000000000){
        ui->pm_freqvalue->setText(QString("FREQ:%1GHz").arg(nowfreq=Freqvalue/1000000000.0));

    }else if(Freqvalue>=1000000){
        ui->pm_freqvalue->setText(QString("FREQ:%1MHz").arg(nowfreq=Freqvalue/1000000.0));

    }else if(Freqvalue>=1000){
        ui->pm_freqvalue->setText(QString("FREQ:%1KHz").arg(nowfreq=Freqvalue/1000.0));

    }else{
        ui->pm_freqvalue->setText(QString("FREQ:%1Hz").arg(nowfreq=Freqvalue/1.0));
    }
    MainWindow::syncdata_pm("pm_freqvalue",QString("%1").arg(nowfreq));
    }

    }
}
void MainWindow::on_pm_offsetsw_clicked()
{
    if(ui->pm_offsetsw->styleSheet()==Style_unitchoiced){
       nRetStatus = viPrintf(pInstrHandle, "SENS1:CORR:GAIN2:STATe 0\n");
    }else{
       nRetStatus = viPrintf(pInstrHandle, "SENS1:CORR:GAIN2:STATe 1\n");
    }
    pm_conuiinit();
}
void MainWindow::on_pm_edit_offset_returnPressed()
{
    if(ui->pm_edit_offset->text()!=NULL&&ui->pm_edit_offset->text()!=""&&ui->pm_edit_offset->text().toDouble()>0){
        QString offsetset=  QString("SENS1:CORR:GAIN2 %1dB\n").arg(ui->pm_edit_offset->text().toDouble());
        char*  ch;
        QByteArray ba = offsetset.toLatin1(); // must
        ch=ba.data();
        nRetStatus = viPrintf(pInstrHandle, ch);
        pm_conuiinit();
    }
}
