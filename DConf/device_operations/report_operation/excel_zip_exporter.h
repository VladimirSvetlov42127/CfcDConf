#ifndef EXCEL_ZIP_EXPORTER_H
#define EXCEL_ZIP_EXPORTER_H

#include "data_model/parameters/parameter.h"

class ExcelZipExporter
{
public:
    ExcelZipExporter();

    bool convertToExcel(const std::vector<Parameter::UPtr> &paramList, const QString &dirPath);
};

#endif // EXCEL_ZIP_EXPORTER_H
