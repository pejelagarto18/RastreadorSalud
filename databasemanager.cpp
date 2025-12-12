#include "databasemanager.h"
#include <QSqlQuery>
#include <QDebug>

DatabaseManager::DatabaseManager() {}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::openDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("health_tracker.db");

    if (!db.open()) {
        qDebug() << "❌ Error al abrir la base de datos.";
        return false;
    }

    QSqlQuery query;

    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT UNIQUE,"
               "password TEXT)");

    query.exec("CREATE TABLE IF NOT EXISTS health_records ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "user_id INTEGER,"
               "date_time TEXT,"
               "weight REAL,"
               "blood_pressure REAL,"
               "glucose_level REAL,"
               "FOREIGN KEY(user_id) REFERENCES users(id))");

    return true;
}

QSqlDatabase& DatabaseManager::getDatabase() {
    return db;
}
