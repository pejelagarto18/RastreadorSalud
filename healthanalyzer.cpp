#include "healthanalyzer.h"
#include "databasemanager.h"
#include <QSqlError>
#include <QDebug>

double HealthAnalyzer::calculateAverage(const QString &column)
{
    QSqlQuery query(DatabaseManager::instance().getDatabase());
    query.prepare(QString("SELECT AVG(%1) FROM health_records").arg(column));

    if (!query.exec()) {
        qDebug() << "Error calculando promedio:" << query.lastError().text();
        return 0;
    }

    if (query.next()) {
        return query.value(0).toDouble();
    }

    return 0;
}
