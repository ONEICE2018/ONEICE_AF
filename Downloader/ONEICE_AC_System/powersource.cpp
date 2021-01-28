//#include "mainwindow.h"
//#include "ui_mainwindow.h"
//#include "powermeter.h"
//viclass *psvi;
//heart *ps_queryheart;
//QString Style_ps_on="background-color: rgb(85, 255, 0);";
//QString Style_psconneted="background-color: rgb(255, 255, 255);";
//QString Style_psdisconneted="background-color: rgb(88, 88, 88);";
//void MainWindow::ps_init(){
//    psvi=new viclass();
//    ps_queryheart=new heart(DOPOWERSOURCE);
//    QObject::connect(ps_queryheart,SIGNAL(maindops(int)), this, SLOT(dorefrashps(int)));
//    ps_queryheart->start();
//}
//void MainWindow::ps_conuiinit(){
//    if(psvi->vilife()){
//       char * backdata=psvi->vimyread(psvi->vipInstrHandle,"FREQuency?\n");
//       if(backdata!=NULL){
//           QString psFreq=QString(QLatin1String(backdata));
//           double  Freqvalue= QString(QString(psFreq.split("\\")[0]).split("E")[0]).toDouble();
//           double  xs= QString(QString(psFreq.split("\\")[0]).split("E")[1]).toDouble();
//           if(Freqvalue>0){
//                       for(int i=0;i<abs(xs);i++){
//                            if(xs>=0){
//                                Freqvalue=Freqvalue*10;
//                            }else{
//                                Freqvalue=Freqvalue/10;
//                            }
//                       }
//           }else{
//               Freqvalue=0;
//           }
//           double nowpsfreq=0;
//           if(Freqvalue>=1000000000){
//               ui->ps_freqvalue->setText(QString("FREQ:%1GHz").arg(nowpsfreq=Freqvalue/1000000000.0));

//           }else if(Freqvalue>=1000000){
//               ui->ps_freqvalue->setText(QString("FREQ:%1MHz").arg(nowpsfreq=Freqvalue/1000000.0));

//           }else if(Freqvalue>=1000){
//               ui->ps_freqvalue->setText(QString("FREQ:%1KHz").arg(nowpsfreq=Freqvalue/1000.0));

//           }else{
//               ui->ps_freqvalue->setText(QString("FREQ:%1Hz").arg(nowpsfreq=Freqvalue/1.0));
//           }
//           MainWindow::syncdata_ps("ps_freqvalue",QString("%1").arg(nowpsfreq));
//           backdata=psvi->vimyread(psvi->vipInstrHandle,"POWer?\n");
//           if(backdata!=NULL){
//               QString psPwd=QString(QLatin1String(backdata));
//               ui->ps_pwdvalue->setText(QString("Pwd:%1dbm").arg(psPwd.toDouble()));
//               MainWindow::syncdata_ps("ps_pwdvalue",QString("%1").arg(psPwd.toDouble()));
//           }else return;
//           backdata=psvi->vimyread(psvi->vipInstrHandle,"OUTP?\n");
//           if(backdata!=NULL){
//               QString psrfsw=QString(QLatin1String(backdata));
//               if(psrfsw=="1"){
//                  ui->ps_rfsw->setStyleSheet(Style_ps_on);
//               }else{
//                  ui->ps_rfsw->setStyleSheet("");
//               }
//           }else return;
//           backdata=psvi->vimyread(psvi->vipInstrHandle,"OUTPut:MODulation?\n");
//           if(backdata!=NULL){
//               QString psmodesw=QString(QLatin1String(backdata));
//               if(psmodesw=="1"){
//                  ui->ps_modesw->setStyleSheet(Style_ps_on);
//               }else{
//                  ui->ps_modesw->setStyleSheet("");
//               }
//           }else return;
//           backdata=psvi->vimyread(psvi->vipInstrHandle,"PULM:STATe?\n");
//           if(backdata!=NULL){
//               QString psplussw=QString(QLatin1String(backdata));
//               if(psplussw=="1"){
//                  ui->ps_plussw->setStyleSheet(Style_ps_on);
//               }else{
//                ui->ps_plussw->setStyleSheet("");
//               }
//           }else return;
//           backdata=psvi->vimyread(psvi->vipInstrHandle,"PULM:SOURce:INTernal?\n");
//           if(backdata!=NULL){
//               QString psmodech=QString(QLatin1String(backdata));
//               ui->ps_modech->setCurrentText(psmodech);
//           }else return;
//           backdata=psvi->vimyread(psvi->vipInstrHandle,"PULM:INT:DEL?\n");
//           if(backdata!=NULL){
//               QString psdelay=QString(QLatin1String(backdata));
//               double doublevalue=psdelay.toDouble();
//               double nowdv=0;
//               if(doublevalue>=1){
//                   ui->ps_delayvalue->setText(QString("delay:%1s？").arg( nowdv=doublevalue));
//               }else if(doublevalue>=0.001){
//                 ui->ps_delayvalue->setText(QString("delay:%1ms？").arg( nowdv=doublevalue*1000));
//               }else if(doublevalue>=0.000001){
//                 ui->ps_delayvalue->setText(QString("delay:%1us？").arg( nowdv=doublevalue*1000000));
//               }else {
//                  ui->ps_delayvalue->setText(QString("delay:%1ns？").arg( nowdv=doublevalue*1000000000));
//               }
//             MainWindow::syncdata_ps("ps_delayvalue",QString("%1").arg(nowdv));

