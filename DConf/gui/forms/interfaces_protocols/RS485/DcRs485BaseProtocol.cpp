#include "DcRs485BaseProtocol.h"

namespace Text {
	const QString Title = "Параметры протокола";

	const QString ExchangeSpeed = "Скорость обмена";
	const QString InputBuffer = "Размер буфера приема";
	const QString OutputBuffer = "Размер буфера передачи";
	const QString AddressStation = "Адрес станции";
}

DcRs485BaseProtocol::DcRs485BaseProtocol(DcController *device, int portIdx, const QList<int> &exchangeSpeedList) :
	m_device(device),
	m_portIdx(portIdx),
	m_indexBase(portIdx * PROFILE_SIZE)
{
	for (auto &it : exchangeSpeedList)
		m_speedList.append(QString::number(it), it);		
}

DcRs485BaseProtocol::~DcRs485BaseProtocol()
{
}

void DcRs485BaseProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_DWORD, indexBase() + Rs485::Indexs::DWord::Speed,
		speedList().toHash(), Text::ExchangeSpeed));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::InputBuffer, Text::InputBuffer));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::OutputBuffer, Text::OutputBuffer));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Address, Text::AddressStation));
}

//=============================================================================
DcRs485BaseProtocolWidget::DcRs485BaseProtocolWidget(DcRs485BaseProtocol *protocol, QWidget *parent) :
	QTabWidget(parent),
	m_protocol(protocol),
	m_mainWidget(new QWidget),
	m_addressLabel(new QLabel(Text::AddressStation))
{
	addTab(m_mainWidget, Text::Title);

    m_inputBufferEditor = new LineParamEditor(device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::InputBuffer));
    m_outputBufferEditor = new LineParamEditor(device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::OutputBuffer));
    m_addressEditor = new LineParamEditor(device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Address));

	QGridLayout *layout = new QGridLayout(m_mainWidget);
	EditorsManager mg(device(), layout);
    mg.addListEditor(device()->paramsRegistry().element(SP_USARTPRTPAR_DWORD, indexBase() + Rs485::Indexs::DWord::Speed), Text::ExchangeSpeed, protocol->speedList());
	mg.addWidget(m_inputBufferEditor, Text::InputBuffer);
	mg.addWidget(m_outputBufferEditor, Text::OutputBuffer);
	mg.addWidget(m_addressEditor, m_addressLabel);

	layout->setColumnStretch(mg.descriptionColumn() + 1, 1);
}

DcRs485BaseProtocolWidget::~DcRs485BaseProtocolWidget()
{
	delete m_protocol;
}

