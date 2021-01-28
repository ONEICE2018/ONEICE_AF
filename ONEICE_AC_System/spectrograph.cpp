#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "powermeter.h"
viclass *spvi;
heart *sp_queryheart;
QString Style_sp_on="background-color: rgb(85, 255, 0);";
QString Style_spconneted="background-color: rgb(255, 255, 255);";
QString Style_spdisconneted="background-color: rgb(88, 88, 88);";
void MainWindow::sp_init(){
    spvi=new viclass();
    sp_queryheart=new heart(DOSPECTROGRAPH);
    QObject::connect(sp_queryheart,SIGNAL(maindosp(int)), this, SLOT(dorefrashsp(int)));
    sp_queryheart->start();

}

void MainWindow::sp_conuiinit(){
    if(spvi->vilife()){
       char * backdata=spvi->vimyread(spvi->vipInstrHandle,"CALCulate:MARKer1:STATe?\n");
       if(backdata!=NULL){
           QString spmst1=QString(QLatin1String(backdata));
           if(spmst1=="1"){
             ui->sp_marker_1->setStyleSheet(Style_sp_on);
             //marker1 FREQ
              backdata=spvi->vimyread(spvi->vipInstrHandle,"CALCulate:MARKer1:X?\n");
              if(backdata!=NULL){
              QString spm1freq=QString(QLatin1String(backdata));
              double  Freqvalue= QString(QString(spm1freq.split("\\")[0]).split("E")[0]).toDouble();
              double  xs= QString(QString(spm1freq.split("\\")[0]).split("E")[1]).toDouble();
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
                  ui->sp_m1_freqvalue->setText(QString("FREQ:%1GHz").arg(Freqvalue/1000000000.0));

              }else if(Freqvalue>=1000000){
                  ui->sp_m1_freqvalue->setText(QString("FREQ:%1MHz").arg(Freqvalue/1000000.0));

              }else if(Freqvalue>=1000){
                  ui->sp_m1_freqvalue->setText(QString("FREQ:%1KHz").arg(Freqvalue/1000.0));

              }else{
                  ui->sp_m1_freqvalue->setText(QString("FREQ:%1Hz").arg(Freqvalue/1.0));
              }
              MainWindow::syncdata_sp("sp_m1_freqvalue",QString("%1").arg(Freqvalue));
              }else return;
             //marker1 Yvalue
              backdata=spvi->vimyread(spvi->vipInstrHandle,"CALC:MARK1:Y?\n");
              if(backdata!=NULL){
              QString spm1y=QString(QLatin1String(backdata));
              ui->sp_marker_value_1->setText(QString("%1:%2").arg(vigetdouble(spm1y)).arg(ui->sp_unit->currentText()));
              MainWindow::syncdata_sp("sp_marker_value_1",QString(QLatin1String(backdata)));
              }else return;
              //marker1 mode
              backdata=spvi->vimyread(spvi->vipInstrHandle,"CALCulate:MARKer1:MODE?\n");
              if(backdata!=NULL){
                  QString spm1mode=QString(QLatin1String(backdata));
                  ui->sp_marker_mode_1->setCurrentText(spm1mode);
              }else return;

           }else{
             ui->sp_marker_1->setStyleSheet("");
           }

           for(int i=2;i<=12;i++){
               QString askfor=QString("CALCulate:MARKer%1:STATe?\n").arg(i);
               backdata=spvi->vimyread(spvi->vipInstrHandle,QString2char(askfor));
               if(backdata!=NULL){
                   if(QString(QLatin1String(backdata))=="1"){
                       QLabel *mkertx;
                       QPushButton *mkerbt =this->findChild<QPushButton*>(QString("sp_marker_%1").arg(i));
                       mkerbt->setStyleSheet(Style_sp_on);
//                       //markeri FREQ
                       QString askfor=QString("CALCulate:MARKer%1:X?\n").arg(i);
                       backdata=spvi->vimyread(spvi->vipInstrHandle,QString2char(askfor));
                       if(backdata!=NULL){
                       QString spmifreq=QString(QLatin1String(backdata));
                       double  Freqvalue= QString(QString(spmifreq.split("\\")[0]).split("E")[0]).toDouble();
                       double  xs= QString(QString(spmifreq.split("\\")[0]).split("E")[1]).toDouble();
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

                        mkertx =this->findChild<QLabel*>(QString("sp_m%1_freqvalue").arg(i));
                        if(Freqvalue>=1000000000){
                            mkertx->setText(QString("FREQ:%1GHz").arg(Freqvalue/1000000000.0));

                        }else if(Freqvalue>=1000000){
                            mkertx->setText(QString("FREQ:%1MHz").arg(Freqvalue/1000000.0));

                        }else if(Freqvalue>=1000){
                            mkertx->setText(QString("FREQ:%1KHz").arg(Freqvalue/1000.0));

                        }else{
                            mkertx->setText(QString("FREQ:%1Hz").arg(Freqvalue/1.0));
                        }
                        MainWindow::syncdata_sp(QString("sp_m%1_freqvalue").arg(i),QString("%1").arg(Freqvalue));
                       }else return;
                       //markeri Yvalue

                       askfor=QString("CALC:MARK%1:Y?\n").arg(i);
                       backdata=spvi->vimyread(spvi->vipInstrHandle,QString2char(askfor));
                       if(backdata!=NULL){
                       mkertx =this->findChild<QLabel*>(QString("sp_marker_value_%1").arg(i));
                       QString spmiy=QString(QLatin1String(backdata));
                       mkertx->setText(QString("%1:%2").arg(vigetdouble(spmiy)).arg(ui->sp_unit->currentText()));
                       MainWindow::syncdata_sp(QString("sp_marker_value_%1").arg(i),QString(QLatin1String(backdata)));
                       }else return;

                       //markeri mode
                       askfor=QString("CALCulate:MARKer%1:MODE?\n").arg(i);
                       backdata=spvi->vimyread(spvi->vipInstrHandle,QString2char(askfor));
                       if(backdata!=NULL){
                           QComboBox *mkerun =this->findChild<QComboBox*>(QString("sp_marker_mode_%1").arg(i));
                           QString spmimode=QString(QLatin1String(backdata));
                           mkerun->setCurrentText(spmimode);
                       }else return;



                   }else{
                       QPushButton *mkerbt =this->findChild<QPushButton*>(QString("sp_marker_%1").arg(i));
                       mkerbt->setStyleSheet("");
                   }

               }else return;


           }

//           //单位
           backdata=spvi->vimyread(spvi->vipInstrHandle,"UNIT:POWer?\n");
           if(backdata!=NULL){
            ui->sp_unit->setCurrentText(QString(QLatin1String(backdata)));
           }else return;
          //显示衰减量
           backdata=spvi->vimyread(spvi->vipInstrHandle,"DISPlay:WINDow:TRACe:Y:RLEVel?\n");
           if(backdata!=NULL){
            ui->sp_refleve->setText(QString("REFLEVE:%1%2").arg(QString(QLatin1String(backdata)).toDouble()).arg(ui->sp_unit->currentText()));
            MainWindow::syncdata_sp("sp_refleve",QString(QLatin1String(backdata)));
           }else return;
           //设置衰减
           backdata=spvi->vimyread(spvi->vipInstrHandle,"DISPlay:WINDow:TRACe:Y:RLEVel:OFFSet?\n");
           if(backdata!=NULL){
            ui->sp_offset->setText(QString("OFFSET:%1dB").arg(QString(QLatin1String(backdata)).toDouble()));
            MainWindow::syncdata_sp("sp_offset",QString(QLatin1String(backdata)));
           }else return;
           //freqstart
           QString askfor=QString("FREQuency:STARt?\n");
           backdata=spvi->vimyread(spvi->vipInstrHandle,QString2char(askfor));
           if(backdata!=NULL){
           QString spmifreq=QString(QLatin1String(backdata));
           double Freqvalue=vigetdouble(spmifreq);
           QString uf=unit_freq(Freqvalue);
           ui->sp_freq_start->setText(QString("FREQ_START:%1").arg(uf));
           MainWindow::syncdata_sp("sp_freq_start",QString(QLatin1String(backdata)));
           }else return;
           //freqcentre
           askfor=QString("FREQuency:CENTer?\n");
           backdata=spvi->vimyread(spvi->vipInstrHandle,QString2char(askfor));
           if(backdata!=NULL){
           QString spmifreq=QString(QLatin1String(backdata));
           double Freqvalue=vigetdouble(spmifreq);
           QString uf=unit_freq(Freqvalue);
           ui->sp_freq_center->setText(QString("FREQ_CENTRE:%1").arg(uf));
           MainWindow::syncdata_sp("sp_freq_center",QString(QLatin1String(backdata)));
           }else return;
           //freqstop
           askfor=QString("FREQuency:STOP?\n");
           backdata=spvi->vimyread(spvi->vipInstrHandle,QString2char(askfor));
           if(backdata!=NULL){
           QString spmifreq=QString(QLatin1String(backdata));
           double Freqvalue=vigetdouble(spmifreq);
           QString uf=unit_freq(Freqvalue);
           ui->sp_freq_stop->setText(QString("FREQ_STOP:%1").arg(uf));
           MainWindow::syncdata_sp("sp_freq_stop",QString(QLatin1String(backdata)));
           }else return;
           //span
           askfor=QString("FREQuency:SPAN?\n");
           backdata=spvi->vimyread(spvi->vipInstrHandle,QString2char(askfor));
           if(backdata!=NULL){
           QString spmifreq=QString(QLatin1String(backdata));
           double Freqvalue=vigetdouble(spmifreq);
           QString uf=unit_freq(Freqvalue);
           ui->sp_span->setText(QString("FREQ_SPAN:%1").arg(uf));
           MainWindow::syncdata_sp("sp_span",QString(QLatin1String(backdata)));
           }else return;
           //triger mode
           backdata=spvi->vimyread(spvi->vipInstrHandle,"TRIGger:SOURce?\n");
           if(backdata!=NULL){
            ui->sp_triger_mode->setCurrentText(QString(QLatin1String(backdata)));
           }else return;
           //triger_delay_sw
           backdata=spvi->vimyread(spvi->vipInstrHandle,"TRIGger:DELay:STATe?\n");
           if(backdata!=NULL){
           QString ts=QString(QLatin1String(backdata));
           if(ts=="1"){
               ui->sp_triger_delaysw->setStyleSheet(Style_sp_on);
           }else{
               ui->sp_triger_delaysw->setStyleSheet("");
           }

           }else return;
           //triger_delay
           backdata=spvi->vimyread(spvi->vipInstrHandle,"TRIGger:DELay?\n");
           if(backdata!=NULL){
            QString dt =  unit_time(vigetdouble(QString(QLatin1String(backdata))));
            ui->sp_trigerdelay_value->setText(QString("Sweep_time:%1").arg(dt));
           }else return;
           //sweeptime
           backdata=spvi->vimyread(spvi->vipInstrHandle,"SWEep:TIME?\n");
           if(backdata!=NULL){
            QString st =  unit_time(vigetdouble(QString(QLatin1String(backdata))));
            ui->sp_sweeptime_value->setText(QString("Sweep_time:%1").arg(st));
           }else return;
           spvi->visaouttime=0;
       }else{
        spvi->visaouttime++;
        if(spvi->visaouttime>2){
            spvi->visaouttime=0;
            spvi->viclose();
            sp_showmsg("con2 powersource outtime");
            ui->con2spectrograph->setText("CON");
            return;
        }
       }



    }
}
int refallcount=0;
void MainWindow::dorefrashsp(int w){
    if(w=DOSPECTROGRAPH){
        if(spvi->vilife()){
            refallcount++;
            //if(refallcount>40){
            if(false){
                refallcount=0;
               sp_conuiinit();
            }else{
            for(int i=1;i<=12;i++){
                 //marker1 Yvalue
                QString  askfor=QString("CALC:MARK%1:Y?\n").arg(i);
                char *backdata=spvi->vimyread(spvi->vipInstrHandle,QString2char(askfor));
                if(backdata!=NULL){
                        QLabel* mkertx =this->findChild<QLabel*>(QString("sp_marker_value_%1").arg(i));
                        QString spmiy=QString(QLatin1String(backdata));
                        mkertx->setText(QString("%1:%2").arg(vigetdouble(spmiy)).arg(ui->sp_unit->currentText()));
                        MainWindow::syncdata_sp(QString("sp_marker_value_%1").arg(i),QString(QLatin1String(backdata)));
                        spvi->visaouttime=0;
                        }else{
                            spvi->visaouttime++;
                            if(spvi->visaouttime>2){
                                spvi->visaouttime=0;
                                spvi->viclose();
                                sp_showmsg("con2 powersource outtime");
                                ui->con2spectrograph->setText("CON");
                                ui->sp_msg->setStyleSheet(Style_spdisconneted);
                                refallcount=0;
                                return;
                            }

                        }
           }


            }
        }

    }
}
void MainWindow::sp_showmsg(QString msg){
    QString smsg=ui->sp_msg->toPlainText();
    smsg.append(QString("%1\n").arg(msg));
    if(smsg.length()>1000){
    smsg.clear();
    }
    ui->sp_msg->setText(smsg);
}

