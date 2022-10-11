#include "WidgetLogin.h"
#include <QDebug>
#include "WidgetForgotPwd.h"

WidgetLogin::WidgetLogin(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	hide();
	m_fZoom = 1.0;

	connect(ui.btnForgot, &LabelClick::clicked, this, &WidgetLogin::btnForgotSlot, Qt::UniqueConnection);
	connect(ui.btnLogin, &LabelClick::clicked, this, &WidgetLogin::btnLoginSlot, Qt::UniqueConnection);
}

WidgetLogin::~WidgetLogin()
{
}

void WidgetLogin::Resizefont(float fZoom)
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

	QList<QLineEdit *> lsEdt = this->findChildren<QLineEdit *>();
	for (int i = 0; i < lsEdt.size(); i++)
	{
		QLineEdit * edt = lsEdt.at(i);

		qDebug() << edt->objectName();

		ft = edt->font();

		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(ft.pointSize() / m_fZoom);
		edt->setFont(ft);
	}
}

void WidgetLogin::btnForgotSlot()
{
	WidgetForgotPwd* w = new WidgetForgotPwd(this);
	
	w->move(this->width() / 2.0 - w->width() / 2, this->height() / 2.0 - w->height() / 2);
	w->Resizefont(m_fZoom);
	w->show();
}

void WidgetLogin::btnLoginSlot()
{
	this->hide();
}