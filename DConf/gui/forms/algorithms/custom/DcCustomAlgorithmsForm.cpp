#include "DcCustomAlgorithmsForm.h"

#include <qapplication.h>
#include <qpushbutton.h>
#include <qdir.h>

#include <db/dc_db_manager.h>

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <gui/forms/algorithms/custom/flex_editor_form.h>
#include <gui/forms/algorithms/custom/cfc_tools/cfc_compiler.h>
#include "gui/forms/algorithms/custom/cfc_algs_model.h"

using namespace Dpc::Gui;

namespace {
}

DcCustomAlgorithmsForm::DcCustomAlgorithmsForm(DcController *controller)
    : DcForm(controller, "Пользовательские алгоритмы", false)
    , m_model{new CfcAlgsModel(&controller->serviceManager()->cfcManager(), this)}
    , m_algsTableView{ new TableView(m_model, this)}
    , m_addButton(new QPushButton(QIcon(":/icons/table_+.svg"), "Добавить"))
    , m_removeButton(new QPushButton(QIcon(":/icons/table_-.svg"), "Удалить"))
    , m_compileButton(new QPushButton(QIcon(":/icons/compil.svg"), "Компилировать"))
    , m_fm(controller)
{
	m_algsTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_algsTableView->setEditTriggers(QAbstractItemView::SelectedClicked);
	m_algsTableView->horizontalHeader()->setStretchLastSection(true);
	m_algsTableView->horizontalHeader()->setHighlightSections(false);
    m_algsTableView->horizontalHeader()->resizeSection(CfcAlgsModel::Column::Number, 20);
    m_algsTableView->horizontalHeader()->resizeSection(CfcAlgsModel::Column::Compile, 120);
    m_algsTableView->horizontalHeader()->resizeSection(CfcAlgsModel::Column::Enable, 100);
    m_algsTableView->horizontalHeader()->resizeSection(CfcAlgsModel::Column::Name, 150);

	connect(m_algsTableView, &QTableView::doubleClicked, this, &DcCustomAlgorithmsForm::onViewDoubleClicked);
	connect(m_addButton, &QPushButton::clicked, this, &DcCustomAlgorithmsForm::onAddButton);
	connect(m_removeButton, &QPushButton::clicked, this, &DcCustomAlgorithmsForm::onRemoveButton);
	connect(m_compileButton, &QPushButton::clicked, this, &DcCustomAlgorithmsForm::onCompileButton);

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(m_addButton);
	buttonsLayout->addWidget(m_removeButton);
	buttonsLayout->addWidget(m_compileButton);
	buttonsLayout->addStretch();

	QGridLayout *layout = new QGridLayout(centralWidget());
	layout->addLayout(buttonsLayout, 0, 0);
	layout->addWidget(m_algsTableView, 1, 0);

    update();
}

DcCustomAlgorithmsForm::~DcCustomAlgorithmsForm()
{
}

bool DcCustomAlgorithmsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_FILE_FLEXLGFILES}
	};

	return hasAny(controller, params);
}

void DcCustomAlgorithmsForm::fillReport(DcIConfigReport * report)
{
//	auto device = report->device();

//	QStringList headers;
//	for (size_t i = 0; i < CfcAlgsModel::Column::__Count; i++)
//		headers << CfcAlgsModel::columnName(static_cast<CfcAlgsModel::Column>(i));

//	QList<DcAlgCfc *> existingAlgs;
//	DcReportTable table(device, headers, { 10, 10, 10, 30 });
//    auto mainflexLgcFilesParam = device->paramsRegistry().parameter(SP_FILE_FLEXLGFILES);
//    for (size_t i = 0; i < mainflexLgcFilesParam->elementsCount(); i++) {
//		DcAlgCfc *alg = device->algs_cfc()->getById(i + 1);
//		if (!alg)
//			continue;

//		existingAlgs << alg;
//		auto num = QString::number(alg->index());
//		bool isCompiled = alg->property(PROPERTY_COMPILED).toInt() && QFileInfo(DcFlexLogicFileManager(device).localBcaFileName(alg)).exists();
//        bool isEnabled = !device->paramsRegistry().element(SP_FILE_FLEXLGFILES, i)->value().isEmpty();
//		auto name = alg->name();
//		auto desc = alg->property(PROPERTY_DESCRIPTION);
//		table.addRow({ num, DcReportTable::checkedSign(isCompiled), DcReportTable::checkedSign(isEnabled), name, desc });
//	}

//	if (!table.values().size())
//		return;

//	report->insertSection();
//	report->insertTable(table);

//	for (auto alg : existingAlgs) {

//		//auto algImage = FbdEditWnd::algImage(device, alg);
//		//if (algImage.isNull()) {
//		//	MsgBox::error(QString("Не удалось создать изображение алгоритма %1").arg(alg->name()));
//		//	continue; }
//		//report->insertImage(algImage, alg->name());
//	}
}