//           }else return;
//           backdata=psvi->vimyread(psvi->vipInstrHandle,"PULM:INT:PER?\n");
//           if(backdata!=NULL){
//               QString psperiod=QString(QLatin1String(backdata));
//               double doublevalue=psperiod.toDouble();
//               double now_prdv=0;
//               if(doublevalue>=1){
//                   ui->ps_periodvalue->setText(QString("period:%1s？").arg(now_prdv=doublevalue));
//               }else if(doublevalue>=0.001){
//                 ui->ps_periodvalue->setText(QString("period:%1ms？").arg(now_prdv=doublevalue*1000));
//               }else if(doublevalue>=0.000001){
//                 ui->ps_periodvalue->setText(QString("period:%1us？").arg(now_prdv=doublevalue*1000000));
//               }else {
//                  ui->ps_periodvalue->setText(QString("period:%1ns？").arg(now_prdv=doublevalue*1000000000));
//               }
//               MainWindow::syncdata_ps("ps_periodvalue",QString("%1").arg(now_prdv));

//           }else return;
//           backdata=psvi->vimyread(psvi->vipInstrHandle,"PULM:INTernal:PWIDth?\n");
//           if(backdata!=NULL){
//               QString pswidth=QString(QLatin1String(backdata));
//               double doublevalue=pswidth.toDouble();
//               double nowwd=0;
//               if(doublevalue>=1){
//                   ui->ps_widthvalue->setText(QString("width:%1s？").arg(nowwd=doublevalue));
//               }else if(doublevalue>=0.001){
//                 ui->ps_widthvalue->setText(QString("width:%1ms？").arg(nowwd=doublevalue*1000));
//               }else if(doublevalue>=0.000001){
//                 ui->ps_widthvalue->setText(QString("width:%1us？").arg(nowwd=doublevalue*1000000));
//               }else {
//                  ui->ps_widthvalue->setText(QString("width:%1ns？").arg(nowwd=doublevalue*1000000000));
//               }
//             MainWindow::syncdata_ps("ps_widthvalue",QString("%1").arg(nowwd));
//           }else return;

//           psvi->visaouttime=0;
//       }else{
//        psvi->visaouttime++;
//        if(psvi->visaouttime>2){
//            psvi->visaouttime=0;
//            psvi->viclose();
//            ps_showmsg("con2 powersource outtime");
//            ui->con2powersource->setText("CON");
//            ui->ps_screen->setStyleSheet(Style_psdisconneted);
//        }
//       }



//    }
//}
//void MainWindow::dorefrashps(int w){
//    if(w==DOPOWERSOURCE){
//        if(psvi->vilife()){
//            ps_conuiinit();
//        }
//    }
//}
//void MainWindow::on_con2powersource_clicked()
//{
//    if(ui->con2powersource->text()=="CON"){
//        if(ui->ps_edit_addr->text()!=NULL&&ui->ps_edit_addr->text()!=""){
//            if(psvi->viconopen(ui->ps_edit_addr->text())==0){
//                ui->con2powersource->setText("DISCON");
//                ui->ps_screen->setStyleSheet(Style_psconneted);
//                ps_conuiinit();
//            }else{
//              ps_showmsg("cannot con2ps");
//              ui->con2powersource->setText("CON");
//              ui->ps_screen->setStyleSheet(Style_psdisconneted);
//            }
//        }
//    }else{
//         psvi->viclose();
//         ui->con2powersource->setText("CON");
//         ui->ps_screen->setStyleSheet(Style_psdisconneted);
//    }
//}

//void MainWindow::ps_showmsg(QString msg){
//    ui->ps_msg->setText(msg);

//}

//void MainWindow::on_ps_modesw_clicked()
//{
//    if(ui->ps_modesw->styleSheet()==Style_ps_on){
//        psvi->viconsend(psvi->vipInstrHandle,"OUTPut:MODulation 0\n");
//    }else{
//        psvi->viconsend(psvi->vipInstrHandle,"OUTPut:MODulation 1\n");
//    }
//    ps_conuiinit();
//}

//void MainWindow::on_ps_rfsw_clicked()
//{
//    if(ui->ps_rfsw->styleSheet()==Style_ps_on){
//        psvi->viconsend(psvi->vipInstrHandle,"OUTP 0\n");
//    }else{
//        psvi->viconsend(psvi->vipInstrHandle,"OUTP 1\n");
//    }
//    ps_conuiinit();
//}

//void MainWindow::on_ps_edit_addr_returnPressed()
//{
//    on_con2powersource_clicked();
//}

