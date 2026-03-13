#include "MultipleRequest.h"

#include <qdebug.h>

namespace {
} // namespace

namespace Dpc::Sybus
{
	MultipleRequest::MultipleRequest(QObject* parent) :
		IRequest(parent),
		m_errorPolicy(TerminateOnError),
		m_executionType(SequentialExecution),
		m_finishedCount(0)
	{
	}

	MultipleRequest::MultipleRequest(const QList<IRequest*> list, QObject* parent) :
		IRequest(parent),
		m_errorPolicy(TerminateOnError),
		m_executionType(SequentialExecution),
		m_finishedCount(0)
	{
		for (auto r : list)
			addRequest(r);
	}

	MultipleRequest::~MultipleRequest()
	{
	}

	void MultipleRequest::addRequest(IRequest* req)
	{
		req->setParent(this);
		m_list.append(req);

		auto prefix = m_name.isEmpty() ? "Seq(" : ", ";
		m_name.append(prefix);
		m_name.append(req->name());
	}

	QString MultipleRequest::name() const
	{
		if (m_name.isEmpty())
			return QString();

		return QString("%1)").arg(m_name);
	}

	void MultipleRequest::run()
	{
		m_finishedCount = 0;

		switch (executionType())
		{
		case Dpc::Sybus::MultipleRequest::SequentialExecution:
			execNext();
			break;
		case Dpc::Sybus::MultipleRequest::ParallelExecution:
			execAll();
			break;
		default:
			break;
		}
	}

	void MultipleRequest::execNext()
	{
		auto req = m_list.value(finishedCount(), nullptr);
		if (!req) {
			finish();
			return;
		}

		connect(req, &IRequest::finished, this, [=] {
			if (req->error()) {
				if (!error())
					setError(req->error(), req->errorString(), QString("%1: %2").arg(name()).arg(req->name()));

				if (TerminateOnError == errorPolicy()) {
					finish();
					return;
				}
			}
			
			m_finishedCount++;
			addProgressValue(static_cast<float>(finishedCount()) / m_list.count() * 100);
			execNext();
		});

		connect(req, &IRequest::progress, this, [=](int value) {
			auto stepSize = 100.0 / m_list.count();
			auto currentStepProgress = value / 100.0 * stepSize;
			addProgressValue(finishedCount() * stepSize + currentStepProgress);
		});

		manager()->exec(req);
	}

	void MultipleRequest::execAll()
	{
		if (m_list.isEmpty()) {
			finish();
			return;
		}

		for (auto req : m_list) {
			connect(req, &IRequest::finished, this, [=] {
				if (req->error() && !error()) {
					setError(req->error(), req->errorString(), QString("%1: %2").arg(name()).arg(req->name()));
				}

				m_finishedCount++;
				addProgressValue(static_cast<float>(finishedCount()) / m_list.count() * 100);

				if (finishedCount() < m_list.count())
					return;

				finish();
			});

			manager()->exec(req);
		}		
	}

} // namespace