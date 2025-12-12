#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool openDatabase();
    QSqlDatabase& getDatabase();

private:
    DatabaseManager();
    QSqlDatabase db;
};

#endif // DATABASEMANAGER_H
