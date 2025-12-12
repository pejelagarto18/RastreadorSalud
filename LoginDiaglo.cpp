#include "logindialog.h"
#include "ui_logindialog.h"     // ⚠️ Este include es obligatorio, no lo elimines
#include "databasemanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

/**
 * @brief Constructor del diálogo de inicio de sesión.
 * @param parent Ventana padre.
 */
LoginDialog::logindialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , loggedUserId(-1)  // inicializar el ID por defecto
{
    ui->setupUi(this);

    // Opcional: configurar campos
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
}

/**
 * @brief Destructor: libera memoria de la interfaz.
 */
LoginDialog::~LoginDialog()
{
    delete ui;
}

/**
 * @brief Devuelve el ID del usuario que inició sesión.
 */
int LoginDialog::getUserId() const
{
    return loggedUserId;
}

/**
 * @brief Slot: botón "Iniciar sesión".
 */
void LoginDialog::on_loginButton_clicked()
{
    // Obtener datos del formulario
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Campos vacíos", "Por favor ingresa usuario y contraseña.");
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
        accept();  // cerrar el diálogo con éxito
    } else {
        QMessageBox::warning(this, "Error", "Usuario o contraseña incorrectos.");
    }
}

/**
 * @brief Slot: botón "Registrar usuario".
 */
void LoginDialog::on_registerButton_clicked()
{
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Campos vacíos", "Debes ingresar usuario y contraseña.");
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
