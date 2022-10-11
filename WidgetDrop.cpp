#include "WidgetDrop.h"

WidgetDrop::WidgetDrop(QWidget *parent)
	: QWidget(parent)
{
	m_nFlag = 0;
	m_blDrag = false;

	ui.setupUi(this);
}

WidgetDrop::~WidgetDrop()
{
}

void WidgetDrop::mousePressEvent(QMouseEvent *event)
{
	if (!m_blDrag)
		return;
	if (1 == m_nFlag)
	{ //È«·ù
		m_dragging = 1;
		qvPoint0.setX(event->pos().x());
		qvPoint0.setY(event->pos().y());
		return;
	}
	if (event->button() == Qt::LeftButton) {
		relativePos = this->pos() - event->globalPos();
	}
}

void WidgetDrop::mouseMoveEvent(QMouseEvent *event)
{
	if (!m_blDrag)
		return;
	if (1 == m_nFlag)
	{
		qvPoint.setX(event->pos().x());
		qvPoint.setY(event->pos().y());
		update();
		return;
	}
	this->move(event->globalPos() + relativePos);
}

void WidgetDrop::mouseReleaseEvent(QMouseEvent *event)
{
	if (!m_blDrag)
		return;
	if (1 == m_nFlag)
		m_dragging = 0;
}
