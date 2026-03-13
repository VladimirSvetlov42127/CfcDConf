#include "cfc_algs_model.h"

#include <QIcon>

namespace {

} // namespace

CfcAlgsModel::CfcAlgsModel(CfcAlgManager *manager, QObject *parent)
    : QAbstractTableModel(parent)
    , m_manager(manager)
{
}

int CfcAlgsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_manager->cfcAlgList().size();
}

int CfcAlgsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Column::__Count;
}

QVariant CfcAlgsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return columnName(static_cast<Column>(section));

    return QVariant();
}

Qt::ItemFlags CfcAlgsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags fl = QAbstractTableModel::flags(index);
    if (index.column() < Column::Enable)
        return fl;

    if (index.column() == Column::Enable) {
        auto service = m_manager->cfcAlgList().at(index.row());
        return service->isCompiled() ? fl |= Qt::ItemIsUserCheckable : fl;
    }

    return fl | Qt::ItemIsEditable;
}

QVariant CfcAlgsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto service = m_manager->cfcAlgList().at(index.row());
    if (Qt::DisplayRole == role) {
        switch (index.column())
        {
        case Column::Number: return service->id();
        case Column::Name: return service->name();
        case Column::Desc: return service->description();
        }
    }

    if (Qt::EditRole == role) {
        switch (index.column())
        {
        case Column::Name: return service->name();
        case Column::Desc: return service->description();
        }
    }

    if (Qt::TextAlignmentRole == role)
        return Qt::AlignCenter;

    if (Qt::DecorationRole == role) {
        switch (index.column())
        {
        case Column::Compile: return service->isCompiled() ? QIcon(":/icons/compil.svg") : QIcon(":/icons/compil_fail.svg");
        }
    }

    if (Qt::CheckStateRole == role) {
        switch (index.column())
        {
        case Column::Enable: return service->isEnabled() ? Qt::Checked : Qt::Unchecked;
        }
    }

    if (Qt::BackgroundRole == role && service->hasInvalidInput())
        return QColor(Qt::yellow);

    return QVariant();
}

bool CfcAlgsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto service = m_manager->cfcAlgList().at(index.row());
    int col = index.column();
    if (col == Column::Enable) {
        service->setEnabled(value.toBool());
        emit dataChanged(index, index);
        return true;
    }

    if (col == Column::Name) {
        service->setName(value.toString());
        emit dataChanged(index, index);
        return true;
    }

    if (col == Column::Desc) {
        service->setDescription(value.toString());
        emit dataChanged(index, index);
        return true;
    }

    return false;
}

CfcAlgService *CfcAlgsModel::create()
{
    if (!m_manager->canCreate())
        return nullptr;

    beginResetModel();
    auto res = m_manager->create();
    endResetModel();
    return res;
}

void CfcAlgsModel::remove(CfcAlgService *cfcAlg)
{
    beginResetModel();
    m_manager->remove(cfcAlg);
    endResetModel();
}

CfcAlgService *CfcAlgsModel::cfcAlg(const QModelIndex &index)
{
    if (!index.isValid())
        return nullptr;

    return m_manager->cfcAlgList().at(index.row());
}

const CfcAlgManager *CfcAlgsModel::cfcManager() const
{
    return m_manager;
}

QString CfcAlgsModel::columnName(Column col)
{
    switch (col)
    {
    case CfcAlgsModel::Column::Number: return "№";
    case CfcAlgsModel::Column::Compile: return "Скомпилирован";
    case CfcAlgsModel::Column::Enable: return "Включен";
    case CfcAlgsModel::Column::Name: return "Название";
    case CfcAlgsModel::Column::Desc: return "Описание";
    default:
        break;
    }

    return QString();
}
