#include "capturedialog.hpp"
#include "ui_capturedialog.h"

CaptureDialog::CaptureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaptureDialog)
{
    ui->setupUi(this);
}

CaptureDialog::~CaptureDialog()
{
    delete ui;
}

void CaptureDialog::setItems(int itemnum)
{
    for (int i = 0; i < itemnum; ++i) {
        ui->deviceNum->addItem(QString::number(i));
    }
}

int CaptureDialog::getDeviceNumber()
{
    return ui->deviceNum->currentText().toInt();
}

QString CaptureDialog::getFileName()
{
    if(ui->groupBox->isChecked()){
        return ui->fileName->text();
    }else{
        return "";
    }
}

int CaptureDialog::getFps()
{
    if(ui->groupBox->isChecked()){
        return ui->fps->value();
    }else{
        return 0;
    }
}

bool CaptureDialog::isRecord()
{
    return ui->groupBox->isChecked();
}