void DcCustomAlgorithmsForm::update()
{
    bool hasAlgs = !m_model->cfcManager()->cfcAlgList().isEmpty();
    bool canAdd = m_model->cfcManager()->canCreate();
	m_removeButton->setEnabled(hasAlgs);
	m_compileButton->setEnabled(hasAlgs);
    m_addButton->setEnabled(canAdd);
    m_algsTableView->viewport()->update();
}

void DcCustomAlgorithmsForm::onAddButton()
{
    auto cfcService = m_model->create();
    if (!cfcService)
        return;

    update();
    editAlgorithm(cfcService);
}

void DcCustomAlgorithmsForm::onRemoveButton()
{
    QList<CfcAlgService*> selectedAlgs;
    auto selectedIndexList = m_algsTableView->selectionModel()->selectedRows(CfcAlgsModel::Column::Number);
    for(auto &index: selectedIndexList)
        selectedAlgs << m_model->cfcAlg(index);

    if (selectedAlgs.empty())
        return;

    if (!MsgBox::question(tr("Вы уверены, что хотите удалить выбранные алгоритмы?")))
        return;

    for (auto cfcAlg : selectedAlgs)
        m_model->remove(cfcAlg);

    update();
}

void DcCustomAlgorithmsForm::onCompileButton()
{
    for (auto &index : m_algsTableView->selectionModel()->selectedRows(CfcAlgsModel::Column::Number)) {
        compileAlgorithm(m_model->cfcAlg(index));
    }

    update();
}

void DcCustomAlgorithmsForm::onViewDoubleClicked(const QModelIndex &index)
{
    auto idx = m_algsTableView->selectionModel()->selectedRows(CfcAlgsModel::Column::Number).value(0);
    auto cfcAlg = m_model->cfcAlg(idx);
    if (!cfcAlg)
        return;

    editAlgorithm(cfcAlg);
}

void DcCustomAlgorithmsForm::onEditorAboutToClose()
{
    auto editor = dynamic_cast<FlexEditorForm*>(sender());
    if (!editor)
        return;

    auto cfcAlg = editor->cfcAlg();
    if (editor->hasError()) {
        cfcAlg->setCompiledData(QByteArray());
    }
    else if (compileAlgorithm(cfcAlg)) {
        cfcAlg->setEnabled(true);
    }

    editor->deleteLater();
    update();
}

void DcCustomAlgorithmsForm::editAlgorithm(CfcAlgService *cfcAlg)
{
    auto editor = new FlexEditorForm(controller(), cfcAlg, this);
    connect(editor, &FlexEditorForm::aboutToClose, this, &DcCustomAlgorithmsForm::onEditorAboutToClose, Qt::QueuedConnection);
	editor->setWindowModality(Qt::ApplicationModal);
	editor->show();	
}

bool DcCustomAlgorithmsForm::compileAlgorithm(CfcAlgService *cfcAlg)
{
    CfcCompiler compiler;
    connect(&compiler, &CfcCompiler::errorToLog, this, [=](const QString& mess) { journal()->addErrorMessage(mess); });
    connect(&compiler, &CfcCompiler::infoToLog, this, [=](const QString& mess) { journal()->addInfoMessage(mess); });
    if (compiler.compile(cfcAlg->parser())) {
        cfcAlg->setCompiledData(*compiler.byteCode());
        return true;
    }

    cfcAlg->setCompiledData(QByteArray());
    MsgBox::error(QString("При компиляции алгоритма номер %1, возникли критические ошибки").arg(cfcAlg->id()));
    return false;
}
