#include "WidgetForgotPwd.h"
#include <LabelClick.h>

WidgetForgotPwd::WidgetForgotPwd(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_fZoom = 1.0f;
	this->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(ui.closeBtn, &LabelClick::clicked, this, &WidgetForgotPwd::closeBtnSlot, Qt::UniqueConnection);
}

WidgetForgotPwd::~WidgetForgotPwd()
{
}

void WidgetForgotPwd::Resizefont(float fZoom)
{
	m_fZoom = fZoom;
	QList<QLabel *> lsLbl = this->findChildren<QLabel *>();
	QFont ft;
	for (int i = 0; i < lsLbl.size(); i++)
	{
		QLabel * lbl = lsLbl.at(i);

		qDebug() << lbl->objectName();

		ft = lbl->font();

		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(ft.pointSize() / m_fZoom);
		lbl->setFont(ft);
	}
}

void WidgetForgotPwd::closeBtnSlot()
{
	this->close();
}
