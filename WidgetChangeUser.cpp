#include "WidgetChangeUser.h"

#include "MultiLargeScreenWidget.h"
extern MultiLargeScreenWidget * g_pMain;

WidgetChangeUser::WidgetChangeUser(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	hide();

	connect(ui.label, &LabelClick::clicked, this, &WidgetChangeUser::changeUserSlot, Qt::UniqueConnection);
	connect(ui.label_2, &LabelClick::clicked, this, &WidgetChangeUser::logoutSlot, Qt::UniqueConnection);
}

WidgetChangeUser::~WidgetChangeUser()
{
}

//WidgetLogin.cpp
void WidgetChangeUser::changeUserSlot()
{
	if (g_pMain)
		g_pMain->changeUser();
	this->hide();
}

void WidgetChangeUser::logoutSlot()
{
	if (g_pMain)
		g_pMain->logout();
	this->hide();
}

void WidgetChangeUser::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		/*if (Qt::WindowFullScreen == (this->windowState() & Qt::WindowFullScreen))
			this->showMaximized();
		else
			this->showFullScreen();*/
		this->hide();
		break;

	default:
		QWidget::keyPressEvent(event);
	}
}
