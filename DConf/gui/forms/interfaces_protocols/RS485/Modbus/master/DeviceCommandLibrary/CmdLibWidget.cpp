#include "CmdLibWidget.h"
#include <qitemselectionmodel.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/DeviceCommandLibrary/CommandLibraryModel.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

CmdLibWidget::CmdLibWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	CommandLibraryModel *model = new CommandLibraryModel();
	QItemSelectionModel *selectModel = new QItemSelectionModel;
	selectModel->setModel(model);

	ui.treeView->setSelectionModel(selectModel);
	ui.treeView->setModel(model);
	ui.treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui.treeView->expandAll();

	// Попробовать сделать раскрытым только первый уровень
	//QModelIndex root = ui.treeView->rootIndex();
	//int row = 1;
	//QModelIndex node = root.child(row, 1);
	//while (node.isValid()) {
	//	ui.treeView->expand(node);
	//	row++;
	//	node = root.child(row, 0);
	//}
	
}

CmdLibWidget::~CmdLibWidget()
{
}

void CmdLibWidget::on_butOk_clicked() {

	ml_periodicCommands.clear();
	ml_initCommands.clear();

	QModelIndexList list (ui.treeView->selectionModel()->selectedIndexes());	

	for (QModelIndex index : list) {
		
		CmdLibTreeItem *item = static_cast<CmdLibTreeItem*> (index.internalPointer());
		if (!item) {
			MsgBox::error("Ошибка получения элемента модели");
			return;
		}
			
		addAllChildrenCommand(item);	// Идем рекурсивно по всем детям и добавляем в m_commandsList, если такого еще нет		
	}

	if (!ml_periodicCommands.isEmpty())
		emit signal_addCommandsFromLibrary(ml_periodicCommands);
	if (!ml_initCommands.isEmpty())
		emit signal_addInitCommandsFromLibrary(ml_initCommands);

	this->accept();
}

void CmdLibWidget::addAllChildrenCommand(CmdLibTreeItem * parent)
{
	if (!parent)
		return;

	if (parent->childCount() != 0) {	// Является ли это последним элементом в дереве?
		for (int i = 0; i < parent->childCount(); i++) {	// Если нет, проходим по всем детям и ищем
			addAllChildrenCommand(parent->child(i));
		}
	}		
	else {	// Если это последний элемент, хначит он должен содержать команду

		if (!parent->isInitCommand()) {	// Если это периодическая команда
			spCommandDescr_t command(parent->getCommand());
			if (command) {
				if (!ml_periodicCommands.contains(command))
					ml_periodicCommands.append(command);
			}
		}
		else {	// Если это команда инициализации

			sp_InitCommand init(parent->getInitCommand());
			if (init) {
				if (!ml_initCommands.contains(init))
					ml_initCommands.append(init);
			}

		}
	}
}

void CmdLibWidget::on_butCancel_clicked()
{
	this->reject();
}
