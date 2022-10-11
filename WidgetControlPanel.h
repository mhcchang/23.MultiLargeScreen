#pragma once
#pragma execution_character_set("utf-8")
#include <QMouseEvent>
#include <QWidget>
#include "ui_WidgetControlPanel.h"
#include <QImage>

class WidgetControlPanel : public QLabel
{
	Q_OBJECT

public:
	WidgetControlPanel(QWidget *parent = nullptr);
	~WidgetControlPanel();

	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
	void ResizeUI();
	void ResizeUI(QSize sz);
public:
	signals:
		void OnClickTop();
		void OnClickTopRight();
		void OnClickRight();
		void OnClickBottomRight();
		void OnClickBottom();
		void OnClickBottomLeft();
		void OnClickLeft();
		void OnClickTopLeft();
		void OnStop();
private:

	QImage m_image;
	QImage m_imagerotate;
	void CheckPlace(float x, float y);

	bool LoadImg(QString apwzFileName, QImage* apImge, int w, int h);
	bool GetAnchorHeadPixmap(QPixmap& aoPixmap, QString apwzHeadPath, int w, int h);
};
