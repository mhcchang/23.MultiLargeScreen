#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "WidgetVideo.h"

#include "ui_WidgetDomeVideo.h"

#include "WidgetSlider.h"

class WidgetDomeVideo : public WidgetVideo
{
	Q_OBJECT

public:
	WidgetDomeVideo(QWidget *parent = Q_NULLPTR);
	~WidgetDomeVideo();

	void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

	void enterEvent(QEvent *event) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	void SetInfo(QString deviceid, bool blHasPtz);
	WidgetSlider* m_widgetSlider;
	Ui::WidgetDomeVideo ui;

private slots:
	//øÿ÷∆≈Ã
	void CtrlPanelTopSlot();
	void CtrlPanelTopRightSlot();
	void CtrlPanelRightSlot();
	void CtrlPanelBottomRightSlot();
	void CtrlPanelBottomSlot();
	void CtrlPanelBottomLeftSlot();
	void CtrlPanelLeftSlot();
	void CtrlPanelTopLeftSlot();
	void ZoominSlot();
	void ZoomoutSlot();
	void CtrlPanelStop();
	void PresetChangeSlot(QListWidgetItem *item);

private:

	QWidget * m_parent;

	QLabel * lblImgBack;
	//QImage image;
	QRect m_rtDefault;
	Qt::WindowFlags m_winType;

	QString m_strImg;

	QLabel * m_lblFront;

	QString m_szDeviceId;
	bool m_blHasPtz;
};
