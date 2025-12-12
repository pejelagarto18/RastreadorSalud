#include "addrecorddialog.h"
#include "ui_addrecorddialog.h"

AddRecordDialog::AddRecordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddRecordDialog)
{
    ui->setupUi(this);
}

AddRecordDialog::~AddRecordDialog()
{
    delete ui;
}

double AddRecordDialog::getWeight() const
{
    return ui->weightEdit->text().toDouble();
}

double AddRecordDialog::getPressure() const
{
    return ui->pressureEdit->text().toDouble();
}

double AddRecordDialog::getGlucose() const
{
    return ui->glucoseEdit->text().toDouble();
}
