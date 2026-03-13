#pragma once

#include <qvector.h>
#include <qvariant.h>
#include <qsharedpointer.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandDescription.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandInitialization.h>

class CmdLibTreeItem 
{
public:
	typedef QSharedPointer<ModbusCommandDescription> sp_Command;
	typedef QSharedPointer<ModbusCommandInitialization> sp_InitCommand;

	CmdLibTreeItem();	// Для помещения в QVariant
	explicit CmdLibTreeItem(const QString &name, sp_Command command = nullptr, CmdLibTreeItem *parentItem = nullptr);
	explicit CmdLibTreeItem(bool isInit, const QString &name, sp_InitCommand command = nullptr, CmdLibTreeItem *parentItem = nullptr);
	~CmdLibTreeItem();

	void appendChild(CmdLibTreeItem *child);

	CmdLibTreeItem *child(int row);
	int childCount() const;
	int columnCount() const;
	const QString &data(int column) const;
	int row() const;
	CmdLibTreeItem *parentItem();

	spCommandDescr_t getCommand() {  return m_Command; };
	sp_InitCommand getInitCommand() { return m_InitCommand;  };
	bool isInitCommand() { return m_isInitCommand;  };

private:
	QVector<CmdLibTreeItem*> m_childItems;
	QString m_name;
	sp_Command m_Command;	// Храним команду
	sp_InitCommand m_InitCommand;
	CmdLibTreeItem *m_parentItem;
	bool m_isInitCommand = false;
};
Q_DECLARE_METATYPE(CmdLibTreeItem)
