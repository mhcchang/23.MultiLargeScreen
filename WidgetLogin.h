#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_WidgetLogin.h"

class WidgetLogin : public QWidget
{
	Q_OBJECT

public:
	WidgetLogin(QWidget *parent = Q_NULLPTR);
	~WidgetLogin();
	void Resizefont(float fZoom);

public slots:
	void btnForgotSlot();
	void btnLoginSlot();
private:
	Ui::WidgetLogin ui;
	float m_fZoom;
};
