#pragma once
#pragma execution_character_set("utf-8")
#include <QtWidgets/QMainWindow>
#include "ui_MultiLargeScreen.h"
#include <QLabel>

class MultiLargeScreen : public QMainWindow
{
    Q_OBJECT

public:
    MultiLargeScreen(QWidget *parent = Q_NULLPTR);
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
private:
    Ui::MultiLargeScreenClass ui;

	QLabel * lblImg;
	QPixmap *pixw;
	QImage image;
};
