#pragma once

#include <qstyleditemdelegate.h>

#include <dpc/gui/widgets/SpinBox.h>

namespace Dpc::Gui
{
	template <class EditorType>
	class DPC_EXPORT SpinBoxDelegate : public QStyledItemDelegate
	{
	public:
		using DataType = typename EditorType::ValueType;

		SpinBoxDelegate(QObject* parent = 0);
		SpinBoxDelegate(DataType min, DataType max, QObject* parent = 0);

		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
		virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
		virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		virtual QString displayText(const QVariant& value, const QLocale& locale) const override;

		void setSingleStep(DataType step);
		void setSpecialValueText(const QString& text);
		void addValuesText(DataType value, const QString& text);

	protected:
		virtual QString defaultDisplayText(const QVariant& value, const QLocale& locale) const;

	private:
		DataType m_min;
		DataType m_max;
		DataType m_singleStep;
		QString m_specialValueText;
		typename EditorType::ValuesTextContainerType m_valuesText;
	};

	class DPC_EXPORT DoubleSpinBoxDelegate : public SpinBoxDelegate<DoubleSpinBox>
	{
	public:
		DoubleSpinBoxDelegate(QObject* parent = 0);
		DoubleSpinBoxDelegate(double min, double max, QObject* parent = 0);

		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		void setDecimals(int decimals);

	protected:
		virtual QString defaultDisplayText(const QVariant& value, const QLocale& locale) const override;

	private:
		int m_decimals;
	};

	// сделано так, потому что using обЪявления нельзя сделать forward declaration
	class DPC_EXPORT IntSpinBoxDelegate : public SpinBoxDelegate<IntSpinBox> { using SpinBoxDelegate::SpinBoxDelegate; };
	class DPC_EXPORT UIntSpinBoxDelegate : public SpinBoxDelegate<UIntSpinBox> { using SpinBoxDelegate::SpinBoxDelegate; };
} // namespace
