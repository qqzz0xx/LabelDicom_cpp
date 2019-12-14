#include "AppMain.h"

#include "LocationTools.h"
#include "LandMarkTools.h"
#include "WinLevelWidthTools.h"
#include "PolygonTools.h"

#include <QFile>
#include <QMovie>

#include "NThreadPool.h"

AppMain::AppMain(int& argc, char** argv) :
	QApplication(argc, argv)
{
	QFile f(":qdarkstyle/style.qss");
	if (!f.exists())
	{
		printf("Unable to set stylesheet, file not found\n");
	}
	else
	{
		f.open(QFile::ReadOnly | QFile::Text);
		QTextStream ts(&f);
		qApp->setStyleSheet(ts.readAll());
	}


	InitLogging();
}

AppMain::~AppMain()
{
}

void AppMain::Enter()
{
	ThreadPool = new NThreadPool(this);

	m_LoadMgr = std::make_shared<LoadMgr>();
	m_ModelMgr = std::make_shared<ModelManager>();

	m_MainWindow = new MainWindow();

	m_MprTools = std::make_shared<Table<BaseMprTools>>();

	InitTools();
}

void AppMain::Exit()
{
	delete m_MainWindow;
	delete ThreadPool;

}

void AppMain::Run()
{
	/*auto slice = new SliceComponent(m_MainWindow);
	slice->show();*/
	m_MainWindow->show();
}

bool AppMain::notify(QObject* obj, QEvent* ev)
{
	bool ok;
	try
	{
		ok = __super::notify(obj, ev);
	}
	catch (std::exception& ex)
	{
		qDebug() << ex.what();
	}

	return ok;
}


void AppMain::ShowLoading(QThread* t)
{
	QLabel* lbl = new QLabel;
	QMovie* movie = new QMovie(":icon/icon/ajax-loader.gif");
	lbl->setMovie(movie);
	lbl->show();
	movie->start();

	if (!t->isFinished())
	{
		QApplication::processEvents();
	}

	delete lbl;
	delete movie;
	
}

void AppMain::InitLogging()
{

	FLAGS_log_dir = "./log";
	google::InitGoogleLogging("RaySharp-Anno");

#ifdef _DEBUG
	google::SetStderrLogging(google::GLOG_INFO); //设置级别高于 google::INFO 的日志同时输出到屏幕
#else
	google::SetStderrLogging(google::GLOG_FATAL);//设置级别高于 google::FATAL 的日志同时输出到屏幕
#endif
	FLAGS_colorlogtostderr = true; //设置输出到屏幕的日志显示相应颜色
	google::SetLogDestination(google::GLOG_FATAL, "./log/log_fatal_"); // 设置 google::FATAL 级别的日志存储路径和文件名前缀
	google::SetLogDestination(google::GLOG_ERROR, "./log/log_error_"); //设置 google::ERROR 级别的日志存储路径和文件名前缀
	google::SetLogDestination(google::GLOG_WARNING, "./log/log_warning_"); //设置 google::WARNING 级别的日志存储路径和文件名前缀
	google::SetLogDestination(google::GLOG_INFO, "./log/log_info_"); //设置 google::INFO 级别的日志存储路径和文件名前缀
	FLAGS_logbufsecs = 0; //缓冲日志输出，默认为30秒，此处改为立即输出
	FLAGS_max_log_size = 100; //最大日志大小为 100MB
	FLAGS_stop_logging_if_full_disk = true; //当磁盘被写满时，停止日志输出
}

void AppMain::InitTools()
{
	auto func = [&](auto tools) {
		m_MprTools->Add(tools);
	};

	//func(std::make_shared<LocationTools>());
	//func(std::make_shared<LandMarkTools>());
	//func(std::make_shared<WinLevelWidthTools>());
	//func(std::make_shared<PolygonTools>());
}
