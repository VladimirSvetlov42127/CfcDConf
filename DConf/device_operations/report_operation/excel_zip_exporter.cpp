#include "excel_zip_exporter.h"

#include <QFileDialog>
#include <QFileInfo>

#include "dpc/sybus/utils.h"

#include "xlsxdocument.h"

namespace {
const std::map<uint8_t, const char *> &getMapType()
{
    static const std::map<uint8_t, const char *> mapType {
        {T_BYTE, "Byte"},
        {T_8BIT, "Bit8"},
        {T_CHAR, "Char"},
        {T_BYTEBOOL, "ByteBool"},
        {T_SHORT, "Short"},
        {T_WORD, "Word"},
        {T_16BIT, "Bit16"},
        {T_BOOL, "Bool"},
        {T_UNIT, "Unit"},
        {T_INTEGER, "Int"},
        {T_DWORD, "DWord"},
        {T_32BIT, "Bit32"},
        {T_LONG, "Long"},
        {T_FLOAT, "Float"},
        {T_DATETIME, "DateTime"},
        {T_STRING, "String"},
        {T_STAMP, "Stamp"},
        {T_STRICT, "Strict"},
        {T_USAGE, "Usage"},
        {T_16BYTE, "Byte16"},
        {T_OUI, "Mac adr"},
        {T_IP4ADR, "IP4 adr"},
        {T_UTCTIME, "UTC time"},
        {T_SBYTE, "SignByte"},
        {T_UINT64, "UInt64"},
        {T_INT64, "Int64"},
        {T_TIME61850, "Time61850"}
    };

    return mapType;
}

const std::array<std::pair<int, const char *>, 9> g_headers = { {
    {2, "Название"}, {3, "#N"}, {4, "#Индекс"}, {5, "#Тип"},
    {6, "Размерность"}, {7, "Длина"}, {8, "Доступ"}, {9, "Обозначение"},
    {10, "#Базовая прошивка"}
}
};
} // namespace

using namespace QXlsx;

ExcelZipExporter::ExcelZipExporter() {}

bool ExcelZipExporter::convertToExcel(const std::vector<Parameter::UPtr> &paramList, const QString &dirPath)
{
    Document xlsx;
    xlsx.currentWorksheet()->setGridLinesVisible(false);
    xlsx.setColumnWidth(1, 2);
    xlsx.setRowHeight(1, 10);
    xlsx.setColumnWidth(2, 70);
    xlsx.setColumnWidth(5, 11);
    xlsx.setColumnWidth(6, 15);
    xlsx.setColumnWidth(8, 25);
    xlsx.setColumnWidth(9, 40);
    xlsx.setColumnWidth(10, 50);

    Format headerFormat;
    headerFormat.setBorderStyle(Format::BorderThin);
    headerFormat.setBorderColor(Qt::black);
    headerFormat.setHorizontalAlignment(Format::AlignHCenter);
    headerFormat.setPatternBackgroundColor(QColor(217, 217, 217));
    headerFormat.setFontBold(true);
    headerFormat.setFontName("Arial");
    headerFormat.setFontSize(9);

    for (int col = 2; col <= 9; ++col) {
        xlsx.write(2, col, QString(), headerFormat);
    }

    xlsx.write(2, 10, "#HDW", headerFormat);

    for (const auto &[col, title] : g_headers) {
        xlsx.write(3, col, title, headerFormat);
    }

    xlsx.renameSheet("Sheet1", "Параметры модуля");

    Format paramNameFormat;
    paramNameFormat.setBorderStyle(Format::BorderDashed);
    paramNameFormat.setBorderColor(Qt::gray);
    paramNameFormat.setHorizontalAlignment(Format::AlignLeft);
    paramNameFormat.setPatternBackgroundColor(QColor(242, 242, 242));
    paramNameFormat.setFontBold(true);
    paramNameFormat.setFontName("Arial");
    paramNameFormat.setFontSize(9);

    Format format;
    format.setBorderStyle(Format::BorderDashed);
    format.setBorderColor(Qt::gray);
    format.setHorizontalAlignment(Format::AlignLeft);
    format.setTextWrap(true);
    format.setFontName("Arial");
    format.setFontSize(9);

    int row = 4;
    bool needGroupHeader = false;

    for (const auto &param : paramList) {
        bool isSimpleParam = (param->profilesCount() == 1 && param->elementsCount() == 1);

        if (!isSimpleParam) {
            xlsx.mergeCells(CellRange(row, 2, row, 10), paramNameFormat);

            xlsx.write(row, 2, param->name(), paramNameFormat);
            ++row;
            needGroupHeader = true;
        }
        else {
            if (needGroupHeader) {
                xlsx.mergeCells(CellRange(row, 2, row, 10), paramNameFormat);

                xlsx.write(row, 2, QString(), paramNameFormat);
                ++row;
                needGroupHeader = false;
            }
        }

        for (const auto &profile : param->profiles()) {
            for (const auto &element : profile) {
                if (element->name().isEmpty())
                    xlsx.write(row, 2, param->name(), format);
                else
                    xlsx.write(row, 2, element->name(), format);

                if (param->profilesCount() == 1)
                    xlsx.write(row, 6, param->elementsCount(), format);
                else
                    xlsx.write(row, 6, QString("%1:%2").arg(param->profilesCount()).arg(param->elementsCount()), format);

                if (param->type() == T_OUI)
                    xlsx.write(row, 10, element->value().replace(':', '-'), format);
                else if (element->value() == "nan")
                    xlsx.write(row, 10, QString(), format);
                else {
                    if (param->addr() == SP_DATETIME) {
                        xlsx.write(row, 10, QDateTime::fromSecsSinceEpoch(element->value().toInt()).toString("dd-MM-yyyy HH:mm:ss"), format);
                    }
                    else if (param->addr() == SP_ASTRONOMIC_TIME) {
                        const auto &time = Dpc::Sybus::fromStrictTime(element->value().toLongLong());
                        xlsx.write(row, 10, time.toString("dd-MM-yyyy HH:mm:ss.zzz"), format);
                    }
                    else if (param->addr() == SP_ASTRONOMIC_TIME_MCS || param->addr() == SP_MNFCFGDATE || param->addr() == SP_CFGDATE) {
                        if (element->value().toLongLong() != 0){
                            const auto &time = Dpc::Sybus::fromUtcTime(element->value().toLongLong());
                            xlsx.write(row, 10, time.toString("dd-MM-yyyy HH:mm:ss.zzz"), format);
                        }
                    }
                    else if (param->addr() == SP_PTPTIME) {
                        const auto &time = Dpc::Sybus::fromTime61850(element->value().toLongLong());
                        xlsx.write(row, 10, time.toString("dd-MM-yyyy HH:mm:ss.zzz"), format);
                    }
                    else
                        xlsx.write(row, 10, element->value(), format);
                }

                xlsx.write(row, 3, QString("0x%1").arg(QString::number(param->addr(), 16).toUpper()), format);
                xlsx.write(row, 4, element->position(), format);
                xlsx.write(row, 7, param->dataSize(), format);
                xlsx.write(row, 8, param->attributes().toString().toUpper(), format);
                xlsx.write(row, 9, QString(), format);

                auto it = getMapType().find(param->type());
                if (it != getMapType().end())
                    xlsx.write(row, 5, it->second, format);

                ++row;
            }
        }
    }

    QDir dir(dirPath);
    if (!dir.exists() && !dir.mkpath("."))
        return false;

    QString filePath = dir.filePath("Excel_report.xlsx");

    return xlsx.saveAs(filePath);
}
