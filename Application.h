#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

#include "MultiLargeScreenWidget.h"

class Application : public QApplication
{
	Q_OBJECT
public:
#ifdef Q_QDOC
	Application(int &argc, char **argv);
#else
	Application(int &argc, char **argv, int flag = ApplicationFlags);
#endif

	virtual ~Application();

private:
	bool keyNumberPress(QKeyEvent *);

protected:
	bool notify(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

public:
	//MultiLargeScreenWidget * w;
	void bindMain(MultiLargeScreenWidget* w);

signals:
	void signal_keyPress(QKeyEvent *keyEvent);
};

#endif // APPLICATION_H