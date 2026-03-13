#include "QxdHeaderViewMenu.h"

#include <QDebug>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QTableView>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>

#include <dpc/gui/widgets/qxd_tableview/QxdProxyModel.h>
#include <dpc/gui/widgets/qxd_tableview/QxdFilter.h>

namespace {
    const QString titleString = "Колонки";
    const QString allString = "Все";
    const QString clearFiltersString = "Очистить все фильтры";

    class HeaderViewModel : public QAbstractTableModel
    {
    public:
        HeaderViewModel(QHeaderView *view, QObject *parent = nullptr) : QAbstractTableModel(parent), m_view(view) {}

        Qt::ItemFlags flags(const QModelIndex &index) const override
        {
            if (!index.isValid())
                return Qt::NoItemFlags;

            return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
        }

        int rowCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : m_view->count(); }

        int columnCount(const QModelIndex &parent) const { return parent.isValid() ? 0 : 1; }

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
        {
            if (!index.isValid())
                return QVariant();

            if (role == Qt::TextAlignmentRole)
                return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

            if (role == Qt::CheckStateRole)
                return m_view->isSectionHidden(index.row()) ? Qt::Unchecked : Qt::Checked;

            if (role == Qt::DisplayRole)
                return m_view->model()->headerData(index.row(), Qt::Horizontal);

            if (role == Qt::DecorationRole)
                return m_view->model()->headerData(index.row(), Qt::Horizontal, Qt::DecorationRole);

            return QVariant();
        }

        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override
        {
            if (role == Qt::CheckStateRole) {
                m_view->setSectionHidden(index.row(), !value.toBool());
                emit dataChanged(index, index);
                return true;
            }

            return false;
        }

        void setAllCheckState(bool state)
        {
            layoutAboutToBeChanged();
            for(int i = 0; i < m_view->count(); i++)
                m_view->setSectionHidden(i, !state);
            layoutChanged();
        }

        Qt::CheckState allState() const
        {
            bool isAllChecked = true;
            bool isAllUnchecked = true;

            for(int i = 0; i < m_view->count(); i++) {
                if (isAllChecked)
                    isAllChecked = !m_view->isSectionHidden(i);
                if (isAllUnchecked)
                    isAllUnchecked = m_view->isSectionHidden(i);
                if (!isAllChecked && !isAllUnchecked)
                    return Qt::PartiallyChecked;
            }

            return isAllUnchecked ? Qt::Unchecked : Qt::Checked;
        }

    private:
        QHeaderView *m_view;
    };
}

QxdHeaderViewMenu::QxdHeaderViewMenu(QHeaderView *source_view, QWidget *parent) :
    QxdFrame(parent)
{
    QVBoxLayout *l = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel(titleString, this);
    titleLabel->setAlignment(Qt::AlignHCenter);
    QFont f = titleLabel->font();
    f.setBold(true);
    titleLabel->setFont(f);
    titleLabel->setMaximumHeight(32);
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(9, 9, 9, 0);
    titleLayout->addWidget(titleLabel);
    l->addWidget(titleLabel);

    HeaderViewModel *hvm = new HeaderViewModel(source_view, this);
    QCheckBox *allCheckBox = new QCheckBox(allString, this);
    allCheckBox->setCheckState(hvm->allState());
    connect(hvm, &HeaderViewModel::dataChanged, this, [=]() { allCheckBox->setCheckState(hvm->allState()); });
    connect(allCheckBox, &QCheckBox::clicked, this, [=](bool checked) { allCheckBox->setTristate(false); hvm->setAllCheckState(checked); });
    l->addWidget(allCheckBox);

    QTableView *tView = new QTableView(this);
    tView->setShowGrid(false);
    tView->horizontalHeader()->setStretchLastSection(true);
    tView->horizontalHeader()->hide();
    tView->verticalHeader()->setMinimumSectionSize(0);
    tView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);    
    tView->verticalHeader()->setDefaultSectionSize(QFontMetrics(hvm->index(0,0).data(Qt::FontRole).value<QFont>()).height() + 2 );
    tView->verticalHeader()->hide();
    tView->setModel(hvm);
    l->addWidget(tView);

    QxdProxyModel *proxyModel = dynamic_cast<QxdProxyModel*>(source_view->model());
    if (proxyModel) {
        QPushButton *clearAllButton = new QPushButton(QIcon(":/clear-filter.png"), clearFiltersString, this);
        connect(clearAllButton, &QPushButton::clicked, this, [proxyModel](){
            proxyModel->resetFilter();
            proxyModel->invalidateFilter();
        });
        l->addWidget(clearAllButton);

        clearAllButton->setEnabled(false);
        for(size_t i = 0; i < proxyModel->columnCount(); i++) {
            auto filter = proxyModel->getFilter(i);
            if (filter && filter->isEnabled()) {
                clearAllButton->setEnabled(true);
                break;
            }
        }
    }

    setGeometry(0, 0, 180, sizeHint().height());
}

QxdHeaderViewMenu::~QxdHeaderViewMenu()
{
}
