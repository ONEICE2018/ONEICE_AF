#include "datarecord_base.h"
#include "ui_datarecord_base.h"
heart *acd_refheart;
datarecord_base *datarecord_base::nowdatarecord_base=NULL;
datarecord_base::datarecord_base(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::datarecord_base)
{
    ui->setupUi(this);
    nowdatarecord_base=this;
    acd_init();
}

datarecord_base::~datarecord_base()
{
    delete ui;
}
void datarecord_base::closeEvent(QCloseEvent *event){
    nowdatarecord_base=NULL;
    if(MainWindow::goto_once_dkb==1){
    MainWindow::goto_once_dkb=0;

    }

}
void datarecord_base::acd_init(){
    acd_refheart=new heart(DOACDATA);
    QObject::connect(acd_refheart,SIGNAL(maindoacd(int)), this, SLOT(dorefacd(int)));
    acd_refheart->start();
    //pm
    ItemModel_pm = new QStandardItemModel(this);
    if(MainWindow::CENTERdata.contains("PM")){
        QMapIterator<QString,QString>j(MainWindow::CENTERdata.value("PM"));
         while (j.hasNext()) {
             j.next();
              QStandardItem *item = new QStandardItem(QString("PM<->%1").arg(j.key()));
              ItemModel_pm->appendRow(item);
         }

        ui->datalist_pm->setModel(ItemModel_pm);
//        ui->datalist_pm->setFixedSize(200,300);

        connect(ui->datalist_pm,SIGNAL(clicked(QModelIndex)),this,SLOT(pmItemClicked(QModelIndex)));
    }
    //ps
    ItemModel_ps = new QStandardItemModel(this);
    if(MainWindow::CENTERdata.contains("PS")){
        QMapIterator<QString,QString>j(MainWindow::CENTERdata.value("PS"));
         while (j.hasNext()) {
             j.next();
              QStandardItem *item = new QStandardItem(QString("PS<->%1").arg(j.key()));
              ItemModel_ps->appendRow(item);
         }

        ui->datalist_ps->setModel(ItemModel_ps);
//        ui->datalist_ps->setFixedSize(200,300);

        connect(ui->datalist_ps,SIGNAL(clicked(QModelIndex)),this,SLOT(psItemClicked(QModelIndex)));
    }
    //sp
    ItemModel_sp = new QStandardItemModel(this);
    if(MainWindow::CENTERdata.contains("SP")){
        QMapIterator<QString,QString>j(MainWindow::CENTERdata.value("SP"));
         while (j.hasNext()) {
             j.next();
              QStandardItem *item = new QStandardItem(QString("SP<->%1").arg(j.key()));
              ItemModel_sp->appendRow(item);
         }

        ui->datalist_sp->setModel(ItemModel_sp);
        //ui->datalist_sp->setFixedSize(200,300);

        connect(ui->datalist_sp,SIGNAL(clicked(QModelIndex)),this,SLOT(spItemClicked(QModelIndex)));
    }
    //dcp
    ItemModel_dcp = new QStandardItemModel(this);
    if(MainWindow::CENTERdata.contains("DCP")){
        QMapIterator<QString,QString>j(MainWindow::CENTERdata.value("DCP"));
         while (j.hasNext()) {
             j.next();
              QStandardItem *item = new QStandardItem(QString("DCP<->%1").arg(j.key()));
              ItemModel_dcp->appendRow(item);
         }

        ui->datalist_dcp->setModel(ItemModel_dcp);
        //ui->datalist_dcp->setFixedSize(200,300);

        connect(ui->datalist_dcp,SIGNAL(clicked(QModelIndex)),this,SLOT(dcpItemClicked(QModelIndex)));
    }
    //ana
    ItemModel_ana = new QStandardItemModel(this);
    if(MainWindow::CENTERdata.contains("ANA")){
        QMapIterator<QString,QString>j(MainWindow::CENTERdata.value("ANA"));
         while (j.hasNext()) {
             j.next();
              QStandardItem *item = new QStandardItem(QString("ANA<->%1").arg(j.key()));
              ItemModel_ana->appendRow(item);
         }

        ui->datalist_ana->setModel(ItemModel_ana);
//        ui->datalist_ana->setFixedSize(200,300);

        connect(ui->datalist_ana,SIGNAL(clicked(QModelIndex)),this,SLOT(anaItemClicked(QModelIndex)));
    }
    ItemModel_choiced  = new QStandardItemModel(this);
 connect(ui->datalist_choiced,SIGNAL(clicked(QModelIndex)),this,SLOT(choicedItemClicked(QModelIndex)));
}
void datarecord_base::choicedItemClicked(QModelIndex  qmi){
    QString strTemp;
     strTemp = qmi.data().toString();
     ItemModel_choiced->removeRows(qmi.row(), 1);
     if(record.contains(strTemp)){
        record.remove(strTemp);
     }

}
void datarecord_base::pmItemClicked(QModelIndex qmi){
    QString strTemp;
     strTemp = qmi.data().toString();
     if(!record.contains(strTemp)){
             QStandardItem *item = new QStandardItem(strTemp);
             ItemModel_choiced->appendRow(item);
             ui->datalist_choiced->setModel(ItemModel_choiced);
             record.insert( strTemp,*(new QStringList) );
             if(recordnum>0){
                 for(int i=0;i<recordnum;i++){
                    (record[strTemp]).append("0");
                 }
             }
      }

}
void datarecord_base::psItemClicked(QModelIndex qmi){
    QString strTemp;
     strTemp = qmi.data().toString();
     if(!record.contains(strTemp)){
         QStandardItem *item = new QStandardItem(strTemp);
         ItemModel_choiced->appendRow(item);
         ui->datalist_choiced->setModel(ItemModel_choiced);
             record.insert( strTemp,*(new QStringList) );
             if(recordnum>0){
                 for(int i=0;i<recordnum;i++){
                    record[strTemp].append("0");
                 }
             }
      }

}
void datarecord_base::spItemClicked(QModelIndex qmi){
    QString strTemp;
     strTemp = qmi.data().toString();
     if(!record.contains(strTemp)){
             QStandardItem *item = new QStandardItem(strTemp);
             ItemModel_choiced->appendRow(item);
             ui->datalist_choiced->setModel(ItemModel_choiced);
             record.insert( strTemp,*(new QStringList) );
             if(recordnum>0){
                 for(int i=0;i<recordnum;i++){
                    record[strTemp].append("0");
                 }
             }
      }
}
void datarecord_base::dcpItemClicked(QModelIndex qmi){
    QString strTemp;
     strTemp = qmi.data().toString();
     if(!record.contains(strTemp)){
             QStandardItem *item = new QStandardItem(strTemp);
             ItemModel_choiced->appendRow(item);
             ui->datalist_choiced->setModel(ItemModel_choiced);
             record.insert( strTemp,*(new QStringList) );
             if(recordnum>0){
                 for(int i=0;i<recordnum;i++){
                    record[strTemp].append("0");
                 }
             }
    }

}
void datarecord_base::anaItemClicked(QModelIndex qmi){
     QString strTemp;
     strTemp = qmi.data().toString();
     if(!record.contains(strTemp)){
         QStandardItem *item = new QStandardItem(strTemp);
         ItemModel_choiced->appendRow(item);
         ui->datalist_choiced->setModel(ItemModel_choiced);
             record.insert( strTemp,*(new QStringList) );
             if(recordnum>0){
                 for(int i=0;i<recordnum;i++){
                    record[strTemp].append("0");
                 }
             }
     }
}

