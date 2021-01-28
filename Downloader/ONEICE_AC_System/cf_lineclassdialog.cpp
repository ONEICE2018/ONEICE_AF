#include "cf_lineclassdialog.h"
#include "ui_cf_lineclassdialog.h"

CF_LineClassDialog::CF_LineClassDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CF_LineClassDialog)
{
    ui->setupUi(this);
    init();
}

CF_LineClassDialog::~CF_LineClassDialog()
{
    delete ui;

}
void CF_LineClassDialog::init(){


   LC_BasePath=QCoreApplication::applicationDirPath();
   LC_filename="LCDATA.txt";
   LC_BasePath.append("\\LC_Data");
   //fileWriter(LC_BasePath,LC_filename,"writtest");
   LC_Load(LC_BasePath,LC_filename);
   QMapIterator<QString, int> i(ClassID);
   while (i.hasNext())
   {
        i.next();
        QString SavedLine=QString("%1,%2").arg(i.value()).arg(i.key());
        ui->comboBox->addItem(SavedLine);
   }


}



void CF_LineClassDialog::on_LC_newLineClass_clicked()
{
    if(  ui->LC_comboBox_Index->currentText()!=NULL&&ui->LC_comboBox_Index->currentText()!=""
       &&ui->LC_comboBox_Input->currentText()!=NULL&&ui->LC_comboBox_Input->currentText()!=""
       &&ui->LC_comboBox_Output->currentText()!=NULL&&ui->LC_comboBox_Output->currentText()!=""
       &&ui->LC_comboBox_variable->currentText()!=NULL&&ui->LC_comboBox_variable->currentText()!=""
                        )
    {
                          //索引 自变量 两个结果因变量
        QString temp=QString("%1,%2,%3,%4").arg(ui->LC_comboBox_Index->currentText()).arg(ui->LC_comboBox_variable->currentText()).arg(ui->LC_comboBox_Input->currentText()).arg(ui->LC_comboBox_Output->currentText());
        if(!ClassID.contains(temp))
        {

            MaxClassID=0;
            while(ClassID.values().contains(MaxClassID)){
              MaxClassID++;
            }

            ClassID.insert(temp,MaxClassID);
            showmsg(QString("ClassID: %1 distributed").arg(MaxClassID));
            ui->LC_Text_ClassID->setText(QString("ClassID:%1").arg(ClassID.value(temp)));
            ui->comboBox->addItem(QString("%1,%2").arg(MaxClassID).arg(temp));
            //先清空文件
            LC_Clear(LC_BasePath,LC_filename);
            //遍历写入
            QMapIterator<QString, int> i(ClassID);
            while (i.hasNext()) {
                 i.next();
                 QString SaveLine=QString("%1,%2").arg(i.value()).arg(i.key());
                 LC_Write_ONE(LC_BasePath,LC_filename,SaveLine);
            }
        }else
        {
            showmsg(QString("ClassID:%1 has been distributed").arg(ClassID.value(temp)));
            ui->LC_Text_ClassID->setText(QString("ClassID:%1").arg(ClassID.value(temp)));
        }
    }
}
void CF_LineClassDialog:: showmsg(QString msg)
{
    ui->LC_Msg->setText(msg);
}

void CF_LineClassDialog::on_buttonBox_clicked(QAbstractButton *button)
{
//if(button==QAbstractButton.)
    if(ui->buttonBox->button(QDialogButtonBox::Ok)  == button) {
        if(ui->comboBox->currentText()!=NULL&&ui->comboBox->currentText()!=""){
           emit  LC_append( ui->comboBox->currentText() );
        }
    }

}

void CF_LineClassDialog::on_LC_FindClassID_clicked()
{

    if(  ui->LC_comboBox_Index->currentText()!=NULL&&ui->LC_comboBox_Index->currentText()!=""
       &&ui->LC_comboBox_Input->currentText()!=NULL&&ui->LC_comboBox_Input->currentText()!=""
       &&ui->LC_comboBox_Output->currentText()!=NULL&&ui->LC_comboBox_Output->currentText()!=""
                        )
    {
        QString temp=QString("%1,%2,%3").arg(ui->LC_comboBox_Index->currentText()).arg(ui->LC_comboBox_Input->currentText()).arg(ui->LC_comboBox_Output->currentText());
        if(ClassID.contains(temp))
        {
            showmsg(QString("find ClassID:%1").arg(ClassID.value(temp)));
            ui->LC_Text_ClassID->setText(QString("ClassID:%1").arg(ClassID.value(temp)));

        }else
        {
               showmsg( QString("ClassID not find") );
        }

    }
}
