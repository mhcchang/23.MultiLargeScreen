#include "WidgetSliderV.h"
#include <QDebug>

WidgetSliderV::WidgetSliderV(QWidget *parent)
	: QWidget(parent)
{
	m_nMin = 0;
	m_nMax = 100;
	ui.setupUi(this);
	m_nValue = 0;
	m_lbl = new QLabel(this);
	m_lbl->setStyleSheet("image: url(:/UI/icon/slidebutton.png);");
	m_lbl->resize(21, 21);
	UpdateUI();
	m_lbl->show();

	m_blMouseDown = false;
	connect(ui.btnBig, &QPushButton::clicked, this, &WidgetSliderV::btnBig, Qt::UniqueConnection);
	connect(ui.btnSmall, &QPushButton::clicked, this, &WidgetSliderV::btnSmall, Qt::UniqueConnection);
}

WidgetSliderV::~WidgetSliderV()
{
}

void WidgetSliderV::btnBig()
{
	if (m_nValue < m_nMax)
		m_nValue++;
	UpdateUI();

	emit btnBigSignal();
}

void WidgetSliderV::btnSmall()
{
	if (m_nValue > m_nMin)
		m_nValue--;

	UpdateUI();
	emit btnSmallSignal();
}

void WidgetSliderV::SetMinValue(int nMin)
{
	m_nMin = nMin;
}

void WidgetSliderV::SetMaxValue(int nMax)
{
	m_nMax = nMax;
}

void WidgetSliderV::SetValue(int nv)
{
	m_nValue = nv;
	if (nv < m_nMin)
		m_nValue = m_nMin;

	if (nv > m_nMax)
		m_nValue = m_nMax;

	UpdateUI();
}

void WidgetSliderV::UpdateUI()
{
	QRect rt = ui.label->geometry();
	int nValue = (rt.height() - m_lbl->height()) * 1.0f;

 	m_lbl->setGeometry(rt.x() + rt.width() / 2 - m_lbl->width() / 2, rt.y() + (rt.height() - m_lbl->height()) * ( (m_nMax - m_nValue + m_nMin) * 1.0f / (m_nMax - m_nMin)), m_lbl->width(), m_lbl->height());
	qDebug() << "+++ y: " << rt.y() + (rt.height() - m_lbl->height()) * ((m_nMax - m_nValue + m_nMin) * 1.0f / (m_nMax - m_nMin)) << " m_nValue:" << m_nValue;

	emit ValueChangeSignal(m_nValue * 1.0f / (m_nMax - m_nMin));
}
//(m_nMax - m_nValue + m_nMin)

void WidgetSliderV::resizeEvent(QResizeEvent *event)
{
	UpdateUI();
	m_lbl->raise();
}

void WidgetSliderV::mousePressEvent(QMouseEvent* event)
{
	m_blMouseDown = true;
}

void WidgetSliderV::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_blMouseDown)
		return;
	QPoint pt = event->pos();

	QRect rt = ui.label->geometry();
	if (pt.y() < rt.y() || pt.y() > rt.y() + rt.height())
		return;

	int y = pt.y() - rt.y();
	m_nValue = y * 1.0 / rt.height() * (m_nMax - m_nMin);
	m_nValue = m_nMax - m_nValue + m_nMin;
	UpdateUI();
}

void WidgetSliderV::mouseReleaseEvent(QMouseEvent *event)
{
	m_blMouseDown = false;
	QPoint pt = event->pos();

	QRect rt = ui.label->geometry();
	if (pt.y() < rt.y() || pt.y() > rt.y() + rt.height())
		return;

	int y = pt.y() - rt.y();
	m_nValue = y * 1.0 / rt.height() * (m_nMax - m_nMin);
	m_nValue = m_nMax - m_nValue + m_nMin;

	UpdateUI();
}
