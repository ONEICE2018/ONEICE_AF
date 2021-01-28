#include "onelistwidget.h"
/**
用法注意：1.  和基本的listwidget 不同 自定义了右键点击事件 重命名与删除
        2.  当不需要重命名时 需要在申明或初始化控件时将different设置为1
        3.  当最后一项删除时 会触发信号 OneListWidget_DelatLastItem（控件名），在运行程序中接收删除即可实现最后一项的删除（需要先断开信号槽，再连上信号槽 再删除最后一项）
        4.  当子项信息变化时会触发     OneListWidget_ListDataChanged(objectName()) 自行接收处理
*/
//鼠标右键拖拽会卡死
OneListWidget::OneListWidget(QWidget *parent):QListWidget(parent)
{
    this->setAcceptDrops(false);//设置QListWidget的Item项可以被拖拽
}

void OneListWidget::mousePressEvent( QMouseEvent *event )
{
    if (event->button() == Qt::LeftButton)
    {
        startPos = event->pos();
        oneitem = itemAt(startPos);

        if(oneitem!=NULL)
        {
            changedrow=row(oneitem);
            noCurrentItem=false;
            this->setCurrentItem(oneitem);
            qDebug("mousePressEvent:%d",noCurrentItem);
        }else{
            noCurrentItem=true;
             qDebug("mousePressEvent:%d",noCurrentItem);
        }
        if(!noCurrentItem){
             mimeData = new QMimeData;
             mimeData->setText( oneitem ->text());
         }
    }else   if (event->button() == Qt::RightButton){

        startPos = event->pos();
        oneitem = itemAt(startPos);
        if(oneitem!=NULL){
            this->setCurrentItem(oneitem);
            QMenu   *popMenu = new QMenu( this );

            QAction *deleteSeed ;
            if(different==0){
                deleteSeed = new QAction(tr("Close"), this);
            }else if(different==1){
                 deleteSeed = new QAction(tr("Delete"), this);
            }
            popMenu->addAction( deleteSeed );
             QAction *renameSeed;
            if(different==0)//有的控件不能有重命名
            {
                  renameSeed = new QAction(tr("ReName"), this);
                  popMenu->addAction( renameSeed );
            }

            QAction *result_pop= popMenu->exec( QCursor::pos() );
           if(result_pop==renameSeed){
               bool ok;
               QString text = QInputDialog::getText(this, tr("ReNameProject"),tr("ProjectName:"), QLineEdit::Normal,0, &ok);
               if (ok && !text.isEmpty())
               {
                 oneitem =currentItem();
                 if(oneitem->text()!=text&&!contains_text(text)){
                     oneitem->setText(text);
                     emit OneListWidget_ListDataChanged(objectName());
                 }
               }
           }
           if(result_pop==deleteSeed){
               QMessageBox customMsgBox;
               if(different==0){
                   customMsgBox.setWindowTitle("close the Item?");
               }else if(different==1){
                   customMsgBox.setWindowTitle("delate the Item?");
               }
               QPushButton *lockButton = customMsgBox.addButton("YES",QMessageBox::ActionRole);
               QPushButton *cancelButton =customMsgBox.addButton(QMessageBox::Cancel);
               // customMsgBox.setIconPixmap(QPixmap(":/image/icon.ico"));
               customMsgBox.exec();

               if (customMsgBox.clickedButton() == lockButton)
               {
                   oneitem =currentItem();
                   if(count()==1)
                   {
                       emit OneListWidget_DelatLastItem(objectName());
                   }else
                   {
                       delete oneitem;
                   }
                    emit OneListWidget_ListDataChanged(objectName());
               }
           }

            delete popMenu;
            delete deleteSeed;
           if(different==0)
           {
            delete renameSeed;
           }
        }
    }
}

