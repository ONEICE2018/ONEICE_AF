#include "chioced_dataitem.h"
#include "ui_chioced_dataitem.h"

chioced_dataitem::chioced_dataitem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chioced_dataitem)
{
    ui->setupUi(this);
}

chioced_dataitem::~chioced_dataitem()
{
    delete ui;
}
