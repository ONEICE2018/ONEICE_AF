#ifndef ONEFILEMANAGE_H
#define ONEFILEMANAGE_H
//文件关闭不好弄
#include <QObject>

class ONEFileManage : public QObject
{
    Q_OBJECT
public:
    explicit ONEFileManage(QObject *parent = nullptr);

signals:

public slots:
};

#endif // ONEFILEMANAGE_H
