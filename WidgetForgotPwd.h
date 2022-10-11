#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_WidgetForgotPwd.h"
#include <QDebug>

class WidgetForgotPwd : public QWidget
{
	Q_OBJECT

public:
	WidgetForgotPwd(QWidget *parent = Q_NULLPTR);
	~WidgetForgotPwd();
	void Resizefont(float fZoom);

public slots:
	void closeBtnSlot();
private:
	Ui::WidgetForgotPwd ui;
	float m_fZoom;
};
