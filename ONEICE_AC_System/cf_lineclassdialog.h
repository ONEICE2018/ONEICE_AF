#ifndef CF_LINECLASSDIALOG_H
#define CF_LINECLASSDIALOG_H
#include <QDebug>
#include <QDialog>
#include <curve_fitting.h>

//文件操作
#include <qfile>
#include <QTextStream>
#include <QDir>

namespace Ui
{
    class CF_LineClassDialog;
}

class CF_LineClassDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CF_LineClassDialog(QWidget* parent = 0);
    ~CF_LineClassDialog();
signals:
    void LC_append(QString);
private slots:

    void on_LC_newLineClass_clicked();

    void on_buttonBox_clicked(QAbstractButton* button);

    void on_LC_FindClassID_clicked();

private:
    Ui::CF_LineClassDialog* ui;
    QString Index = NULL, Input = NULL, Output = NULL, NowLC = NULL;
    void LC_DataChanged();
    QMap<QString, int> ClassID;
    void showmsg(QString msg);
    int MaxClassID = 0;
    void init();

    ///文件操作
    QString LC_BasePath;//=QCoreApplication::applicationDirPath()+"\\LC_Data";
    QString LC_filename;//="LCDATA.bin";



    //读取
    int LC_Load(QString path, QString name)
    {
        QFile f(QString(path + "\\" + name).replace("/", "\\"));
        if(!f.open(QIODevice::ReadWrite  | QIODevice::Text))
        {
            return 0;
        }
        QTextStream txtInput(&f);
        QString lineStr;
        while(!txtInput.atEnd())
        {
            QString line = txtInput.readLine();
            lineStr = QString(line.toLocal8Bit());
            if(line.split(",").length() == 5)
            {
                QString rindex = line.split(",")[1],
                        rvarible = line.split(",")[2],
                        rinput = line.split(",")[3],
                        routput = line.split(",")[4];
                QString temp = QString("%1,%2,%3,%4").arg(rindex).arg(rvarible).arg(rinput).arg(routput);
                if(!ClassID.contains(temp))
                {
                    ClassID.insert(temp, QString(line.split(",")[0]).toInt());
                }
                else
                {
                    qDebug() << "load reCover erro" << line;
                }
            }
            else
            {
                qDebug() << "load Line erro" << line;
            }
            qDebug() << lineStr;
        }
        f.close();
        return 1;
    }
    int LC_Clear(QString path, QString name)
    {
        QDir dir;
        if(!dir.exists(path))
        {
            dir.mkdir(path);
        }
        QFile f(QString(path + "\\" + name).replace("/", "\\"));
        if(!f.open(QIODevice::WriteOnly | QIODevice::Text))//不存在会自动创建
        {
            return -1;
        }
        QTextStream txtOutput(&f);
        txtOutput << "";
        f.close();
    }
    //写入
    int LC_Write_ONE(QString path, QString name, QString data)
    {
        QDir dir;
        if(!dir.exists(path))
        {
            dir.mkdir(path);
        }
        QFile f(QString(path + "\\" + name).replace("/", "\\"));
        if(!f.open(QIODevice::Append | QIODevice::Text))//不存在会自动创建
        {
            return -1;
        }
        QString Context = QString("%1\n").arg(data);
        QTextStream txtOutput(&f);
        txtOutput << Context;
        f.close();
    }

};

#endif // CF_LINECLASSDIALOG_H
