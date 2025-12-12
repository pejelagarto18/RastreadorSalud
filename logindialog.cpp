#include "logindialog.h"
#include "ui_logindialog.h"   // 👈 este include es imprescindible
#include "databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

/**
 * @brief Constructor del diálogo de inicio de sesión.
 * @param parent Ventana padre.
 */
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , loggedUserId(-1)  // inicializar el ID por defecto
{
    ui->setupUi(this);

    // Configurar el campo de contraseña
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
}

/**
 * @brief Destructor: libera la interfaz.
 */
LoginDialog::~LoginDialog()
{
    delete ui;
}

/**
 * @brief Devuelve el ID del usuario autenticado.
 * @return ID del usuario si inició sesión, -1 si no.
 */
int LoginDialog::getUserId() const
{
    return loggedUserId;
}

/**
 * @brief Evento al hacer clic en "Iniciar sesión".
 */
void LoginDialog::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Campos vacíos",
                             "Por favor ingresa usuario y contraseña.");
        return;
    }

    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT id FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        QMessageBox::critical(this, "Error de base de datos",
                              "No se pudo verificar el usuario: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        loggedUserId = query.value(0).toInt();
        accept();  // cerrar el diálogo exitosamente
    } else {
        QMessageBox::warning(this, "Error", "Usuario o contraseña incorrectos.");
    }
}

/**
 * @brief Evento al hacer clic en "Registrar usuario".
 */
void LoginDialog::on_registerButton_clicked()
{
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Campos vacíos",
                             "Debes ingresar usuario y contraseña.");
        return;
    }

    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec()) {
        QMessageBox::warning(this, "Error",
                             "No se pudo registrar el usuario: " + query.lastError().text());
    } else {
        QMessageBox::information(this, "Éxito",
                                 "Usuario registrado correctamente. Ahora puedes iniciar sesión.");
    }
}
