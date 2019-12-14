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
	google::SetStderrLogging(google::GLOG_INFO); //���ü������ google::INFO ����־ͬʱ�������Ļ
#else
	google::SetStderrLogging(google::GLOG_FATAL);//���ü������ google::FATAL ����־ͬʱ�������Ļ
#endif
	FLAGS_colorlogtostderr = true; //�����������Ļ����־��ʾ��Ӧ��ɫ
	google::SetLogDestination(google::GLOG_FATAL, "./log/log_fatal_"); // ���� google::FATAL �������־�洢·�����ļ���ǰ׺
	google::SetLogDestination(google::GLOG_ERROR, "./log/log_error_"); //���� google::ERROR �������־�洢·�����ļ���ǰ׺
	google::SetLogDestination(google::GLOG_WARNING, "./log/log_warning_"); //���� google::WARNING �������־�洢·�����ļ���ǰ׺
	google::SetLogDestination(google::GLOG_INFO, "./log/log_info_"); //���� google::INFO �������־�洢·�����ļ���ǰ׺
	FLAGS_logbufsecs = 0; //������־�����Ĭ��Ϊ30�룬�˴���Ϊ�������
	FLAGS_max_log_size = 100; //�����־��СΪ 100MB
	FLAGS_stop_logging_if_full_disk = true; //�����̱�д��ʱ��ֹͣ��־���
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