void MainWindow::on_con2spectrograph_clicked()
{
    if(ui->con2spectrograph->text()=="CON"){
        if(ui->sp_edit_addr->text()!=NULL&&ui->sp_edit_addr->text()!=""){

            if(spvi->viconopen(ui->sp_edit_addr->text())==0){
                ui->con2spectrograph->setText("DISCON");
                ui->sp_msg->setStyleSheet(Style_spconneted);
                sp_conuiinit();
            }else{
              sp_showmsg("cannot con2ps");
              ui->con2spectrograph->setText("CON");
               ui->sp_msg->setStyleSheet(Style_spdisconneted);
            }
        }
    }else{
         spvi->viclose();
         ui->con2spectrograph->setText("CON");
    }
}
void MainWindow::on_sp_edit_addr_returnPressed()
{
    on_con2spectrograph_clicked();
}
void MainWindow::on_sp_unit_currentTextChanged(const QString &arg1)
{
    QString askfor=QString("UNIT:POWer %1\n").arg(arg1);
    sp_sendeddo(spvi->vipInstrHandle,QString2char(askfor));
}

void MainWindow::sp_makersw(int w){
     QPushButton *mkerbt =this->findChild<QPushButton*>(QString("sp_marker_%1").arg(w));
     QString askforon=QString("CALCulate:MARKer%1:STATe 1\n").arg(w);
     QString askforoff=QString("CALCulate:MARKer%1:STATe 0\n").arg(w);
     if(mkerbt->styleSheet()==Style_sp_on){
         sp_sendeddo(spvi->vipInstrHandle,QString2char(askforoff));
     }else{
         sp_sendeddo(spvi->vipInstrHandle,QString2char(askforon));
     }
}
void MainWindow::on_sp_marker_1_clicked()
{
   sp_makersw(1);
}

