#ifndef CUSTOMPROTOCOL_H
#define CUSTOMPROTOCOL_H

#include <QDialog>

namespace Ui {
class CustomProtocol;
}

class CustomProtocol : public QDialog
{
    Q_OBJECT

public:
    explicit CustomProtocol(QWidget *parent = 0);
    ~CustomProtocol();

private:
    Ui::CustomProtocol *ui;
};

#endif // CUSTOMPROTOCOL_H
