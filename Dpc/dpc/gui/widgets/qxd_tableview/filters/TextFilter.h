#ifndef TEXTFILTER_H
#define TEXTFILTER_H

#include <QRegularExpression>

#include <dpc/gui/widgets/qxd_tableview/QxdFilter.h>

class TextFilter : public QxdIFilter
{
public:
	TextFilter();
	~TextFilter();

	virtual int type() const override;
    virtual bool accept(const QModelIndex &index, int filterRole) const override;    
	virtual void reset() override;

	QString pattern() const { return m_pattern; }
	void setPattern(const QString &t) { m_pattern = t; }

    QRegularExpression regExp() const { return m_regExp; }
    void setRegExp(QRegularExpression regexp) { m_regExp = regexp; }

	bool exactMatch() const {	return m_exactMatch; }
    void setExactMatch(bool exactmatch) { m_exactMatch = exactmatch; }

protected:
    virtual void onSetEqualTo(const QList<QVariant> &list) override;

private:
	QString m_pattern;
    QRegularExpression m_regExp;
	bool m_exactMatch;
};

#endif // TEXTFILTER_H
