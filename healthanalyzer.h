#ifndef HEALTHANALYZER_H
#define HEALTHANALYZER_H

#include <QSqlQuery>
#include <QVariant>

class HealthAnalyzer {
public:
    static double calculateAverage(const QString &column);
};

#endif // HEALTHANALYZER_H
