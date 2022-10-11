#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_WidgetRefList.h"
#include "PubDefs.h"

class WidgetRefList : public QWidget
{
	Q_OBJECT

public:
	explicit WidgetRefList(QWidget *parent = Q_NULLPTR);
	~WidgetRefList();
	Ui::WidgetRefList ui;
	void Resizefont(float fZoom);
	void AddDevice(STRU_DB_DEVICE &deviceInf);

	void SetScene(int npos);
public slots:
	//void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
	void DbClickSignal(QListWidgetItem *item);
public:
signals:
	void ChangeScene(QString sceneId);
private:
	float m_fZoom;

	int m_nPos;
	QList<STRU_DB_DEVICE> m_lsDevice;
};
