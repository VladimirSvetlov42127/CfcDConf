#include "ListEditorContainer.h"

ListEditorContainer::ListEditorContainer()
{
}

ListEditorContainer::ListEditorContainer(const QList<ListEditorContainer::Item>& items) :
	m_items(items)
{
}

ListEditorContainer::ListEditorContainer(std::initializer_list<ListEditorContainer::Item> list)
{
	for (auto &it : list)
		m_items.append({ it.text, it.value });
}

ListEditorContainer::ListEditorContainer(const QStringList & list, bool useTextAsValue)
{
	for (size_t i = 0; i < list.count(); i++) {
		QString item = list.at(i);
		QString value = useTextAsValue ? item : QString::number(i);
		m_items.append({ item, value });
	}
}

ListEditorContainer::ListEditorContainer(std::initializer_list<QString> list, bool useTextAsValue)
{
	size_t index = 0;
	for (auto &it : list) {
		QString value = useTextAsValue ? it : QString::number(index);
		m_items.append({ it, value });
		++index;
	}
}

void ListEditorContainer::append(const ListEditorContainer::Item & item)
{
	m_items.append(item);
}

void ListEditorContainer::append(const QString & text, const QVariant & value)
{
	m_items.append({text, value});
}

void ListEditorContainer::unite(const ListEditorContainer& list)
{
	for (size_t i = 0; i < list.size(); i++)
		m_items.append(list.at(i));
}

size_t ListEditorContainer::size() const
{
	return m_items.size();
}

ListEditorContainer::Item ListEditorContainer::at(size_t index) const
{
	return m_items.at(index);
}

QString ListEditorContainer::text(size_t index) const
{
	return m_items.at(index).text;
}

QString ListEditorContainer::text(const QVariant & value) const
{
	for (auto &it : m_items)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		if (it.value == value)
#else
		if (QVariant::compare(it.value, value) == 0)
#endif
			return it.text;

	return QString();
}

QVariant ListEditorContainer::value(size_t index) const
{
	return m_items.at(index).value;
}

QHash<QString, QString> ListEditorContainer::toHash() const
{
	QHash<QString, QString> result;
	for (auto &it : m_items)
		result[it.value.toString()] = it.text;

	return result;
}

void ListEditorContainer::exclude(const QVariantList & values)
{
	auto it = m_items.begin();
	while (it != m_items.end()) {
		if (values.contains(it->value))
			it = m_items.erase(it);
		else
			it++;
	}
}

ListEditorContainer::Iterator ListEditorContainer::begin()
{
	return m_items.begin();
}

ListEditorContainer::Iterator ListEditorContainer::end()
{
	return m_items.end();
}
