#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_WidgetDetailInfo.h"
#include "PubDefs.h"
#include "WidgetVideo.h"

class WidgetDetailInfoAlarm : public QWidget
{
	Q_OBJECT

public:
	WidgetDetailInfoAlarm(QWidget *parent = Q_NULLPTR);
	~WidgetDetailInfoAlarm();
	Ui::WidgetDetailInfo ui;

	void Resizefont(float fZoom);

	void SetPlayCloseAnimation(bool blPlay) { m_blPlayCloseAnimation = blPlay; };
	bool GetPlayCloseAnimation() { return m_blPlayCloseAnimation;  };
	void SetPlayShowAnimation(bool blPlay) { m_blPlayShowAnimation = blPlay; };
	bool GetPlayShowAnimation() { return m_blPlayShowAnimation;	};

	void InitInf(MqAlarmInfo* pInf);

	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
	void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
	void playCloseAnimation();
	void playShowAnimation();

	void OpenStream(QString url);
	void CloseStream();

public slots:
	void SendClose();

	void Preset1Slot();
	void Preset2Slot();
	void Preset3Slot();
	void Preset4Slot();

public:
signals:
	void Closed(int i);

	void ShowFullDomeSignle(QString deviceid);
private:
	float m_fZoom;

	QString m_sceneId;
	QString m_labelid;
	QString m_deviceid;

	WidgetVideo* m_widgetVideo;

	bool m_blPlayCloseAnimation;
	bool m_blPlayShowAnimation;

	bool m_blCloseAnimationState;
	bool m_blShowAnimationState;
	QLabel * m_lbl;
	std::vector<STRU_DB_PRESET> m_presets;
};
