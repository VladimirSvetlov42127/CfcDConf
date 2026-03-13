#pragma once

#include <qcombobox.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui
{
	class DPC_EXPORT ComboBox : public QComboBox
	{
		Q_OBJECT
	public:
		ComboBox(QWidget* parent = nullptr);

        void setWheelEventEnabled(bool enabled);

    protected:
        void wheelEvent(QWheelEvent *event) override;

	private slots:
		void onCurrentIndexChanged(int index);

    private:
        bool m_wheelEventEnabled = false;
	};
} // namespace
