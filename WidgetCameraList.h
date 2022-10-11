#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_WidgetCameraList.h"
#include "WidgetDetailInfo.h"
 
class WidgetCameraList : public QWidget
{
	Q_OBJECT

public:
	WidgetCameraList(QWidget *parent = Q_NULLPTR);
	~WidgetCameraList();
	void Resizefont(float fZoom);

	void Init(QString szId);
	void closeEvent(QCloseEvent* e) Q_DECL_OVERRIDE;

private slots:
	void DbClickSignal(QListWidgetItem* item);

public:
	signals:
	void WinClosed();
private:
	Ui::WidgetCameraList ui;
	float m_fZoom;
	QWidget * m_parent;
	QString m_szDeviceId;
	bool m_blShowDetail;
	WidgetDetailInfo* m_widgetDetailInfo;

	void ShowDetail();
};
