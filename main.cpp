//#include "MultiLargeScreen.h"

#include "MultiLargeScreenWidget.h"

#include <QtWidgets/QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>
#include <Log4Qt>

#include "Application.h"
////////////////////////
#include "PubUnit.h"

using namespace Log4Qt;
extern Log4Qt::Logger * g_logger;

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{	
	EXCEPTION_RECORD* record = pException->ExceptionRecord;
	QString errCode(QString::number(record->ExceptionCode, 16)), errAdr(QString::number((uint64_t)record->ExceptionAddress, 16)), errMod;
	g_logger->error(QString("<div>错误代码：%1</div><div>错误地址：%2</div></FONT>").arg(errCode).arg(errAdr));

	QMessageBox::critical(NULL, "程序异常", "<FONT size=4><div><b>对于发生的错误，表示诚挚的歉意</b><br/></div>" +
		QString("<div>错误代码：%1</div><div>错误地址：%2</div></FONT>").arg(errCode).arg(errAdr),
		QMessageBox::Ok);
	return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);
	//GlobalApplication *app = new GlobalApplication(argc, argv);
	Application app(argc, argv);
    //MultiLargeScreen w;
	qInfo() << "QApplication start";
	//SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注冊异常捕获函数
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(codec);
	g_logger = NULL;
	///------ log4qt
	g_logger = Log4Qt::LogManager::rootLogger();
	TTCCLayout *p_layout = new TTCCLayout();

	//p_layout->setDateFormat("ISO8601");// "yyyy-MM-dd HH:mm:ss");
	//p_layout->setName(QLatin1String("My Layout"));
	//p_layout->activateOptions();

	//// 创建ConsoleAppender
	//Log4Qt::DailyRollingFileAppender *appender = new Log4Qt::DailyRollingFileAppender;
	//// 设置输出目的地为应用程序所在目录下的logFile.log
	//appender->setFile("logFile.log");
	//// 设置日志文件每天回滚
	////appender->setDatePattern(Log4Qt::DailyRollingFileAppender::MINUTELY_ROLLOVER);
	//appender->setDatePattern("'.'yyyy-MM-dd-hh");// -mm");
	//// 设置日志为追加方式写入输出文件
	//appender->setAppendFile(true);
	//appender->setLayout(p_layout);
	//// 设置编码
	//appender->setEncoding(QTextCodec::codecForName("UTF-8"));

	//appender->setImmediateFlush(true);
	//// 设置阈值级别为INFO
	//appender->setThreshold(Log4Qt::Level::INFO_INT);
	//// 激活选项
	//appender->activateOptions();
	//g_logger->addAppender(appender);
	//// 设置级别为 DEBUG
	//g_logger->setLevel(Log4Qt::Level::DEBUG_INT);

	qInfo() << "MultiLargeScreenWidget start";
	MultiLargeScreenWidget w;
	app.bindMain(&w);
    //w.showFullScreen();
	w.showMaximized();
    return app.exec();
}
