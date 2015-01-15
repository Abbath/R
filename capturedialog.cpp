#include "capturedialog.hpp"
#include "ui_capturedialog.h"
#include <QFileDialog>

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

void CaptureDialog::setItems(QStringList items)
{
        ui->deviceNum->addItems(items);   
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

void CaptureDialog::on_pushButton_clicked()
{
    QString name = QFileDialog::getSaveFileName(this, "Save video", ".", "Video files (*.avi)");
    if(!name.endsWith(".avi")){
        name += ".avi";
    }
    ui->fileName->setText(name);
}
