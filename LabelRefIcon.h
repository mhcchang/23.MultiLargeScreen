#pragma once
#pragma execution_character_set("utf-8")
#include "LabelClick.h"
#include <QTime>
#include "PubDefs.h"

#include "WidgetDetailInfoAlarm.h"
#include "WidgetAlarmInfo.h"

class LabelRefIcon : public LabelClick
{
	Q_OBJECT

public:
	LabelRefIcon(QWidget *parent = NULL);
	~LabelRefIcon();

	void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
	void Init(int flag, STRU_DB_DEVICE inf, QString lblId = "", bool blShow = false);
	//¥ÊªÓ ±º‰
	uint64_t GetLiveMillSecond();
	void GetLabelInfo(QString & lblid, QString &lat, QString & lng);
private:
	QString m_szLabelId;
	QTime m_tm;
	QWidget * m_parent;

	STRU_DB_DEVICE m_inf;
protected:
	void mousePressEvent(QMouseEvent* event);
public slots:
	void ShowLabelPlaceSlot(QString lblid, QString strx, QString stry);

	void CloseSubWin(int i);
};
