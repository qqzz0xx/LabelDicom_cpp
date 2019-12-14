#include "NThreadPool.h"


void NThreadTask::run()
{
	if (Callback) Callback();

	emit Finished();
}

////////////////////////////

NThreadPool::NThreadPool(QObject *parent /*= Q_NULLPTR*/) : QThreadPool(parent)
{
	this->setMaxThreadCount(4);
}

void NThreadPool::Start(std::function<void()> callback, std::function<void()> finishCB)
{
	auto task = new NThreadTask;
	task->Callback = callback;
	connect(task, &NThreadTask::Finished, [finishCB] {
		if (finishCB)
		{
			finishCB();
		}
	});

	this->start(task);

}

//////////////////////////////////////////

void NThread::run()
{
	if (Callback) Callback();
	emit FinishedSignal();
}
