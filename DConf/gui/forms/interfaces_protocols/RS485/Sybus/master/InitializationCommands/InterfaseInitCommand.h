#pragma once
#include <qbytearray.h>
#include <QWidget>
#include <qbytearray.h>

class InterfaseInitCommand : public QWidget {
		
public:

	InterfaseInitCommand(QWidget * parent = Q_NULLPTR) : QWidget(parent) {

	}
	virtual ~InterfaseInitCommand() {};

	virtual QByteArray getCommandByteArray() const = 0;

};