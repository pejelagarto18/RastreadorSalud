#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <QString>

class CSVExporter {
public:
    static bool exportToCSV(const QString &filePath);
};

#endif // CSVEXPORTER_H
