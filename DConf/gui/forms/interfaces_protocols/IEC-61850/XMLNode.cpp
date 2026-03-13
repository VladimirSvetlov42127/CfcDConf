#include "XMLNode.h"

#include <qdebug.h>

namespace {
} // namespace

XMLNode::XMLNode(const QDomElement & elem) : 
	m_element(elem)
{
}

XMLNode::~XMLNode()
{
	//qDebug() << Q_FUNC_INFO;
}

bool XMLNode::isValid() const
{ 
	return !m_element.isNull(); 
}

void XMLNode::setElement(const QDomElement & elem)
{
	m_element = elem; 
}

QString XMLNode::text() const 
{
	return getChildTextNode().nodeValue();
}

void XMLNode::setText(const QString & text) 
{
	QDomText t = getChildTextNode();
	if (t.isNull()) {
		t = m_element.ownerDocument().createTextNode(text);
		m_element.appendChild(t);
	}

	t.setNodeValue(text);
}

QString XMLNode::attribute(const QString & name, const QString & defValue) const
{
	return m_element.attribute(name, defValue);
}

void XMLNode::setAttribute(const QString & name, const QString & value)
{
	m_element.setAttribute(name, value);
}

QList<XMLNode> XMLNode::childs(const QString & tagName) const
{
	QList<XMLNode> result;

	auto childs = m_element.childNodes();
	for (size_t i = 0; i < childs.count(); i++) {
		QDomElement e = childs.at(i).toElement();
		if (e.isNull())
			continue;

		if (!tagName.isEmpty() && tagName != e.tagName())
			continue;

		result.append(e);
	}

	return result;
}

QList<XMLComment> XMLNode::comments() const
{
	QList<XMLComment> result;

	auto childs = m_element.childNodes();
	for (size_t i = 0; i < childs.count(); i++) {
		auto e = childs.at(i).toComment();
		if (e.isNull())
			continue;

		result.append(e);
	}

	return result;
}

XMLNode XMLNode::createChild(const QString & name)
{
	QDomElement child = m_element.ownerDocument().createElement(name);
	m_element.appendChild(child);
	return child;
}

void XMLNode::removeChild(const XMLNode & child)
{
	m_element.removeChild(child.m_element);
}

XMLNode XMLNode::child(const QString & name) const
{
	auto list = childs(name);
	if (!list.isEmpty())
		return list.first();

	return XMLNode();
}

XMLNode XMLNode::forceChild(const QString & name)
{
	XMLNode n = child(name);
	if (n.isValid())
		return n;

	return createChild(name);
}

QDomText XMLNode::getChildTextNode() const 
{
	auto childs = m_element.childNodes();
	for (size_t i = 0; i < childs.count(); i++) {
		QDomText t = childs.at(i).toText();
		if (!t.isNull())
			return t;
	}

	return QDomText();
}