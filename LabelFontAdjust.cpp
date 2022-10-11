#include "LabelFontAdjust.h"

LabelFontAdjust::LabelFontAdjust(QWidget *parent)
	: QLabel(parent)
{
	m_fZoom = 1;
	m_fFontSize = 12;
}

LabelFontAdjust::~LabelFontAdjust()
{
}

void LabelFontAdjust::SetZoom(float zoom)
{
	m_fZoom = zoom;
	QFont ft;
	ft = this->font();
	int sz = ft.pointSize();
	sz /= m_fZoom;
	m_fFontSize = sz;
	ft.setPointSize(sz);
	this->setFont(ft);
}

void LabelFontAdjust::showEvent(QShowEvent *event)
{
	update();
}