//void MainWindow::on_ps_plussw_clicked()
//{
//    if(ui->ps_plussw->styleSheet()==Style_ps_on){
//        psvi->viconsend(psvi->vipInstrHandle,"PULM:STATe 0\n");
//    }else{
//        psvi->viconsend(psvi->vipInstrHandle,"PULM:STATe 1\n");
//    }
//    ps_conuiinit();
//}

//void MainWindow::on_ps_modech_currentTextChanged(const QString &arg1)
//{
//    QString psmodech=  QString("PULM:SOURce:INTernal %1\n").arg(arg1);
//    char*  ch;
//    QByteArray ba = psmodech.toLatin1(); // must
//    ch=ba.data();
//    psvi->viconsend(psvi->vipInstrHandle,ch);
//    ps_conuiinit();
//}

//void MainWindow::on_ps_edit_width_returnPressed()
//{

//    if((ui->ps_edit_width->text()!=NULL&&ui->ps_edit_width->text().toDouble()>0)||ui->ps_edit_width->text()=="0"){
//     ps_setwidth(ui->ps_edit_width->text().toDouble(),ui->ps_unit_width->currentText());
//    }


//}

//void MainWindow::on_ps_edit_period_returnPressed()
//{
//          if((ui->ps_edit_period->text()!=NULL&&ui->ps_edit_period->text().toDouble()>0)||ui->ps_edit_period->text()=="0"){
//            ps_setperiod(ui->ps_edit_period->text().toDouble(),ui->ps_unit_period->currentText());
//          }

//}

//void MainWindow::on_ps_edit_delay_returnPressed()
//{
//        if((ui->ps_edit_delay->text()!=NULL&&ui->ps_edit_delay->text().toDouble()>0)||ui->ps_edit_delay->text()=="0"){
//           ps_setdelay(ui->ps_edit_delay->text().toDouble(),ui->ps_unit_delay->currentText());
//        }
//}

//void MainWindow::on_ps_edit_freq_returnPressed()
//{
//    if((ui->ps_edit_freq->text()!=NULL&&ui->ps_edit_freq->text().toDouble()>0)||ui->ps_edit_freq->text()=="0"){
//       ps_setfreq(ui->ps_edit_freq->text().toDouble(),ui->ps_freq_unit->currentText());
//    }
//}

//void MainWindow::on_ps_edit_pwd_returnPressed()
//{

//    if(ui->ps_edit_pwd->text()!=NULL&&ui->ps_edit_pwd->text()!=""){
//        ps_setpout(ui->ps_edit_pwd->text().toDouble());
//    }
//}
//void MainWindow::ps_setfreq(double freq, QString uint){
//    QString psfreq=  QString("FREQuency %1%2\n").arg(freq).arg(uint);
//    char*  ch;
//    QByteArray ba = psfreq.toLatin1(); // must
//    ch=ba.data();
//    psvi->viconsend(psvi->vipInstrHandle,ch);
//    ps_conuiinit();
//}
//void MainWindow::ps_setpout(double pout){
//    QString pspwd=  QString("POWer %1dbm\n").arg(pout);
//    char*  ch;
//    QByteArray ba = pspwd.toLatin1(); // must
//    ch=ba.data();
//    psvi->viconsend(psvi->vipInstrHandle,ch);
//    ps_conuiinit();

//}
//void MainWindow::ps_setwidth(double width,QString unit){
//    QString pswidth=  QString("PULM:INTernal:PWIDth %1%2\n").arg(width).arg(unit);
//    char*  ch;
//    QByteArray ba = pswidth.toLatin1(); // must
//    ch=ba.data();
//    psvi->viconsend(psvi->vipInstrHandle,ch);
//    ps_conuiinit();

//}
//void MainWindow::ps_setperiod(double period,QString unit){
//    QString psperiod=  QString("PULM:INT:PER %1%2\n").arg(period).arg(unit);
//    char*  ch;
//    QByteArray ba = psperiod.toLatin1(); // must
//    ch=ba.data();
//    psvi->viconsend(psvi->vipInstrHandle,ch);
//    ps_conuiinit();

//}
//void MainWindow::ps_setdelay(double delay,QString unit){
//    QString psdelay=  QString("PULM:INT:DEL %1%2\n").arg(delay).arg(unit);
//    char*  ch;
//    QByteArray ba = psdelay.toLatin1(); // must
//    ch=ba.data();
//    psvi->viconsend(psvi->vipInstrHandle,ch);
//    ps_conuiinit();

//}
//void MainWindow::on_ps_freq_unit_currentTextChanged(const QString &arg1)
//{
//    on_ps_edit_freq_returnPressed();
//}

//void MainWindow::on_ps_unit_width_currentTextChanged(const QString &arg1)
//{
//    on_ps_edit_width_returnPressed();
//}

//void MainWindow::on_ps_unit_period_currentTextChanged(const QString &arg1)
//{
//    on_ps_edit_period_returnPressed();
//}

//void MainWindow::on_ps_unit_delay_currentTextChanged(const QString &arg1)
//{
//    on_ps_edit_delay_returnPressed();
//}

