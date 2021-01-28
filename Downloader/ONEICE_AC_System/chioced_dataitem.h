#ifndef CHIOCED_DATAITEM_H
#define CHIOCED_DATAITEM_H

#include <QWidget>

namespace Ui {
class chioced_dataitem;
}

class chioced_dataitem : public QWidget
{
    Q_OBJECT

public:
    explicit chioced_dataitem(QWidget *parent = 0);
    ~chioced_dataitem();

private:
    Ui::chioced_dataitem *ui;
    QString name;
};

#endif // CHIOCED_DATAITEM_H
