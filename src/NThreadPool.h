#pragma once
#include "Header.h"

#include <QThreadPool>
#include <QObject>
#include <QRunnable>

class NThread : public QThread
{
	Q_OBJECT
public:
	std::function<void()> Callback = NULL;
protected:
	virtual void run() override;
Q_SIGNALS:
	void FinishedSignal();
};

class NThreadTask : public QObject, public QRunnable
{
	Q_OBJECT
public:
	std::function<void()> Callback = NULL;
	virtual void run() override;

Q_SIGNALS:
	void Finished();

};

class NThreadPool : public QThreadPool
{
	Q_OBJECT
public:
	NThreadPool(QObject *parent = Q_NULLPTR);

	void Start(std::function<void()> task, std::function<void()> finishCB=nullptr);

};

