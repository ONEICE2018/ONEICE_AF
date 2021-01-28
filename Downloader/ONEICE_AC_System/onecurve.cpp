#include "onecurve.h"
#include "ui_onecurve.h"


Drowcuvertime::Drowcuvertime(QObject *parent) :
    QThread(parent)
{

}
void Drowcuvertime::run()
{

}
OneCurve::OneCurve(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OneCurve)
{
    ui->setupUi(this);
    init();
}

OneCurve::~OneCurve()
{
    delete ui;
}
void OneCurve::init(){
     this->installEventFilter(this);
     ui->refernceline->setStyleSheet("");
     ui->axis_lockX->setStyleSheet("");
     ui->axis_lockY->setStyleSheet("");

    /*******菜********单******/
    ui->list_curves->setDifferent(1);
    ui->list_curves->setViewMode(QListView::ListMode);
    //设置列表从左往右排列
    ui->list_curves->setFlow(QListView::LeftToRight);
    //屏蔽水平与垂直的滑动条
    ui->list_curves->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->list_curves->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //设置为像素滚动
    ui->list_curves->setHorizontalScrollMode(QListWidget::ScrollPerPixel);
    //设置鼠标左键拖动
    // QScroller::grabGesture(ui->list_curves,QScroller::LeftMouseButtonGesture);
    ui->list_curves->setStyleSheet(R"(
           QListWidget { outline: none; border:1px solid gray; color: black; }
           QListWidget::Item { width: 50px; height: 50px; }
           QListWidget::Item:hover { background: #4CAF50; color: white; }
           QListWidget::item:selected { background: #e7e7e7; color: #f44336; }
           QListWidget::item:selected:!active { background: lightgreen; }
                           )");
    connect(ui->list_curves,SIGNAL(OneListWidget_ListDataChanged(QString)), this, SLOT(list_curves_datachanged(QString)));
    connect(ui->list_curves,SIGNAL(OneListWidget_DelatLastItem(QString)), this, SLOT(list_curves_lastonedelat(QString)));
    connect(ui->list_curves, SIGNAL(OneListWidget_ItemClicked(int)),this, SLOT(ON_list_currentClicked(int)));

    //初始化坐标极限
    limit_X=4095;
    limit_Y=6000;
    ui->limit_X->setText(QString("%1").arg(limit_X));
    ui->limit_Y->setText(QString("%1").arg(limit_Y));

    /***曲线区****/
    axis_start_x=0;
    axis_start_y=0;
    axis_stop_x=limit_X;
    axis_stop_y=limit_Y;
    //隐藏编辑悬浮窗
    show_lineEditer(false,false);
    //初始化界面 让按钮事件得以响应
    on_curveZero_clicked();

    //画点测试：
    //    FinalDatas tetsdata;
    //    for(int i=0;i<30;i++ ){
    //       tetsdata.KVS.insert(i*100,i*100);
    //    }
    //    drawOnePoint(2450,tetsdata);

}
void OneCurve::refcurve(){

     update();

}
int  OneCurve::getX(int x){
 return x;
}
int  OneCurve::getY(int y){
    return view_H-y;
}

void OneCurve:: show_lineEditer(bool showxy,bool show){

    ui->widget_chioedEditer->setVisible(show);
    ui->edit_X->setVisible(showxy);
    ui->edit_Y->setVisible(showxy);
    ui->choiced_X->setVisible(showxy);
    ui->choiced_Y->setVisible(showxy);

}

void OneCurve::drawOnePoint(uint16_t index,FinalDatas  MAD){
    ONELines.clear();
    ONELines.insert(index,MAD);
    disconnect(ui->list_curves, SIGNAL(currentRowChanged(int)),this, SLOT(on_list_curves_currentRowChanged(int)));
    ui->list_curves->clear();
    connect(ui->list_curves, SIGNAL(currentRowChanged(int)),this, SLOT(on_list_curves_currentRowChanged(int)));
    ui->list_curves->addItem(QString("%1").arg(index));
    update();
}
void OneCurve::drawMorePoint(uint16_t index,FinalDatas MAD){
    ONELines.insert(index,MAD);
    ui->list_curves->addItem(QString("%1").arg(index));
      update();
}

void OneCurve::controller_Keep_Stat(){//各个控件保持相对位置
    ui->menubox->resize(view_W,ui->menubox->height()); //菜单栏
    ui->menubox->move(0,0);
    ui->axis_x->move(getX(view_W-ui->axis_x->width()),getY(disY+ui->axis_x->height()+6));//X轴
    ui->axis_y->move(getX(disX+12),getY(view_H-ui->menubox->height()));
    ui->axis_startpoint->move(getX(0),getY(ui->axis_startpoint->height()));
    //悬浮窗
     ui->widget_chioedEditer->move(getX(view_W-ui->widget_chioedEditer->width()),getY(view_H-ui->menubox->height()));

}

bool  OneCurve::eventFilter(QObject *watched, QEvent *event){
       if(event->type()==QEvent::KeyPress){
            QKeyEvent  *kEvent=static_cast<QKeyEvent *>(event);
           if(kEvent->key()==Qt::Key_C){
               C_OK=true;
           }else

           if(kEvent->key()==Qt::Key_Control){
               ctrl_OK=true;
           }else
           if(ctrl_OK&&kEvent->key()==Qt::Key_R)
           {
               on_refernceline_clicked();
               R_OK=true;
           }
           if(ctrl_OK&&kEvent->key()==Qt::Key_Z){
               on_curveZero_clicked();
           }

       }
       if(event->type()==QEvent::KeyRelease){
           QKeyEvent  *kEvent=static_cast<QKeyEvent *>(event);
           if(kEvent->key()==Qt::Key_Control)
           {
               ctrl_OK=false;
           }
           if(kEvent->key()==Qt::Key_R)
           {

               R_OK=false;
           }
           if(kEvent->key()==Qt::Key_C){
               C_OK=false;
           }
       }

       if(event->type()==QEvent::MouseButtonRelease){
           QMouseEvent *msEvent=static_cast<QMouseEvent *>(event);
           if (msEvent->button() == Qt::LeftButton) {
               cmove_start.setX(0);
               cmove_start.setY(0);
               mouse_left_OK=false;
           }

           if (msEvent->button() == Qt::RightButton) {
               mouse_right_OK=false;
           }
          update();
       }
       if(event->type()==QEvent::MouseButtonPress){
           QMouseEvent *msEvent=static_cast<QMouseEvent *>(event);

           if (msEvent->button() == Qt::LeftButton)
           {
               if(ctrl_OK)
               {
                   cmove_start=msEvent->pos();
               }
               else
               {
                   cmove_start.setX(0);
                   cmove_start.setY(0);
               }
               cchange_start=msEvent->pos();
               PointChoiced=false;//点击空白处取消选中
               lineChoiced=false ;//点击空白处取消选中
               ChoiceP_OK=true;
               ClikPoint= msEvent->pos();
               mouse_left_OK=true;
           }

           if (msEvent->button() == Qt::RightButton) {
               mouse_right_OK=true;
               //ChoiceP_OK=false;  //右键取消选中
                PointChoiced=false;
           }
          update();
       }
       if(event->type()==QEvent::MouseMove){
           QMouseEvent *msEvent=static_cast<QMouseEvent *>(event);
           endpos = msEvent->pos();
           if (ctrl_OK&&mouse_left_OK)
           {
               if(cmove_start.x()!=0&&cmove_start.y()!=0){
                   if(!Axis_x_lock){
                       int disx=(axis_stop_x-axis_start_x)*(1.0*(endpos.x()-cmove_start.x())/view_W);
                       axis_start_x=axis_start_x-disx;
                       axis_stop_x=axis_stop_x-disx;

                       if(axis_start_x<0||axis_stop_x<0||axis_stop_x>limit_X||axis_start_x>limit_X){
                           axis_start_x=axis_start_x+disx;
                           axis_stop_x=axis_stop_x+disx;
                       }

                   }
                   if(!Axis_y_lock){
                       int disy=(axis_stop_y-axis_start_y)*(1.0*(endpos.y()-cmove_start.y())/view_H);
                       axis_start_y=axis_start_y+disy;
                       axis_stop_y=axis_stop_y+disy;

                       if(axis_start_y<0||axis_stop_y<0||axis_stop_y>limit_Y||axis_start_y>limit_Y){
                           axis_start_y=axis_start_y-disy;
                           axis_stop_y=axis_stop_y-disy;
                       }
                   }
               }
           }
           if(C_OK){
               if(PointChoiced)
               {
                   if(ONELines.contains(Choicedindex))
                   {
                       if(ONELines.value(Choicedindex).KVS.contains(ChoicedPoint.x()))
                       {

                           int disy=(axis_stop_y-axis_start_y)*(1.0*(endpos.y()-cchange_start.y())/view_H);
                           if(ChoicedPoint.y()-disy<0)
                           {
                               disy=ChoicedPoint.y();
                           }
                           if(ChoicedPoint.y()-disy>limit_Y){
                              disy=ChoicedPoint.y()-limit_Y;
                           }
                           FinalDatas ft=ONELines.value(Choicedindex);
                           ft.KVS.remove(ChoicedPoint.x());
                           ft.KVS.insert(ChoicedPoint.x(),ChoicedPoint.y()-disy);

                           ChoicedPoint.setY(ChoicedPoint.y()-disy);
                           ONELines.insert(Choicedindex,ft);
                           emit on_OneCurver_Datahchanged(Choicedindex,ft);

                       }

                  }
               }

           }

          cchange_start=cmove_start=endpos;

           update();
       }

       if(event->type() == QEvent::Resize){
           view_W=this->width();
           view_H=this->height();
           controller_Keep_Stat();
           update();
       }
       if(ctrl_OK&&event->type()==QEvent::Wheel){//ctrl组合滚轮为曲线缩放
           QWheelEvent *myEvent = static_cast<QWheelEvent *>(event);
           QPoint wheelpos=myEvent->pos();

           int wpx=getX(wheelpos.x());
           int wpy=getY(wheelpos.y());

           int XSPAN= axis_stop_x - axis_start_x;  //每次移动0.2倍总带宽
           int YSPAN= axis_stop_y-axis_start_y;  //每次移动0.2倍总带宽
           //int tx=axis_start_x+XSPAN*((1.0*wpx)/view_W);
           //int ty=axis_start_y+YSPAN*(1.0*wpy/view_H);
           //ui->test_xy->setText(QString("tets(%1,%2)").arg(tx).arg(ty));
           float sj_left= (1.0*wpx)/view_W  ;//x轴左右移动量
           float sj_bottem= (1.0*wpy)/view_H;//y轴上下缩进量

           if(myEvent->delta()>0){//放大
               if(!Axis_x_lock)
               {
                   XSPAN=2*((XSPAN*0.05)/2);
                   int leftremove=sj_left*XSPAN;
                   int rightmove= XSPAN-leftremove;
                   int after_stop_x = axis_stop_x-rightmove;
                   int after_start_x= axis_start_x+leftremove;
                   if(after_stop_x>after_start_x){
                        int afterXSPAN=after_stop_x-after_start_x;//修改后的span

                        if(afterXSPAN>MIN_X_SPAN){

                            if(after_start_x>0){
                               axis_start_x = after_start_x;
                            }else{
                               axis_start_x = 0;
                            }


                            if(after_stop_x<limit_X){
                               axis_stop_x= after_stop_x;
                            }else{
                               axis_stop_x=limit_X;
                            }

                        }

                   }
               }

               if(!Axis_y_lock)
               {
                   YSPAN=2*((YSPAN*0.05)/2);
                   int leftremove=sj_bottem*YSPAN;
                   int rightmove= YSPAN-leftremove;
                   int after_stop_y= axis_stop_y-rightmove;
                   int after_start_y= axis_start_y+leftremove;
                   if(after_stop_y>after_start_y){
                        int afterYSPAN=after_stop_y-after_start_y;//修改后的span

                        if(afterYSPAN>MIN_Y_SPAN){

                            if(after_start_y>0){
                               axis_start_y= after_start_y;
                            }else{
                               axis_start_y= 0;
                            }

                            if(after_stop_y<limit_Y){
                               axis_stop_y= after_stop_y;
                            }else{
                               axis_stop_y=limit_Y;
                            }

                        }
                   }
               }
           }else{//缩小
               if(!Axis_x_lock)
               {
                   XSPAN=2*((XSPAN*0.05)/2);
                   int leftremove=sj_left*XSPAN;
                   int rightmove= XSPAN-leftremove;
                   int after_stop_x= axis_stop_x+rightmove;
                   int after_start_x= axis_start_x-leftremove;
                   if(after_stop_x>after_start_x){
                        int afterXSPAN=after_stop_x-after_start_x;//修改后的span

                        if(afterXSPAN>MIN_X_SPAN){

                            if(after_start_x>0){
                               axis_start_x= after_start_x;
                            }else{
                               axis_start_x= 0;
                            }

                            if(after_stop_x<limit_X){
                               axis_stop_x= after_stop_x;
                            }else{
                               axis_stop_x=limit_X;
                            }

                        }

                   }
               }

               if(!Axis_y_lock)
               {
                   YSPAN=2*((YSPAN*0.05)/2);
                   int leftremove=sj_bottem*YSPAN;
                   int rightmove= YSPAN-leftremove;
                   int after_stop_y= axis_stop_y+rightmove;
                   int after_start_y= axis_start_y-leftremove;
                   if(after_stop_y>after_start_y){
                        int afterYSPAN=after_stop_y-after_start_y;//修改后的span

                        if(afterYSPAN>MIN_Y_SPAN){

                            if(after_start_y>0){
                               axis_start_y= after_start_y;
                            }else{
                               axis_start_y= 0;
                            }

                            if(after_stop_y<limit_Y){
                               axis_stop_y= after_stop_y;
                            }else{
                               axis_stop_y=limit_Y;
                            }

                        }

                   }
               }

           }
           update();
       }

       //绘制图像
       if( event->type() == QEvent::Paint){
            QPainter painter(this);
            // 反走样
            painter.setRenderHint(QPainter::Antialiasing, true);
            // 设置画笔颜色
            painter.setPen(QColor(0, 160, 230));
            // 绘制x轴
            painter.drawLine(QPointF(getX(0),getY(disY)),QPointF( getX(view_W), getY(disY)));

            int XSPAN=axis_stop_x-axis_start_x;  //当前数据宽度
            int YSPAN=axis_stop_y-axis_start_y;

            for(int i=0;i<101;i++)//曲线
            {
                if(i%10==0)
                {
                    painter.drawLine(QPointF(getX((view_W/100.0)*i),getY(disY)),QPointF( getX((view_W/100.0)*i), getY(disY+6)));
                    painter.drawText( getX((view_W/100.0)*i-20), getY(disY+9),QString("%1").arg(axis_start_x+i*XSPAN/100));
                    continue;
                }
                painter.drawLine(QPointF(getX((view_W/100.0)*i),getY(disY)),QPointF( getX((view_W/100.0)*i), getY(disY+3)));
            }

            // 绘制y轴
            painter.drawLine(QPointF(getX(disX),getY(0)),QPointF( getX(disX), getY(view_H)));
            for(int i=0;i<101;i++)//曲线
            {
                if(i%10==0)
                {
                    painter.drawLine(QPointF(getX(disX),getY((view_H/100.0)*i)),QPointF( getX(disX+6), getY((view_H/100.0)*i)));
                    painter.drawText(getX(disX+6+2), getY((view_H/100.0)*i-8),QString("%1").arg(axis_start_y+i*YSPAN/100));
                    continue;
                }
                 painter.drawLine(QPointF(getX(disX),getY((view_H/100.0)*i)),QPointF( getX(disX+3), getY((view_H/100.0)*i)));
            }
            ui->curve_max_x->setText(QString("view_W:%1").arg(axis_stop_x));
            ui->curve_max_y->setText(QString("view_H:%1").arg(axis_stop_y));
            ui->axis_startpoint->setText(QString("start(%1,%2)").arg(axis_start_x).arg(axis_start_y));
            if(drowxyline)
            {
                //设置画笔  黑色 宽5
                painter.setPen(QPen(Qt::black,2,Qt::DashDotLine,Qt::RoundCap));
                // 绘制x轴平行线
                painter.drawLine(QPointF(getX(0),endpos.y()),QPointF( getX(Max_X), endpos.y()));
                // 绘制y轴平行线
                painter.drawLine(QPointF(endpos.x(),getY(0)),QPointF( endpos.x(), getY(view_H)));
                //显示坐标数据
                int tx=axis_start_x+XSPAN*(1.0*getX(endpos.x())/view_W);
                int ty=axis_start_y+YSPAN*(1.0*getY(endpos.y())/view_H);
                painter.drawText(endpos.x(),endpos.y(), QString("CK(%1,%2)").arg(tx).arg(ty));
            }

            //绘制点和曲线

            QMapIterator<uint16_t,FinalDatas>  lineI(ONELines);
            while(lineI.hasNext()){
               lineI.next();
               QPen setedpen;
               QMapIterator<uint16_t,uint16_t> pointI(lineI.value().KVS);
               if(lineChoiced&&lineI.key()==Choicedindex)
               {
                  painter.setPen(setedpen=QPen(Qt::green,5,Qt::SolidLine,Qt::RoundCap));
               }else{
                  painter.setPen(setedpen=QPen(Qt::black,5,Qt::SolidLine,Qt::RoundCap));
               }
               //画曲线
               QPoint oldPoint;
               bool candrawline=false;
               while(pointI.hasNext())
               {
                   pointI.next();
                   const int px=pointI.key()*(lineI.value().mVdata/1500.0),py=pointI.value();
                   {
                       if(px>=axis_start_x&&px<=axis_stop_x&&py>=axis_start_y&&py<=axis_stop_y)
                       {
                           int XSPAN=axis_stop_x-axis_start_x;
                           int YSPAN=axis_stop_y-axis_start_y;
                           int dx=(1.0*(px-axis_start_x)/XSPAN)*view_W;
                           int dy=(1.0*(py-axis_start_y)/YSPAN)*view_H;
                           //转换为世界坐标
                           dx=getX(dx);
                           dy=getY(dy);

                           if(ChoiceP_OK){
                               if(dx-5<ClikPoint.x()&&dx+5>ClikPoint.x()&&
                                  dy-5<ClikPoint.y()&&dy+5>ClikPoint.y()
                                       ){
                                   Choicedindex=lineI.key();
//                                   if(ui->list_curves->currentItem()->text().toInt()!=Choicedindex){
//                                       for(int i=0;i<ONELines.size();i++){
//                                           if(ui->list_curves->item(i)->text().toInt()==Choicedindex){
//                                               ui->list_curves->setCurrentRow(i);
//                                               break;
//                                           }
//                                       }
//                                   }
                                   ChoicedPoint.setX(px);
                                   ChoicedPoint.setY(py);
                                   ChoiceP_OK  =false;
                                   PointChoiced=true;
                                   lineChoiced =true;
                                }

                           }
                           bool cd=false;
                           if(PointChoiced){
                               if(lineI.key()==Choicedindex&&ChoicedPoint.x()==px&&ChoicedPoint.y()==py) {
                                   painter.setPen(QPen(Qt::red,10,Qt::SolidLine,Qt::RoundCap));
                                   cd=true;
                               }
                            }
                           QPoint onepoint(dx,dy);
                           painter.drawPoint(onepoint);
                           if(cd)
                           {
                               cd=false;
                               painter.setPen(setedpen);
                           }
                           if(candrawline)
                           {
                               int painwid=setedpen.width();
                               setedpen.setWidth(1);
                               painter.setPen(setedpen);
                               painter.drawLine(oldPoint,onepoint);
                               setedpen.setWidth(painwid);
                               painter.setPen(setedpen);
                           }
                           oldPoint=onepoint;
                           candrawline=true;//第二次才画
                       }
                   }

              }


            }
            //综合处理后显示悬浮窗
            if(lineChoiced){
                ui->choiced_index->setText(QString("Index:%1").arg(Choicedindex));
                ui->choiced_mvdata->setText(QString("mVData:%1").arg(ONELines.value(Choicedindex).mVdata));
                if(PointChoiced)
                {
                    ui->choiced_X->setText(QString("X:%1").arg(ChoicedPoint.x()));
                    ui->choiced_Y->setText(QString("Y:%1").arg(ChoicedPoint.y()));
                }
            }

            show_lineEditer(PointChoiced,PointChoiced||lineChoiced);

       }
}


void OneCurve::on_refernceline_clicked()
{
    if(ui->refernceline->styleSheet()==""){
        ui->refernceline->setStyleSheet("color: rgb(85, 0, 127);background-color: rgb(0, 255, 0);");
        drowxyline=true;
    }else{
        drowxyline=false;
        ui->refernceline->setStyleSheet("");
        update();
    }


}

void OneCurve::on_axis_lockY_clicked()
{
    if(ui->axis_lockY->styleSheet()==""){
        ui->axis_lockY->setStyleSheet("color: rgb(85, 0, 127);background-color: rgb(0, 255, 0);");
        Axis_y_lock=true;
    }else{
        Axis_y_lock=false;
        ui->axis_lockY->setStyleSheet("");
        update();
    }
}

void OneCurve::on_axis_lockX_clicked()
{
    if(ui->axis_lockX->styleSheet()==""){
        ui->axis_lockX->setStyleSheet("color: rgb(85, 0, 127);background-color: rgb(0, 255, 0);");
        Axis_x_lock=true;
    }else{
        Axis_x_lock=false;
        ui->axis_lockX->setStyleSheet("");
        update();
    }
}


void OneCurve::ON_list_currentClicked(int row){
    if(!lineChoiced || Choicedindex!=ui->list_curves->item(row)->text().toInt() )
    {
        Choicedindex=ui->list_curves->item(row)->text().toInt();
        lineChoiced=true;
        update();
    }

}

void OneCurve::on_list_curves_currentRowChanged(int currentRow)
{
    if(!lineChoiced || Choicedindex!=ui->list_curves->item(currentRow)->text().toInt() )
    {
        Choicedindex=ui->list_curves->item(currentRow)->text().toInt();
        lineChoiced=true;
        update();
    }

}
void OneCurve:: list_curves_datachanged(QString uiname){
    if(uiname==ui->list_curves->objectName()){
        //只支持删除了
        QMapIterator<uint16_t,FinalDatas> lineI(ONELines);
        while(lineI.hasNext()){
            lineI.next();
            if(!ui->list_curves->contains_text(QString("%1").arg(lineI.key())))
            {
              ONELines.remove(lineI.key());
              return;
            }
        }
    }

}

void OneCurve:: list_curves_lastonedelat(QString uiname){//删除最后

    if(uiname==ui->list_curves->objectName())
    {
        disconnect(ui->list_curves, SIGNAL(currentRowChanged(int)),this, SLOT(on_list_curves_currentRowChanged(int)));
        ui->list_curves->removeItemWidget(ui->list_curves->currentItem());
        delete ui->list_curves->currentItem();
        ONELines.clear();
        connect(ui->list_curves, SIGNAL(currentRowChanged(int)),this, SLOT(on_list_curves_currentRowChanged(int)));
        list_curves_datachanged(uiname);
    }

}

void OneCurve::on_lineEdit_maxY_returnPressed()
{
    if(ui->lineEdit_maxY->text().toInt()>100)
    {
        limit_Y=ui->lineEdit_maxY->text().toInt();
        ui->limit_Y->setText(QString("%1").arg(limit_Y));
        on_curveZero_clicked();
    }

}

void OneCurve::on_lineEdit_maxX_returnPressed()
{
    if(ui->lineEdit_maxX->text().toInt()>100)
    {
        limit_X=ui->lineEdit_maxX->text().toInt();
        ui->limit_X->setText(QString("%1").arg(limit_X));
        on_curveZero_clicked();
    }
}

void OneCurve::on_curveZero_clicked()
{
    axis_start_x=0;
    axis_start_y=0;
    axis_stop_x=limit_X;
    axis_stop_y=limit_Y;
    update();
}

void OneCurve::on_edit_mvdata_returnPressed()
{
    if(PointChoiced||lineChoiced)
    {
        if(ui->edit_mvdata->text()!="")
        {
            if(ui->edit_mvdata->text().toInt()>0||ui->edit_mvdata->text()=="0")
            {
                if(ONELines.contains(Choicedindex))
                {
                   FinalDatas ft=ONELines.value(Choicedindex);
                    ft.mVdata=ui->edit_mvdata->text().toInt();
                    ONELines.insert(Choicedindex,ft);
                    emit on_OneCurver_Datahchanged(Choicedindex,ft);
                    update();
                }
            }
        }
    }
}

void OneCurve::on_edit_X_returnPressed()
{
    if(PointChoiced||lineChoiced)
    {
        if(ui->edit_X->text()!="")
        {
            if(ui->edit_X->text().toInt()>0||ui->edit_X->text()=="0")
            {
                if(ONELines.contains(Choicedindex))
                {
                    if(ONELines.value(Choicedindex).KVS.contains(ChoicedPoint.x()))
                    {
                        FinalDatas ft=ONELines.value(Choicedindex);
                        ft.KVS.remove(ChoicedPoint.x());
                        ft.KVS.insert(ui->edit_X->text().toInt(),ChoicedPoint.y());
                        ChoicedPoint.setX(ui->edit_X->text().toInt());
                        ONELines.insert(Choicedindex,ft);
                        emit on_OneCurver_Datahchanged(Choicedindex,ft);
                        update();
                    }

                }
            }
        }
    }
}

void OneCurve::on_edit_Y_returnPressed()
{
    if(PointChoiced||lineChoiced)
    {
        if(ui->edit_Y->text()!="")
        {
            if(ui->edit_Y->text().toInt()>0||ui->edit_Y->text()=="0")
            {
                if(ONELines.contains(Choicedindex))
                {
                    if(ONELines.value(Choicedindex).KVS.contains(ChoicedPoint.x()))
                    {
                        FinalDatas ft=ONELines.value(Choicedindex);
                        ft.KVS.remove(ChoicedPoint.x());
                        ft.KVS.insert(ChoicedPoint.x(),ui->edit_Y->text().toInt());
                        ChoicedPoint.setY(ui->edit_Y->text().toInt());
                        ONELines.insert(Choicedindex,ft);
                        emit on_OneCurver_Datahchanged(Choicedindex,ft);
                        update();
                    }

                }
            }
        }
    }
}
