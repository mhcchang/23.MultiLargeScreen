#include "application.h"
#include <QDebug>
#include <QTranslator>
//#include "baselibdefine.h"
#include "MultiLargeScreenWidget.h"

#ifdef Q_QDOC
Application::Application(int &argc, char **argv)
	:QApplication(argc, argv)
{

}
#else
Application::Application(int &argc, char **argv, int flag)
	: QApplication(argc, argv, flag)
{
	
}
#endif

Application::~Application()
{

}

bool Application::notify(QObject *obj, QEvent * event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyNumberPress(keyEvent))
		{
			emit signal_keyPress(keyEvent);
			return true;
		}
	}
	else if (event->type() == QEvent::KeyRelease)
	{
		return true;
	}

	return QApplication::notify(obj, event);
}


bool Application::keyNumberPress(QKeyEvent * keyEvent)
{
	if (keyEvent->key() == Qt::Key_Escape)
	{
		return true;
	}
	else
		return false;

	if (keyEvent->key() == Qt::Key_0
		|| keyEvent->key() == Qt::Key_1
		|| keyEvent->key() == Qt::Key_2
		|| keyEvent->key() == Qt::Key_3
		|| keyEvent->key() == Qt::Key_4
		|| keyEvent->key() == Qt::Key_5
		|| keyEvent->key() == Qt::Key_6
		|| keyEvent->key() == Qt::Key_7
		|| keyEvent->key() == Qt::Key_8
		|| keyEvent->key() == Qt::Key_9
		|| keyEvent->key() == Qt::Key_Backspace
		|| keyEvent->key() == Qt::Key_Delete)
	{
		return true;
	}

	return false;
}

void Application::bindMain(MultiLargeScreenWidget* w)
{
	connect(this, &Application::signal_keyPress, w, &MultiLargeScreenWidget::slot_keyPressed);
}