void MainWindow::on_sp_marker_2_clicked()
{
   sp_makersw(2);
}

void MainWindow::on_sp_marker_3_clicked()
{
   sp_makersw(3);
}

void MainWindow::on_sp_marker_4_clicked()
{
    sp_makersw(4);
}

void MainWindow::on_sp_marker_5_clicked()
{
   sp_makersw(5);
}

void MainWindow::on_sp_marker_6_clicked()
{
    sp_makersw(6);
}

void MainWindow::on_sp_marker_7_clicked()
{
    sp_makersw(7);
}

void MainWindow::on_sp_marker_8_clicked()
{
    sp_makersw(8);
}

void MainWindow::on_sp_marker_9_clicked()
{
    sp_makersw(9);
}

void MainWindow::on_sp_marker_10_clicked()
{
    sp_makersw(10);
}

void MainWindow::on_sp_marker_11_clicked()
{
    sp_makersw(11);
}

void MainWindow::on_sp_marker_12_clicked()
{
    sp_makersw(12);
}

void MainWindow::sp_makerfreqset(int ch, QString value){
    QString askfor=QString("CALCulate:MARKer%1:STATe 1\n").arg(ch);
    spvi->viconsend(spvi->vipInstrHandle,QString2char(askfor));
    askfor=QString("CALCulate:MARKer%1:X %2%3\n").arg(ch).arg(value).arg(ui->sp_freq_unit->currentText());
    sp_sendeddo(spvi->vipInstrHandle,QString2char(askfor));
}
void MainWindow::sp_mainwindowfreqset(int ch){
  QLineEdit  *fedit =this->findChild<QLineEdit*>(QString("sp_m%1_edit_fv").arg(ch));
  if(fedit->text()!=NULL&&fedit->text()!=""&&fedit->text().toDouble()>0){
    sp_makerfreqset(ch,fedit->text());
  }
}
void MainWindow::on_sp_m1_edit_fv_returnPressed()
{
    sp_mainwindowfreqset(1);
}

