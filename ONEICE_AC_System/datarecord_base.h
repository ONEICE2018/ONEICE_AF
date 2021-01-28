#ifndef DATARECORD_BASE_H
#define DATARECORD_BASE_H

#include <QWidget>
#include "mainwindow.h"
#include <QAxObject>
#include <QDir>
#include <QFile>

#include <QStringListModel>
#include <QStandardItemModel>
#include <QModelIndex>
#include "chioced_dataitem.h"

namespace Ui {
class datarecord_base;
}

class datarecord_base : public QWidget
{
    Q_OBJECT

public:
    explicit datarecord_base(QWidget *parent = 0);
    ~datarecord_base();
     void recorddata_one();
     static datarecord_base *nowdatarecord_base;
private slots:
    //数据中心
    void dorefacd(int w);
    //ItemClicked
    void pmItemClicked(QModelIndex   qmi);
    void psItemClicked(QModelIndex   qmi);
    void spItemClicked(QModelIndex   qmi);
    void dcpItemClicked(QModelIndex  qmi);
    void anaItemClicked(QModelIndex  qmi);
    void choicedItemClicked(QModelIndex  qmi);

    void on_drb_record_clicked();

    void on_drb_clearnowrecord_clicked();

protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::datarecord_base *ui;
   void  showmsg(QString adcmsg);
   void acd_init();
   //listviwe 的库item
   QStandardItemModel *ItemModel_pm;
   QStandardItemModel *ItemModel_ps;
   QStandardItemModel *ItemModel_sp;
   QStandardItemModel *ItemModel_dcp;
   QStandardItemModel *ItemModel_ana;
   QStandardItemModel *ItemModel_choiced;
   int recordnum=0;
   QMap<QString, QStringList> record;

};

#endif // DATARECORD_BASE_H
