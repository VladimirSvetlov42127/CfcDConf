#pragma once

#include <qabstractitemmodel.h>
#include <qvector.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/DeviceCommandLibrary/CmdLibTreeItem.h>

class CommandLibraryModel : public QAbstractItemModel
{
	Q_OBJECT
		
public:
	typedef QSharedPointer<ModbusCommandDescription> sp_Command;
	typedef QSharedPointer<ModbusCommandInitialization> sp_InitCommand;

	explicit CommandLibraryModel(QObject *parent = nullptr);
	~CommandLibraryModel();

	QVariant data(const QModelIndex &index, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
	CmdLibTreeItem *m_rootItem;

private:
	void setupModelData(CmdLibTreeItem *parent);	// Заполняем библиотеку
	void setMercury230Catalog(CmdLibTreeItem *parent);

	// Формирование команд Меркурия
	bool makeInitializationCommands(CmdLibTreeItem *parent);	// Добавляет команды инициализации в корень
	bool makeMercury230PowerCommands(CmdLibTreeItem *parent);	// Добавляет команды мощности
	
	bool makeMercuryCommands(CmdLibTreeItem *parent, const QString& paramName, const  QVector<QString> &v_commandNames, const QVector<QString> &v_itemsNames, const QVector<uint8_t> v_commandCodes, float k0, float k1);
	bool makeInitCommand(CmdLibTreeItem * parent, const QString& paramName, uint8_t expectedByteCount, const QByteArray& firstRspBytes, const QByteArray& commandBuf);

	bool makeSingleCommandForMercury(CmdLibTreeItem *parent, const QString& commandName, const QString& treeItemName, uint8_t funcCode, float k0, float k1);
};
