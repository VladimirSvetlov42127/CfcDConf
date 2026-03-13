#pragma once

#include <QDomElement>
#include <qlist.h>

class XMLComment 
{
public:
	XMLComment(const QDomComment &comm = QDomComment()) : m_comment(comm) {}

	QString text() const { return m_comment.data(); }

private:
	QDomComment m_comment;
};

class XMLNode
{
public:
	XMLNode(const QDomElement &elem = QDomElement());
	virtual ~XMLNode();
	bool isValid() const;
	void setElement(const QDomElement &elem);

	QString text() const;
	void setText(const QString &text);

	QString attribute(const QString &name, const QString &defValue = QString()) const;
	void setAttribute(const QString &name, const QString &value);

	QList<XMLNode> childs(const QString &tagName = QString()) const;
	QList<XMLComment> comments() const;

	XMLNode createChild(const QString &name);
	void removeChild(const XMLNode &child);

	XMLNode child(const QString &name) const;
	XMLNode forceChild(const QString &name);

private:
	QDomText getChildTextNode() const;

private:
	QDomElement m_element;
};