//void OneListWidget::right_clicked_delate(){
//       QMessageBox customMsgBox;
//       customMsgBox.setWindowTitle("delate_Project?");
//       QPushButton *lockButton = customMsgBox.addButton("YES",QMessageBox::ActionRole);
//       QPushButton *cancelButton =customMsgBox.addButton(QMessageBox::Cancel);
//      // customMsgBox.setIconPixmap(QPixmap(":/image/icon.ico"));
//       customMsgBox.exec();

//       if (customMsgBox.clickedButton() == lockButton)
//       {
//           oneitem =currentItem();
//           delete oneitem;
//       }

//}

void OneListWidget::mouseMoveEvent(QMouseEvent *event)
{

    if(!noCurrentItem){
         QDrag *drag = new QDrag(this);
         drag->setMimeData(mimeData);
         //QPixmap *pixmap = new QPixmap("/imgsource/button_preset.png");
         //drag->setPixmap(*pixmap);

        //删除原有的item,在dropEvent(QDropEvent *event)函数中插入item
         Qt::DropAction dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
         if(dropAction == Qt::MoveAction)
         {
             delete oneitem;//先删除再插入
             QListWidgetItem *backitem=itemAt(endPos);
            if(backitem!=NULL){
             this->setCurrentItem(backitem);
            }
         }
     }
//    qDebug("mouseMovmentEvent:%d",noCurrentItem);
//    endPos = event->pos();//得到鼠标移动到的坐标
//    QListWidgetItem *itemRow = itemAt(endPos);//通过endPos获取鼠标位置所在的行
//    if(itemRow!=NULL){
//        itemRow->setBackgroundColor(Qt::red);
//    }
}

void OneListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        emit  OneListWidget_ItemClicked(currentRow());
        }
    if(!noCurrentItem){
        int distance = (endPos - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
        {
            startDrag(Qt::MoveAction);
        }
     }
}

void OneListWidget::setDifferent(int value)
{
    different = value;
}

void OneListWidget::dragEnterEvent( QDragEnterEvent *event )
{
    OneListWidget *source = (OneListWidget *)((void*)(event->source()));
    if (source && source == this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void OneListWidget::dragMoveEvent( QDragMoveEvent *event )
{
    OneListWidget *source = (OneListWidget *)((void*)(event->source()));
    if (source && source == this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void OneListWidget::dropEvent( QDropEvent *event )
{

    OneListWidget *source = (OneListWidget *)((void*)(event->source()));
    if (source && source == this)
    {
        if(!noCurrentItem)
        {
            qDebug("dropEvent:%d",noCurrentItem);
            endPos = event->pos();//得到鼠标移动到的坐标
            QListWidgetItem *itemRow = itemAt(endPos);//通过endPos获取鼠标位置所在的行
            int rowCount;
            if(itemRow!=NULL)
            {
                rowCount = row(itemRow);

                insertItem(rowCount,mimeData->text());//row+1 为鼠标所指向的行，如果只是向末尾位置插入，把insertItem(rowCount + 1,mimeData->text())改为addItem(mimeData->text())

                if(changedrow==rowCount){
                    //delete oneitem;//先删除再插入
                    QListWidgetItem *backitem=itemAt(endPos);
                    this->setCurrentItem(backitem);
                   ;
                    this->setCurrentItem(backitem);

                    this->setCurrentItem(backitem);
                }else{
                    QListWidgetItem *backitem=itemAt(endPos);
                    this->setCurrentItem(backitem);
                }


            }else
            {
                rowCount = count();
                insertItem(rowCount,mimeData->text());
                this->setCurrentItem(item(count()-1));//设置为最后一行被选中
            }
            if(changedrow!=rowCount)
            {
                qDebug("datchanged:%d",rowCount);
                emit OneListWidget_ListDataChanged(objectName());
            }

        }
        event->setDropAction(Qt::MoveAction);
        event->accept();

    }
}
bool OneListWidget:: contains_text(QString text){
    for(int i=0;i<this->count();i++){
        if(item(i)->text()==text){
            return true;
        }
    }
    return false;
}
