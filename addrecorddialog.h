#ifndef ADDRECORDDIALOG_H
#define ADDRECORDDIALOG_H

#include <QDialog>

namespace Ui {
class AddRecordDialog;
}

/**
 * @brief Ventana para ingresar un nuevo registro de salud.
 */
class AddRecordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddRecordDialog(QWidget *parent = nullptr);
    ~AddRecordDialog();

    double getWeight() const;   // ← getter para peso
    double getPressure() const; // ← getter para presión
    double getGlucose() const;  // ← getter para glucosa

private:
    Ui::AddRecordDialog *ui;
};

#endif // ADDRECORDDIALOG_H
