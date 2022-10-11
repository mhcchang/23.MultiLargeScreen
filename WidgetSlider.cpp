#include "WidgetSlider.h"

WidgetSlider::WidgetSlider(QWidget *parent)
	: QWidget(parent)
{
	m_nMin = 0;
	m_nMax = 100;
	ui.setupUi(this);
	m_nValue = 20;
	m_lbl = new QLabel(this);
	m_lbl->setStyleSheet("image: url(:/UI/icon/slidebutton.png);");
	m_lbl->resize(21, 21);

	UpdateUI();
	m_lbl->show();

	m_blMouseDown = false;
	//connect(ui.btnBig, &QPushButton::clicked, this, &WidgetSlider::btnBig, Qt::UniqueConnection);
	//connect(ui.btnSmall, &QPushButton::clicked, this, &WidgetSlider::btnSmall, Qt::UniqueConnection);

	connect(ui.btnBig, &QPushButton::pressed, this, &WidgetSlider::btnBig, Qt::UniqueConnection);
	connect(ui.btnSmall, &QPushButton::pressed, this, &WidgetSlider::btnSmall, Qt::UniqueConnection);
	connect(ui.btnBig, &QPushButton::released, this, &WidgetSlider::btnStop, Qt::UniqueConnection);
	connect(ui.btnSmall, &QPushButton::released, this, &WidgetSlider::btnStop, Qt::UniqueConnection);
}

WidgetSlider::~WidgetSlider()
{
}

void WidgetSlider::btnBig()
{
	//if (m_nValue < m_nMax)
	//	m_nValue++;
	//UpdateUI();
	emit btnBigSignal();
}

void WidgetSlider::btnSmall()
{
	//if (m_nValue > m_nMin)
	//	m_nValue--;

	//UpdateUI();
	emit btnSmallSignal();
}

void WidgetSlider::btnStop()
{
	emit btnStopSignal();
}

void WidgetSlider::SetMinValue(int nMin)
{
	m_nMin = nMin;
}

void WidgetSlider::SetMaxValue(int nMax)
{
	m_nMax = nMax;
}

void WidgetSlider::SetValue(int nv)
{
	m_nValue = nv;
	if (nv < m_nMin)
		m_nValue = m_nMin;

	if (nv > m_nMax)
		m_nValue = m_nMax;

	UpdateUI();
}

void WidgetSlider::UpdateUI()
{
	QRect rt = ui.label->geometry();

	m_lbl->setGeometry(rt.x() + (rt.width()- m_lbl->width()) * 1.0f / (m_nMax - m_nMin) * m_nValue, rt.y() + rt.height() / 2 - m_lbl->height() / 2, m_lbl->width(), m_lbl->height());
}

void WidgetSlider::resizeEvent(QResizeEvent *event)
{
	UpdateUI();
	m_lbl->raise();
}

void WidgetSlider::mousePressEvent(QMouseEvent* event)
{
	m_blMouseDown = true;
}

void WidgetSlider::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_blMouseDown)
		return;
	QPoint pt = event->pos();

	QRect rt = ui.label->geometry();
	if (pt.x() < rt.x() || pt.x() > rt.x() + rt.width())
		return;

	int x = pt.x() - rt.x();
	m_nValue = x * 1.0 / rt.width() * (m_nMax - m_nMin);
	UpdateUI();
}

void WidgetSlider::mouseReleaseEvent(QMouseEvent *event)
{
	m_blMouseDown = false;

	QPoint pt = event->pos();

	QRect rt = ui.label->geometry();
	if (pt.x() < rt.x() || pt.x() > rt.x() + rt.width())
		return;

	int x = pt.x() - rt.x();
	m_nValue = x * 1.0 / rt.width() * (m_nMax - m_nMin);
	UpdateUI();
}
