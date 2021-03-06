#ifndef CAPTUREDIALOG_HPP
#define CAPTUREDIALOG_HPP

#include <QDialog>

namespace Ui {
class CaptureDialog;
}

class CaptureDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CaptureDialog(QWidget *parent = 0);
    ~CaptureDialog();
    void setItems(QStringList items);
    int getDeviceNumber();
    QString getFileName();
    int getFps();
    bool isRecord();
    
    void testSetItems();
private slots:
    void on_pushButton_clicked();
    
private:
    Ui::CaptureDialog *ui;
};

#endif // CAPTUREDIALOG_HPP
