#ifndef ONELISTWIDGET_H
#define ONELISTWIDGET_H
#include <QTableWidget>
#include <QListWidget>
#include <QtGui>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <qdebug.h>
#include <QPixmap>
#include <QObject>
#include <curve_fitting.h>
#include <QMenu>
#include <QAction>

class OneListWidget :  public QListWidget
{
      Q_OBJECT
public:
    OneListWidget(QWidget *parent=0);
    bool contains_text(QString text);
    void setDifferent(int value);

signals:
    void OneListWidget_ListDataChanged(QString);
     void OneListWidget_DelatLastItem(QString);
     void OneListWidget_ItemClicked(int);
//public slots:
//     void emited_OneListWidget_ListDataChanged(QString objname);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint startPos;
    QPoint endPos;
    int changedrow;
    QMimeData *mimeData;
    bool noCurrentItem=true;
    QListWidgetItem * oneitem ;
    QList<QString> cpList;
    int oldrow=0;
    int different=0;      //0 所有功能都有 1 只有删除

};


#endif // ONELISTWIDGET_H
