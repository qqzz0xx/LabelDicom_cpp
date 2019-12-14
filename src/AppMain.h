#pragma once

#include "MainWindow.h"
#include "Interface.h"
#include "LoadMgr.h"
#include "ModelManager.h"
#include <QApplication>
#include <QThread>

class BaseMprTools;
class NThreadPool;

class AppMain : public QApplication, public IEnterExit, public IRunable
{
	Q_OBJECT
public:
	AppMain(int &argc, char **argv);
	~AppMain();

	// 通过 IEnterExit 继承
	virtual void Enter() override;
	virtual void Exit() override;
	// 通过 IRunable 继承
	virtual void Run() override;

	const std::shared_ptr<Table<BaseMprTools>>& GetToolsTable() { return m_MprTools; }

	virtual bool notify(QObject* obj, QEvent* ev) override;

	void ShowLoading(QThread* t);

private:
	void InitLogging();
	void InitTools();


public:
	MainWindow* m_MainWindow;
	std::shared_ptr<LoadMgr> m_LoadMgr;
	std::shared_ptr<ModelManager> m_ModelMgr;
	std::shared_ptr<Table<BaseMprTools>> m_MprTools;
	NThreadPool* ThreadPool;
};

#define gAppMain ((AppMain*)qApp)
#define gLoadMgr (gAppMain->m_LoadMgr)
#define gModelMgr (gAppMain->m_ModelMgr)
#define gMainWindow (gAppMain->m_MainWindow)