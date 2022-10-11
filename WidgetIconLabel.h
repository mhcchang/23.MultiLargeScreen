#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include "WidgetDetailInfo.h"

#include "ui_WidgetIconLabel.h"

//只有图标的标签  双击显示详细信息
class WidgetIconLabel : public QWidget
{
	Q_OBJECT

public:
	explicit WidgetIconLabel(QWidget *parent = Q_NULLPTR);
	~WidgetIconLabel();
	//void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	void SetInfo(QString sceneId, QString id, int flag, QList<QString> lsDeviceid);
	QString GetId() { return m_id; };
public slots:
	void CloseSubWin();
public:
signals:
	void ShowDetailSignle(QString id);
private:
	Ui::WidgetIconLabel ui;

	QString m_id;
	QString m_sceneId;
	QList<QString> m_lsDeviceid;

	bool m_blDrag;
	QPoint relativePos;
	int m_dragging;
	bool m_blShowDetail;
	QWidget* m_parent;
	WidgetDetailInfo* m_widgetDetailInfo;
};