void MainWindow::on_sp_m2_edit_fv_returnPressed()
{
sp_mainwindowfreqset(2);
}

void MainWindow::on_sp_m3_edit_fv_returnPressed()
{
sp_mainwindowfreqset(3);
}

void MainWindow::on_sp_m4_edit_fv_returnPressed()
{
sp_mainwindowfreqset(4);
}

void MainWindow::on_sp_m5_edit_fv_returnPressed()
{
sp_mainwindowfreqset(5);
}

void MainWindow::on_sp_m6_edit_fv_returnPressed()
{
sp_mainwindowfreqset(6);
}

void MainWindow::on_sp_m7_edit_fv_returnPressed()
{
sp_mainwindowfreqset(7);
}

void MainWindow::on_sp_m8_edit_fv_returnPressed()
{
sp_mainwindowfreqset(8);
}

void MainWindow::on_sp_m9_edit_fv_returnPressed()
{
sp_mainwindowfreqset(9);
}

void MainWindow::on_sp_m10_edit_fv_returnPressed()
{
sp_mainwindowfreqset(10);
}

void MainWindow::on_sp_m11_edit_fv_returnPressed()
{
sp_mainwindowfreqset(11);
}

void MainWindow::on_sp_m12_edit_fv_returnPressed()
{
sp_mainwindowfreqset(12);
}

