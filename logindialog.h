#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    int getUserId() const; // retorna el ID del usuario autenticado

private slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked();

private:
    Ui::LoginDialog *ui;
    int loggedUserId = -1;
};

#endif // LOGINDIALOG_H
