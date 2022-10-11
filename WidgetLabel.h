#pragma once
#pragma execution_character_set("utf-8")
#include <QLabel>
#include <QWidget>
#include "WidgetCameraList.h"
#include "ui_WidgetLabel.h"

class WidgetLabel : public QWidget
{	
	Q_OBJECT

public:
	WidgetLabel(QWidget *parent = Q_NULLPTR);
	~WidgetLabel();
	void SetText(QString str);
	void SetIcon(QString str);
	void SetInfo(QString strText, QString strImage, QString id = "");
	void SetInfoWithIcon(QString strText, QString strImagebase64, QString id, QList<QString> lsDeviceid);

	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	QString GetId() { return m_szId; };
public slots:
	void closeSub();
public:
signals:
	void ClickSignle(QString id);
private:
	Ui::WidgetLabel ui;
	QWidget * m_parent;
	QString m_szId;
	bool m_blShow;

	QList<QString> m_lsDeviceid;

	WidgetCameraList* m_widgetCamera;

};