void MainWindow::sp_sendeddo(ViSession vi, ViConstString writeFmt){
    spvi->viconsend(vi,writeFmt);
    sp_conuiinit();
}
void MainWindow::on_sp_edit_refleve_returnPressed()
{
    if(ui->sp_edit_refleve->text().toDouble()>0||ui->sp_edit_refleve->text()=="0"){
        sp_sendeddo(spvi->vipInstrHandle,QString2char(QString("DISPlay:WINDow:TRACe:Y:RLEVel %1dBm\n").arg(ui->sp_edit_refleve->text())));
    }
}

void MainWindow::on_sp_edit_freq_start_returnPressed()
{
    if(ui->sp_edit_freq_start->text().toDouble()>0){

        sp_sendeddo(spvi->vipInstrHandle,QString2char(QString("FREQuency:STARt %1%2\n").arg(ui->sp_edit_freq_start->text()).arg(ui->sp_freq_unit->currentText())));
    }
}

void MainWindow::on_sp_edit_freq_stop_returnPressed()
{
    if(ui->sp_edit_freq_stop->text().toDouble()>0){
        sp_sendeddo(spvi->vipInstrHandle,QString2char(QString("FREQuency:STOP %1%2\n").arg(ui->sp_edit_freq_stop->text()).arg(ui->sp_freq_unit->currentText())));
    }
}

void MainWindow::on_sp_edit_freq_centre_returnPressed()
{
    if(ui->sp_edit_freq_centre->text().toDouble()>0){
        sp_sendeddo(spvi->vipInstrHandle,QString2char(QString("FREQuency:CENTer %1%2\n").arg(ui->sp_edit_freq_centre->text()).arg(ui->sp_freq_unit->currentText())));
    }
}

void MainWindow::on_sp_edit_freq_span_returnPressed()
{
    if(ui->sp_edit_freq_span->text().toDouble()>0||ui->sp_edit_freq_span->text()=="0"){
        sp_sendeddo(spvi->vipInstrHandle,QString2char(QString("FREQuency:SPAN %1%2\n").arg(ui->sp_edit_freq_span->text()).arg(ui->sp_freq_unit->currentText())));
    }
}

void MainWindow::on_sp_edit_offset_returnPressed()
{
    if(ui->sp_edit_offset->text().toDouble()>0||ui->sp_edit_offset->text()=="0"){
        sp_sendeddo(spvi->vipInstrHandle,QString2char(QString("DISPlay:WINDow:TRACe:Y:RLEVel:OFFSet %1dB\n").arg(ui->sp_edit_offset->text())));
    }

}

void MainWindow::on_sp_trigdelay_edit_returnPressed()
{
    if(ui->sp_trigdelay_edit->text().toDouble()>0||ui->sp_trigdelay_edit->text()=="0"){
         sp_sendeddo(spvi->vipInstrHandle,QString2char(QString("TRIGger:DELay %1%2\n").arg(ui->sp_trigdelay_edit->text()).arg(ui->sp_time_unit->currentText())));
    }
}


void MainWindow::on_sp_edit_sweeptime_edit_returnPressed()
{
    if(ui->sp_edit_sweeptime_edit->text().toDouble()>0){
        sp_sendeddo(spvi->vipInstrHandle,QString2char(QString("SWEep:TIME %1%2\n").arg(ui->sp_edit_sweeptime_edit->text()).arg(ui->sp_time_unit->currentText())));
    }
}
void MainWindow::on_sp_triger_mode_currentTextChanged(const QString &arg1)
{
  sp_sendeddo(spvi->vipInstrHandle,QString2char(QString("TRIGger:SOURce %1\n").arg(ui->sp_triger_mode->currentText())));
}

void MainWindow::on_sp_triger_delaysw_clicked()
{
    if(ui->sp_triger_delaysw->styleSheet()==Style_sp_on){
        sp_sendeddo(spvi->vipInstrHandle,"TRIGger:DELay:STATe 0\n");
    }else{
        sp_sendeddo(spvi->vipInstrHandle,"TRIGger:DELay:STATe 1\n");
    }
}
