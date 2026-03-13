#include "LinearPiecewiseSettingsWidget.h"

#include <set>

#include <qdebug.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qfiledialog.h>
#include <qsettings.h>

#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

namespace {
#pragma pack(push, 1)
    struct HCXPNT
    {
        HCXPNT(const QByteArray& buff = QByteArray()) {
            if (!buff.isEmpty())
                memcpy(this, buff.data(), sizeof(HCXPNT));
        }

        float temp = 0.f;
        float resistnc = 0.f;

        QByteArray data() const { return QByteArray((const char*)this, sizeof(HCXPNT)); }
    };
#pragma pack(pop)

    using HCXPNTList = QList<HCXPNT>;

    class TableModel : public QAbstractTableModel
    {
    public:
        TableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {}

        enum Columns {
            NumColumn = 0,
            TempColumn,
            ResistncColumn,

            ColumnsCount
        };

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override { 
            if (parent.isValid())
                return 0;

            return m_items.size();
        }

        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override {
            if (parent.isValid())
                return 0;

            return ColumnsCount;
        }

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override { 
            if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
                return columnName((Columns)section);

            return QVariant();
        }

        virtual Qt::ItemFlags flags(const QModelIndex& index) const override {
            auto fl = QAbstractTableModel::flags(index);
            if (!index.column())
                return fl;
            return fl | Qt::ItemIsEditable;
        }

        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
            if (!index.isValid())
                return QVariant();            

            if (Qt::TextAlignmentRole == role)
                return Qt::AlignCenter;

            auto item = m_items.at(index.row());
            int col = index.column();

            if (Qt::EditRole == role || Qt::DisplayRole == role) {
                switch (col)    
                {
                case NumColumn: return index.row() + 1;
                case ResistncColumn: return item.resistnc;
                case TempColumn: return item.temp;
                }
            }

            return QVariant();
        }

        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override {
            if (!index.isValid())
                return false;
            
            auto& item = m_items[index.row()];
            int col = index.column();            

            if (col && col < Columns::ColumnsCount) {                
                if (col == Columns::ResistncColumn)
                    item.resistnc = value.toFloat();
                if (col == Columns::TempColumn)
                    item.temp = value.toFloat();

                emit dataChanged(index, index);
                return true;
            }

            return false;
        }

        QString columnName(Columns col) const {
            switch (col)
            {   
            case NumColumn: return "№";
            case ResistncColumn: return "Входное значение";
            case TempColumn: return "Выходное значение";
            default: return QString();
            }
        }

        HCXPNTList items() const { return m_items; }

        void setItems(const HCXPNTList& items) {
            beginResetModel();
            m_items = items;
            endResetModel();
        }

        void appendItem(const HCXPNT& item) {
            auto count = m_items.count();
            beginInsertRows(QModelIndex(), count, count);
            m_items << item;
            endInsertRows();
        }

        void removeItems(const std::set<int> &rows) {
            beginResetModel();
            for (auto it = rows.rbegin(); it != rows.rend(); it++)
                m_items.removeAt(*it);
            endResetModel();
        }

    private:
        HCXPNTList m_items;
    };
}

