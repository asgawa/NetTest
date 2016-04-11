#include "customprotocol.h"
#include "ui_customprotocol.h"

CustomProtocol::CustomProtocol(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomProtocol)
{
    ui->setupUi(this);
}

CustomProtocol::~CustomProtocol()
{
    delete ui;
}
