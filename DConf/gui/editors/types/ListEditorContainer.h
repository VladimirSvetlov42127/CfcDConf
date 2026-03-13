#pragma once

#include <qvariant.h>

class ListEditorContainer
{
public:
	struct Item
	{
		QString text;
		QVariant value;
	};

	using Iterator = QList<ListEditorContainer::Item>::iterator;

	ListEditorContainer();
	ListEditorContainer(const QList<ListEditorContainer::Item> &items);
	ListEditorContainer(std::initializer_list<ListEditorContainer::Item> list);
	ListEditorContainer(const QStringList &list, bool useTextAsValue = false);
	ListEditorContainer(std::initializer_list<QString> list, bool useTextAsValue = false);	

	void append(const ListEditorContainer::Item &item);
	void append(const QString &text, const QVariant &value);

	void unite(const ListEditorContainer& list);

	size_t size() const;
	ListEditorContainer::Item at(size_t index) const;

	QString text(size_t index) const;
	QString text(const QVariant &value) const;

	QVariant value(size_t index) const;

	// QHash<Value, Text>
	QHash<QString, QString> toHash() const;

	void exclude(const QVariantList &values);
	ListEditorContainer::Iterator begin();
	ListEditorContainer::Iterator end();

private:
	QList<ListEditorContainer::Item> m_items;
};

