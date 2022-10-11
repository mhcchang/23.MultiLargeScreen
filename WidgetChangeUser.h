#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "LabelClick.h"
#include "ui_WidgetChangeUser.h"

class WidgetChangeUser : public QWidget
{
	Q_OBJECT

public:
	WidgetChangeUser(QWidget *parent = Q_NULLPTR);
	~WidgetChangeUser();
	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

public slots:
	void changeUserSlot();
	void logoutSlot();

private:
	Ui::WidgetChangeUser ui;
};
