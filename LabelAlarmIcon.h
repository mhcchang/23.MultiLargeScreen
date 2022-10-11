#pragma once
#pragma execution_character_set("utf-8")
#include "LabelClick.h"
#include <QTime>
#include "PubDefs.h"

#include "WidgetDetailInfoAlarm.h"
#include "WidgetAlarmInfo.h"

class LabelAlarmIcon : public LabelClick
{
	Q_OBJECT

public:
	LabelAlarmIcon(QWidget *parent = NULL);
	~LabelAlarmIcon();

	void closeEvent(QCloseEvent* event) Q_DECL_OVERRIDE;
	void Init(int flag, QString lblId = "", MqAlarmInfo * pInf = nullptr, bool blShow = false);
	//¥ÊªÓ ±º‰
	uint64_t GetLiveMillSecond();
	void GetLabelInfo(QString & lblid, QString &lat, QString & lng);
private:
	QString m_szLabelId;
	QTime m_tm;
	QWidget * m_parent;

	bool m_blShowDetail;
	MqAlarmInfo* m_pInf;
	
	WidgetDetailInfoAlarm * m_widgetDetailInfo;
	WidgetAlarmInfo * m_widgetAlarmInfo;

protected:
	void mousePressEvent(QMouseEvent* event);
public slots:
	void ShowLabelPlaceSlot(QString lblid, QString strx, QString stry);

	void CloseSubWin(int i);
};
