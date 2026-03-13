#include "frmOpenProj.h"

#include <QDebug>
#include <QScreen>

#include <dpc/helper.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

namespace {

const int g_ProjectRole = Qt::UserRole + 1;

const char* g_StyleSheet = "QHeaderView::section { \
        background-color: #7AB1C9; \
        padding-left: 4px;\
        border: 1px solid #999999; \
        height: 25px}";

} // namespace

frmOpenProj::frmOpenProj(DcProjectsManager *projectsManager, QWidget *parent)
	: QDialog(parent)
    , m_model{new QStandardItemModel(this)}
    , m_projectsManager{projectsManager}
    , m_project{nullptr}
{
    ui.setupUi(this);

    QStringList horizontalHeader;
    horizontalHeader.append("Проект");
    horizontalHeader.append("Время изменения");
    horizontalHeader.append("Автор");
    horizontalHeader.append("Объект");
    m_model->setHorizontalHeaderLabels(horizontalHeader);

    auto& projectList = projectsManager->projects();
    for(auto it = projectList.crbegin(); it != projectList.crend(); ++it) {
        auto project = it->get();
        QList<QStandardItem*> row;
        QStandardItem *pname = new QStandardItem(project->name());
        pname->setData(Dpc::fromPtr(project), g_ProjectRole);
        pname->setEditable(false);
        QStandardItem *pobject = new QStandardItem(project->object());
        pobject->setEditable(false);
        QStandardItem *pauth = new QStandardItem(project->author());
        pauth->setEditable(false);
        QStandardItem *ptimeupdate = new QStandardItem(project->updatedTime().toString("dd.MM.yyyy hh:mm:ss"));
        ptimeupdate->setEditable(false);
        row.append(pname);
        row.append(ptimeupdate);
        row.append(pauth);
        row.append(pobject);
        m_model->appendRow(row);
    }

    ui.tblProjectSelect->setModel(m_model);
    ui.tblProjectSelect->verticalHeader()->hide();
    ui.tblProjectSelect->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tblProjectSelect->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.tblProjectSelect->horizontalHeader()->setStyleSheet(g_StyleSheet);
    ui.tblProjectSelect->horizontalHeader()->setStretchLastSection(true);
//	ui.tblProjectSelect->resizeRowsToContents();
//	ui.tblProjectSelect->setColumnWidth(2, 120);
    ui.tblProjectSelect->resizeColumnsToContents();
    ui.tblProjectSelect->setCurrentIndex(QModelIndex());
    ui.tblProjectSelect->clearSelection();

    auto onCurrentChanged = [=]() {
        bool hasSelection = ui.tblProjectSelect->currentIndex().isValid();
        ui.bntOpen->setEnabled(hasSelection);
        ui.btnRemove->setEnabled(hasSelection);
    };
    onCurrentChanged();
    connect(ui.tblProjectSelect->selectionModel(), &QItemSelectionModel::currentChanged, this, onCurrentChanged);

    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen)
        return;

    QRect screenGeometry = screen->availableGeometry();
    QSizeF dialogSize = { screenGeometry.width() * 0.4, screenGeometry.height() * 0.4 };
    int x = screenGeometry.x() + (screenGeometry.width() - dialogSize.width()) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - dialogSize.height()) / 2;
    setGeometry(QRect(QPoint(x, y), dialogSize.toSize()));
}

DcProject *frmOpenProj::project() const
{
    return m_project;
}

void frmOpenProj::projOpen()
{
    projOpen(QModelIndex());
}

void frmOpenProj::projRemove()
{
    auto firstColumnIndex = ui.tblProjectSelect->selectionModel()->selectedRows(0).value(0);
    auto item = m_model->itemFromIndex(firstColumnIndex);
    if (!item)
        return;

    auto project = Dpc::toPtr<DcProject>(item->data(g_ProjectRole));
    if (!project)
        return;

    if (!MsgBox::question(QString("Вы действительно хотите удалить выбранный проект?")))
        return;

    m_model->removeRow(item->row());
    m_projectsManager->remove(project);

    ui.tblProjectSelect->setCurrentIndex(QModelIndex());
    ui.tblProjectSelect->clearSelection();
}

void frmOpenProj::projOpen(const QModelIndex &)
{
    auto firstColumnIndex = ui.tblProjectSelect->selectionModel()->selectedRows(0).value(0);
    auto item = m_model->itemFromIndex(firstColumnIndex);
    if (!item)
        return;

    auto project = Dpc::toPtr<DcProject>(item->data(g_ProjectRole));
    if (!project)
        return;

    m_project = project;
    accept();
}
