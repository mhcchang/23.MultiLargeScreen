#include "LabelClick.h"
//const QString& text = "", 
//const QString& text, 
LabelClick::LabelClick(QWidget* parent)
	: QLabel(parent)
{
	//setText(text);

	m_blChecked = false;
	m_blCheckEnable = false;
}

LabelClick::~LabelClick()
{

}

void LabelClick::mousePressEvent(QMouseEvent* event)
{
	if (m_blCheckEnable)
		m_blChecked = !m_blChecked;
	emit clicked();
}

void LabelClick::enterEvent(QEvent* event)
{
	m_szStyleSheet = this->styleSheet();
	if (m_szStyleSheet == "")
	{
		setStyleSheet("QLabel{color: rgb(0, 252, 252);}; ");
	}
}

void LabelClick::leaveEvent(QEvent* event)
{
	if (m_blCheckEnable && m_blChecked)
	{
		setStyleSheet("QLabel{color: rgba(0, 255, 255); }; ");
		return;
	}
	else if (m_blCheckEnable && !m_blChecked)
	{
		setStyleSheet("QLabel{color: rgba(222, 223, 225); }; ");
		return;
	}
	if (m_szStyleSheet == "")
	{
		if (m_blCheckEnable && m_blChecked)
			setStyleSheet("QLabel{color: rgba(0, 255, 255); }; ");
		else
			setStyleSheet("QLabel{color: rgba(222, 223, 225); }; ");
	}
	else
		setStyleSheet(m_szStyleSheet);
}

void LabelClick::CheckEnable(bool blCheckEnable)
{
	m_blCheckEnable = blCheckEnable;
}

void LabelClick::SetCheckStatus(bool blChecked)
{
	m_blChecked = blChecked;
	if (m_blCheckEnable && m_blChecked)
		setStyleSheet("QLabel{color: rgba(0, 255, 255); }; ");
	else
		setStyleSheet("QLabel{color: rgba(222, 223, 225); }; ");
}
