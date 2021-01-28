#include "mainwindow.h"
 QMap<QString,QString>                  MainWindow::DCdata;//直流电源数据
 QMap<QString,QString>                  MainWindow::PSdata;//功率源数据
 QMap<QString,QString>                  MainWindow::SPdata;//频谱仪数据
 QMap<QString,QString>                  MainWindow::PMdata;//功率计
 QMap<QString,QString>                  MainWindow::ANAdata;//网分
 QMap<QString,QMap<QString,QString> >   MainWindow::CENTERdata;//数据源 <数据名称 数据值>
void MainWindow::datainit_all(){
//    DCdata.insert("DCdata","0");
//    PSdata.insert("PSdata","0");
//    SPdata.insert("SPdata","0");
//    PMdata.insert("PMdata","0");
//    ANAdata.insert("ANAdata","0");
    CENTERdata.insert("DCP", DCdata );
    CENTERdata.insert("PS", PSdata );
    CENTERdata.insert("SP", SPdata );
    CENTERdata.insert("PM", PMdata );
    CENTERdata.insert("ANA", ANAdata);

    //pm
    MainWindow::syncdata_pm("pm_upervalue"  ,"0");
    MainWindow::syncdata_pm("pm_lowervalue" ,"0");
    MainWindow::syncdata_pm("pm_offsetvalue","0");
    MainWindow::syncdata_pm("pm_freqvalue"  ,"0");

    //功率校准的一些参数放到功率计这里
    MainWindow::syncdata_pm("pm_stm_ADC"       ,"0");
    MainWindow::syncdata_pm("pm_stm_DAC"       ,"0");
    MainWindow::syncdata_pm("pm_stm_Variable"  ,"0"); //自变量的实时值 一般是DAC
    MainWindow::syncdata_pm("pm_stm_Index"     ,"0");    //索引的值


    //ps
    MainWindow::syncdata_ps("ps_freqvalue"  ,"0");
    MainWindow::syncdata_ps("ps_pwdvalue"   ,"0");
    MainWindow::syncdata_ps("ps_delayvalue" ,"0");
    MainWindow::syncdata_ps("ps_periodvalue","0");
    MainWindow::syncdata_ps("ps_widthvalue" ,"0");
    //sp
    MainWindow::syncdata_sp("sp_m1_freqvalue"  ,"0");
    MainWindow::syncdata_sp("sp_m2_freqvalue"  ,"0");
    MainWindow::syncdata_sp("sp_m3_freqvalue"  ,"0");
    MainWindow::syncdata_sp("sp_m4_freqvalue"  ,"0");
    MainWindow::syncdata_sp("sp_m5_freqvalue"  ,"0");
    MainWindow::syncdata_sp("sp_m6_freqvalue"  ,"0");
    MainWindow::syncdata_sp("sp_m7_freqvalue"  ,"0");
    MainWindow::syncdata_sp("sp_m8_freqvalue"  ,"0");
    MainWindow::syncdata_sp("sp_m9_freqvalue"  ,"0");
    MainWindow::syncdata_sp("sp_marker_value_1","0");
    MainWindow::syncdata_sp("sp_marker_value_2","0");
    MainWindow::syncdata_sp("sp_marker_value_3","0");
    MainWindow::syncdata_sp("sp_marker_value_4","0");
    MainWindow::syncdata_sp("sp_marker_value_5","0");
    MainWindow::syncdata_sp("sp_marker_value_6","0");
    MainWindow::syncdata_sp("sp_marker_value_7","0");
    MainWindow::syncdata_sp("sp_marker_value_8","0");
    MainWindow::syncdata_sp("sp_marker_value_9","0");


    MainWindow::syncdata_sp("sp_refleve"    ,"0");
    MainWindow::syncdata_sp("sp_offset"     ,"0");
    MainWindow::syncdata_sp("sp_freq_start" ,"0");
    MainWindow::syncdata_sp("sp_freq_center","0");
    MainWindow::syncdata_sp("sp_freq_stop"  ,"0");
    MainWindow::syncdata_sp("sp_span"       ,"0");
    //dcp
    MainWindow::syncdata_dc("dc_ch1_vset" ,"0");
    MainWindow::syncdata_dc("dc_ch1_iset" ,"0");
    MainWindow::syncdata_dc("dc_ch2_vset" ,"0");
    MainWindow::syncdata_dc("dc_ch2_iset" ,"0");
    MainWindow::syncdata_dc("dc_ch3_vset" ,"0");
    MainWindow::syncdata_dc("dc_ch3_iset" ,"0");
    MainWindow::syncdata_dc("dc_ch1_realV","0");
    MainWindow::syncdata_dc("dc_ch2_realV","0");
    MainWindow::syncdata_dc("dc_ch3_realV","0");
    MainWindow::syncdata_dc("dc_ch1_realI","0");
    MainWindow::syncdata_dc("dc_ch2_realI","0");
    MainWindow::syncdata_dc("dc_ch3_realI","0");
    MainWindow::syncdata_dc("dc_ch1_realP","0");
    MainWindow::syncdata_dc("dc_ch2_realP","0");
    MainWindow::syncdata_dc("dc_ch3_realP","0");

    //ana
    MainWindow::syncdata_ana("ana_freqstart" ,"0");
    MainWindow::syncdata_ana("ana_freqspan"  ,"0");
    MainWindow::syncdata_ana("ana_freqcenter","0");
    MainWindow::syncdata_ana("ana_freqstop"  ,"0");

    for(int i=1;i<10;i++)
    {
        for(int j=1;j<10;j++){
            MainWindow::syncdata_ana(QString("Tr%1->MK%2").arg(i).arg(j),"0");
        }
    }



}
//dc数据同步
void MainWindow::syncdata_dc(QString  name, QString value){

    if(MainWindow::CENTERdata.contains("DCP")){
       MainWindow::CENTERdata["DCP"].insert(name,value);
    }
}
//ps数据同步
void MainWindow::syncdata_ps(QString  name, QString value){

    if(MainWindow::CENTERdata.contains("PS")){
       MainWindow::CENTERdata["PS"].insert(name,value);
    }
}
//sp数据同步
void MainWindow::syncdata_sp(QString  name, QString value){

    if(MainWindow::CENTERdata.contains("SP")){
       MainWindow::CENTERdata["SP"].insert(name,value);
    }
}
//pm数据同步
void MainWindow::syncdata_pm(QString  name, QString value){

    if(MainWindow::CENTERdata.contains("PM")){
       MainWindow::CENTERdata["PM"].insert(name,value);
    }
}
//ana数据同步
void MainWindow::syncdata_ana(QString  name, QString value){

    if(MainWindow::CENTERdata.contains("ANA")){
       MainWindow::CENTERdata["ANA"].insert(name,value);
    }
}