LinearPiecewiseSettingsWidget::LinearPiecewiseSettingsWidget(const QByteArray &data, QWidget *parent) :
    ICustomConversionSettingsWidget(parent)
{
    auto addButton = new QPushButton("Добавить");
    connect(addButton, &QPushButton::clicked, this, &LinearPiecewiseSettingsWidget::onAddButton);

    auto removeButton = new QPushButton("Удалить");
    connect(removeButton, &QPushButton::clicked, this, &LinearPiecewiseSettingsWidget::onRemoveButton);

    auto fromFileButton = new QPushButton("Из файла");
    connect(fromFileButton, &QPushButton::clicked, this, &LinearPiecewiseSettingsWidget::onFromFileButton);

    auto dataCopy = data;
    HCXPNTList itemsList;
    while (dataCopy.size() >= sizeof(HCXPNT)) {
        itemsList << HCXPNT(dataCopy);
        dataCopy.remove(0, sizeof(HCXPNT));
    }

    auto model = new TableModel(this);
    model->setItems(itemsList);

    m_view = new TableView(model, this);
    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->setColumnWidth(TableModel::NumColumn, 10);
    m_view->setColumnWidth(TableModel::ResistncColumn, 150);
    m_view->setColumnWidth(TableModel::TempColumn, 150);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

    auto max = std::numeric_limits<float>::max();
    auto min = -max;
    auto decimals = 3;

    auto flDelegate = new DoubleSpinBoxDelegate(min, max, this);
    flDelegate->setDecimals(decimals);

    m_view->setItemDelegateForColumn(TableModel::ResistncColumn, flDelegate);
    m_view->setItemDelegateForColumn(TableModel::TempColumn, flDelegate);

    auto layout = new QGridLayout(this);
    layout->addWidget(addButton, 0, 0);
    layout->addWidget(removeButton, 0, 1);
    layout->addWidget(fromFileButton, 0, 2);
    layout->addWidget(m_view, 1, 0, 2, 3);
}

uint16_t LinearPiecewiseSettingsWidget::itemsCount() const
{
    return static_cast<TableModel*>(m_view->model())->items().count();
}

QByteArray LinearPiecewiseSettingsWidget::settingsData() const
{
    QByteArray result;
    for (auto&& it : static_cast<TableModel*>(m_view->model())->items())
        result += it.data();
    return result;
}

void LinearPiecewiseSettingsWidget::onAddButton()
{
    static_cast<TableModel*>(m_view->model())->appendItem(HCXPNT{});
}

void LinearPiecewiseSettingsWidget::onRemoveButton()
{
    std::set<int> selectedRows;
    for (auto&& index : m_view->selectionModel()->selectedRows())
        selectedRows.insert(index.row());

    if (selectedRows.size()) {
        if(!MsgBox::question("Вы уверены что хотите удалить выбранные строки?"))
            return;
    }

    static_cast<TableModel*>(m_view->model())->removeItems(selectedRows);
}

void LinearPiecewiseSettingsWidget::onFromFileButton()
{
    auto dirPathKey = "AnalogsConversion/customFilePath";
    QSettings st;
    
    auto dir = st.value(dirPathKey, QDir::homePath()).toString();
    auto fileName = QFileDialog::getOpenFileName(this, "Выберите файл", dir, "CSV c разделителем - ';' (*.csv)");
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        MsgBox::error(QString("Не удалось открыть файл(%1): %2").arg(fileName).arg(file.errorString()));
        return;
    }

    st.setValue(dirPathKey, QFileInfo(fileName).absolutePath());

    HCXPNTList itemsList;
    size_t lineNum = 0;
    for (auto&& line : file.readAll().split('\n')) {
        lineNum++;
        QString errorTemplate = QString("Ошибка обработки файла, строка %1: %2").arg(lineNum);

        line = line.trimmed();
        if (line.isEmpty())
            continue;

        auto splitedLineList = line.split(';');
        if (splitedLineList.count() < 2) {
            MsgBox::error(errorTemplate.arg("недостаточно значений"));
            return;
        }

        auto val0 = splitedLineList[0].trimmed();
        auto val1 = splitedLineList[1].trimmed();

        bool ok1;
        bool ok2;
        HCXPNT item;
        item.temp = QLocale().toFloat(val0, &ok1);
        if (!ok1) {
            MsgBox::error(errorTemplate.arg("не удалось преобразовать 1-е значение"));
            return;
        }

        item.resistnc = QLocale().toFloat(val1, &ok2);
        if (!ok2) {
            MsgBox::error(errorTemplate.arg("не удалось преобразовать 2-е значение"));
            return;
        }

        itemsList.append(item);
    }

    static_cast<TableModel*>(m_view->model())->setItems(itemsList);
}
