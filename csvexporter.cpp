#include "csvexporter.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QFile>
#include <QTextStream>
#include <QDebug>

bool CSVExporter::exportToCSV(const QString &filePath)
{
    QSqlQuery query("SELECT date_time, weight, blood_pressure, glucose_level FROM health_records",
                    DatabaseManager::instance().getDatabase());

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "Fecha/Hora,Peso,Presion,Glucosa\n";

    while (query.next()) {
        out << query.value(0).toString() << ","
            << query.value(1).toString() << ","
            << query.value(2).toString() << ","
            << query.value(3).toString() << "\n";
    }

    file.close();
    return true;
}