void datarecord_base::dorefacd(int w){
    if(w=DOACDATA){
        QString alldata="";
        QMapIterator<QString ,QStringList>i(record);
        while(i.hasNext()){
            i.next();
            alldata.append(i.key());
            alldata.append("	");
        }
        alldata.append("\n");
        for(int j=0;j<recordnum;j++){
            QMapIterator<QString ,QStringList>mi(record);
            while(mi.hasNext()){
                mi.next();
                if(mi.value().length()==recordnum){
                    alldata.append(mi.value()[j]);
                    alldata.append("	");
                }

            }
            alldata.append("\n");
        }
        if(alldata!=ui->acd_msg->toPlainText()){
            showmsg(alldata);
        }
//        QString alldata="";
//    QMapIterator<QString,QMap<QString,QString>>i(MainWindow::CENTERdata);
//    while (i.hasNext()) {
//            i.next();
//            alldata.append(QString("%1\n").arg(i.key()));
//            QMapIterator<QString,QString>j(i.value());
//            while (j.hasNext()) {
//                j.next();
//                alldata.append(QString("%1 %2\n").arg(j.key()).arg(j.value()));
//            }

//       }
//       showmsg(alldata);


}
}
void datarecord_base::showmsg(QString adcmsg){
    ui->acd_msg->setText(adcmsg);

}

void datarecord_base::recorddata_one()
{
    QMapIterator<QString,QStringList>i(record);
    int recordok=0;
    while(i.hasNext()){
        i.next();
        if(MainWindow::CENTERdata.contains(i.key().split("<->")[0])){
            if(MainWindow::CENTERdata.value(i.key().split("<->")[0]).contains(i.key().split("<->")[1])){
                record[i.key()].append(MainWindow::CENTERdata.value(i.key().split("<->")[0]).value(i.key().split("<->")[1]));
            recordok++;
            }
        }
    }

    if(recordok>0&&recordok==record.size()){
    recordnum++;//数据记录成功
    }
}

void datarecord_base::on_drb_record_clicked()
{
    recorddata_one();
}

void datarecord_base::on_drb_clearnowrecord_clicked()
{
    QMapIterator<QString,QStringList>i(record);
    while(i.hasNext()){
        i.next();
     record[i.key()].clear();
    }

  recordnum=0;


}
