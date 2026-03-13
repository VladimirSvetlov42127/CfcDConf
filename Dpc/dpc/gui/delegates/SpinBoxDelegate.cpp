#include "SpinBoxDelegate.h"

namespace Dpc::Gui
{
	template class SpinBoxDelegate<IntSpinBox>;
	template class SpinBoxDelegate<UIntSpinBox>;
	template class SpinBoxDelegate<DoubleSpinBox>;

	template<class EditorType>
	SpinBoxDelegate<EditorType>::SpinBoxDelegate(QObject* parent) :
		QStyledItemDelegate(parent),
		m_min(0),
		m_max(std::numeric_limits<DataType>::max()),
		m_singleStep(1)
	{
	}

	template<class EditorType>
	SpinBoxDelegate<EditorType>::SpinBoxDelegate(DataType min, DataType max, QObject* parent) :
		QStyledItemDelegate(parent),
		m_min(min),
		m_max(max),
		m_singleStep(1)
	{
	}

	template<class EditorType>
	QWidget* SpinBoxDelegate<EditorType>::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		EditorType* editor = new EditorType(parent);
		editor->setAlignment(Qt::AlignHCenter);
		editor->setFrame(false);
		editor->setMinimum(m_min);
		editor->setMaximum(m_max);
		editor->setSingleStep(m_singleStep);
		editor->setSpecialValueText(m_specialValueText);
		editor->setValuesText(m_valuesText);
		return editor;
	}

	template<class EditorType>
	void SpinBoxDelegate<EditorType>::setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		DataType value = index.model()->data(index, Qt::EditRole).value<DataType>();
		EditorType* spinBox = static_cast<EditorType*>(editor);
		spinBox->setValue(value);
	}

	template<class EditorType>
	void SpinBoxDelegate<EditorType>::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		EditorType* spinBox = static_cast<EditorType*>(editor);
		model->setData(index, spinBox->value(), Qt::EditRole);
	}

	template<class EditorType>
	void SpinBoxDelegate<EditorType>::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		editor->setGeometry(option.rect);
	}

	template<class EditorType>
	QString SpinBoxDelegate<EditorType>::displayText(const QVariant& value, const QLocale& locale) const
	{
		if (!m_specialValueText.isEmpty() && m_min == value.value<DataType>())
			return m_specialValueText;

		auto it = m_valuesText.find(value.value<DataType>());
		if (it != m_valuesText.end())
			return it.value();

		return defaultDisplayText(value, locale);
	}

	template<class EditorType>
	void SpinBoxDelegate<EditorType>::setSingleStep(DataType step)
	{
		m_singleStep = step;
	}

	template<class EditorType>
	void SpinBoxDelegate<EditorType>::setSpecialValueText(const QString& text)
	{
		m_specialValueText = text;
	}

	template<class EditorType>
	void SpinBoxDelegate<EditorType>::addValuesText(DataType value, const QString& text)
	{
		m_valuesText[value] = text;
	}

	template<class EditorType>
	QString SpinBoxDelegate<EditorType>::defaultDisplayText(const QVariant& value, const QLocale& locale) const
	{
		return QStyledItemDelegate::displayText(value, locale);
	}

	//================================================================================================
	DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(QObject* parent) :
		SpinBoxDelegate<DoubleSpinBox>(parent),
		m_decimals(2)
	{
	}

	DoubleSpinBoxDelegate::DoubleSpinBoxDelegate(double min, double max, QObject* parent) :
		SpinBoxDelegate<DoubleSpinBox>(min, max, parent),
		m_decimals(2)
	{
	}

	QWidget* DoubleSpinBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		auto editor = static_cast<DoubleSpinBox*>(SpinBoxDelegate::createEditor(parent, option, index));
		editor->setDecimals(m_decimals);
		return editor;
	}

	void DoubleSpinBoxDelegate::setDecimals(int decimals)
	{
		m_decimals = decimals;
	}

	QString DoubleSpinBoxDelegate::defaultDisplayText(const QVariant& value, const QLocale& locale) const
	{
		return locale.toString(value.value<double>(), 'f', m_decimals);
	}
} // namespace
