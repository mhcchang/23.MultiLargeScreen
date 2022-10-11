#include "WidgetLabel.h"

WidgetLabel::WidgetLabel(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	ui.lblIcon->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	m_blShow = false;

	m_parent = parent;
}

WidgetLabel::~WidgetLabel()
{
}

void WidgetLabel::SetText(QString str)
{
	ui.lblText->setText(str);
}

void WidgetLabel::SetIcon(QString str)
{
	ui.lblIcon->setPixmap(QPixmap(str));
}

void WidgetLabel::SetInfo(QString strText, QString strImage, QString id)
{
	ui.lblText->setText(strText);
	ui.lblIcon->setPixmap(QPixmap(strImage));

	m_szId = id;
}

void WidgetLabel::SetInfoWithIcon(QString strText, QString strImagebase64, QString id, QList<QString> lsDeviceid)
{
	ui.lblText->setText(strText);
	QPixmap image;
	image.loadFromData(QByteArray::fromBase64(strImagebase64.toLocal8Bit() ) );
	ui.lblIcon->setPixmap(image);

	m_szId = id;

	m_lsDeviceid = lsDeviceid;
}

void WidgetLabel::mousePressEvent(QMouseEvent *event)
{
	if (!m_blShow)
	{
		emit ClickSignle(m_szId);
		m_widgetCamera = new WidgetCameraList(m_parent);
		m_widgetCamera->Init(m_szId);
		QRect rt = this->geometry();
		m_widgetCamera->move(rt.x() + rt.width() + 20, rt.y());

		connect(m_widgetCamera, &WidgetCameraList::WinClosed, this, &WidgetLabel::closeSub, Qt::AutoConnection);
		m_widgetCamera->show();

		m_blShow = true;
	}
	else
	{
		m_blShow = false;
		m_widgetCamera->close();
	}
}

void WidgetLabel::closeSub()
{
	m_blShow = false;
}
