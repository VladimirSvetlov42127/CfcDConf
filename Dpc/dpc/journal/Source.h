#pragma once

#include <QObject>

#include <dpc/journal/Journal.h>

namespace Dpc
{
    class DPC_EXPORT Journal::ISource : public QObject
	{
		Q_OBJECT
	public:
		using IndexType = uint32_t;

		virtual IndexType id() const = 0;
		virtual QString name() const = 0;

	signals:
		void nameChanged(const QString &name);
	};
}
