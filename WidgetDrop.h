#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_WidgetDrop.h"
#include <QMouseEvent>

class WidgetDrop : public QWidget
{
	Q_OBJECT

public:
	WidgetDrop(QWidget *parent = Q_NULLPTR);
	~WidgetDrop();

protected:
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
	Ui::WidgetDrop ui;

private:
	bool m_blDrag;
	int m_nFlag;

	int m_dragging;

	QPoint qvPoint;
	QPoint qvPoint0;

	QPoint relativePos;
};
