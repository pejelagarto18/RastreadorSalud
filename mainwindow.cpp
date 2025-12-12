#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addrecorddialog.h"
#include "databasemanager.h"
#include "healthanalyzer.h"
#include "csvexporter.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QFileDialog>

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

// ------------------------------------------------------------
// FUNCIÓN AUXILIAR: LOGIN Y REGISTRO DE USUARIOS
// ------------------------------------------------------------
int MainWindow::showLoginDialog()
{
    QDialog loginDialog(this);
    loginDialog.setWindowTitle("Iniciar sesión");
    loginDialog.setModal(true);
    loginDialog.resize(300, 180);

    QVBoxLayout *layout = new QVBoxLayout(&loginDialog);

    QLabel *userLabel = new QLabel("Usuario:");
    QLineEdit *usernameEdit = new QLineEdit();
    QLabel *passLabel = new QLabel("Contraseña:");
    QLineEdit *passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *loginButton = new QPushButton("Iniciar sesión");
    QPushButton *registerButton = new QPushButton("Registrar usuario");

    layout->addWidget(userLabel);
    layout->addWidget(usernameEdit);
    layout->addWidget(passLabel);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);

    int loggedUserId = -1;

    // --- LOGIN ---
    QObject::connect(loginButton, &QPushButton::clicked, [&]() {
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text().trimmed();

        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(&loginDialog, "Campos vacíos", "Por favor ingresa usuario y contraseña.");
            return;
        }

        QSqlQuery query(DatabaseManager::instance().getDatabase());
        query.prepare("SELECT id FROM users WHERE username = :username AND password = :password");
        query.bindValue(":username", username);
        query.bindValue(":password", password);

        if (query.exec() && query.next()) {
            loggedUserId = query.value(0).toInt();
            loginDialog.accept();
        } else {
            QMessageBox::warning(&loginDialog, "Error", "Usuario o contraseña incorrectos.");
        }
    });

    // --- REGISTRO ---
    QObject::connect(registerButton, &QPushButton::clicked, [&]() {
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text().trimmed();

        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(&loginDialog, "Campos vacíos", "Debes ingresar usuario y contraseña.");
            return;
        }

        QSqlQuery query(DatabaseManager::instance().getDatabase());
        query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
        query.bindValue(":username", username);
        query.bindValue(":password", password);

        if (!query.exec()) {
            QMessageBox::warning(&loginDialog, "Error",
                                 "No se pudo registrar el usuario: " + query.lastError().text());
        } else {
            QMessageBox::information(&loginDialog, "Éxito",
                                     "Usuario registrado correctamente. Ahora puedes iniciar sesión.");
        }
    });

    loginDialog.exec();
    return loggedUserId;
}

// ------------------------------------------------------------
// CONSTRUCTOR Y CONFIGURACIÓN INICIAL
// ------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Abrir base de datos
    if (!DatabaseManager::instance().openDatabase()) {
        QMessageBox::critical(this, "Error", "No se pudo abrir la base de datos.");
        close();
        return;
    }

    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);

    // Crear tabla de usuarios si no existe
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT UNIQUE, "
               "password TEXT)");

    // Verificar si la tabla health_records tiene columna user_id
    bool hasUserId = false;
    if (query.exec("PRAGMA table_info(health_records)")) {
        while (query.next()) {
            if (query.value(1).toString() == "user_id") {
                hasUserId = true;
                break;
            }
        }
    }

    // Si no tiene user_id, recreamos correctamente la tabla
    if (!hasUserId) {
        qDebug() << "⚙️  Actualizando estructura de health_records...";
        query.exec("DROP TABLE IF EXISTS health_records");
        query.exec("CREATE TABLE health_records ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "user_id INTEGER, "
                   "date_time TEXT, "
                   "weight REAL, "
                   "blood_pressure REAL, "
                   "glucose_level REAL, "
                   "FOREIGN KEY(user_id) REFERENCES users(id))");

        QMessageBox::information(this, "Base de datos actualizada",
                                 "La tabla de registros se ha actualizado correctamente.");
    }

    // Mostrar login al iniciar
    currentUserId = showLoginDialog();
    if (currentUserId == -1) {
        close();
        return;
    }

    loadRecords();
}

// ------------------------------------------------------------
// DESTRUCTOR
// ------------------------------------------------------------
MainWindow::~MainWindow()
{
    delete ui;
}

// ------------------------------------------------------------
// BOTÓN: AGREGAR REGISTRO
// ------------------------------------------------------------
void MainWindow::on_addRecordButton_clicked()
{
    AddRecordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {

        double peso = dialog.getWeight();
        double presion = dialog.getPressure();
        double glucosa = dialog.getGlucose();
        QString fechaHora = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        QSqlQuery query(DatabaseManager::instance().getDatabase());
        query.prepare("INSERT INTO health_records (user_id, date_time, weight, blood_pressure, glucose_level) "
                      "VALUES (:user_id, :date_time, :weight, :blood_pressure, :glucose_level)");
        query.bindValue(":user_id", currentUserId);
        query.bindValue(":date_time", fechaHora);
        query.bindValue(":weight", peso);
        query.bindValue(":blood_pressure", presion);
        query.bindValue(":glucose_level", glucosa);

        if (!query.exec()) {
            QMessageBox::critical(this, "Error",
                                  "No se pudo insertar el registro: " + query.lastError().text());
        } else {
            QMessageBox::information(this, "Éxito", "Registro guardado correctamente.");
            loadRecords();
        }
    }
}

// ------------------------------------------------------------
// CARGAR REGISTROS DESDE LA BASE DE DATOS
// ------------------------------------------------------------
void MainWindow::loadRecords()
{
    ui->recordsTable->setRowCount(0);

    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT date_time, weight, blood_pressure, glucose_level "
                  "FROM health_records WHERE user_id = :user_id");
    query.bindValue(":user_id", currentUserId);
    query.exec();

    int row = 0;
    while (query.next()) {
        ui->recordsTable->insertRow(row);

        ui->recordsTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->recordsTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->recordsTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        ui->recordsTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));

        row++;
    }

    updateAverages();
}

// ------------------------------------------------------------
// ACTUALIZAR PROMEDIOS AUTOMÁTICAMENTE
// ------------------------------------------------------------
void MainWindow::updateAverages()
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare("SELECT AVG(weight), AVG(blood_pressure), AVG(glucose_level) "
                  "FROM health_records WHERE user_id = :user_id");
    query.bindValue(":user_id", currentUserId);
    query.exec();

    if (query.next()) {
        double avgWeight = query.value(0).toDouble();
        double avgPressure = query.value(1).toDouble();
        double avgGlucose = query.value(2).toDouble();

        ui->avgWeightLabel->setText(QString("Promedio Peso: %1 kg").arg(avgWeight, 0, 'f', 1));
        ui->avgPressureLabel->setText(QString("Promedio Presión: %1 mmHg").arg(avgPressure, 0, 'f', 1));
        ui->avgGlucoselabel->setText(QString("Promedio Glucosa: %1 mg/dL").arg(avgGlucose, 0, 'f', 1));
    }
}

// ------------------------------------------------------------
// BOTÓN: EXPORTAR A CSV
// ------------------------------------------------------------
void MainWindow::on_exportCSVButton_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Guardar CSV", "", "Archivos CSV (*.csv)");

    if (!filePath.isEmpty()) {
        if (CSVExporter::exportToCSV(filePath))
            QMessageBox::information(this, "Éxito", "Datos exportados correctamente.");
        else
            QMessageBox::critical(this, "Error", "No se pudo exportar el archivo CSV.");
    }
}
