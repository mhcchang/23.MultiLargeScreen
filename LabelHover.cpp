#include "LabelHover.h"

LabelHover::LabelHover(QWidget *parent)
	: QLabel(parent)
{
}

LabelHover::~LabelHover()
{
}

void LabelHover::enterEvent(QEvent* event)
{
	setStyleSheet("QLabel{color: rgb(0, 252, 252);}; ");
}

void LabelHover::leaveEvent(QEvent* event)
{
	setStyleSheet("QLabel{color: rgb(255, 255, 255); }; ");
}