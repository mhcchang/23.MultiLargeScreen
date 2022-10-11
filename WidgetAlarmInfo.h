#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include "ui_WidgetAlarmInfo.h"
#include "PubDefs.h"
#include <QCloseEvent>
#include <QShowEvent>

class WidgetAlarmInfo : public QWidget
{
	Q_OBJECT

public:
	WidgetAlarmInfo(QWidget *parent = Q_NULLPTR);
	~WidgetAlarmInfo();

	void InitInf(MqAlarmInfo* pInf);
	void Resizefont(float fZoom);

	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
	void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
	void playCloseAnimation();
	void playShowAnimation();

	void SetPlayCloseAnimation(bool blPlay) { m_blPlayCloseAnimation = blPlay; };
	bool GetPlayCloseAnimation() { return m_blPlayCloseAnimation; };
	void SetPlayShowAnimation(bool blPlay) { m_blPlayShowAnimation = blPlay; };
	bool GetPlayShowAnimation() { return m_blPlayShowAnimation; };
public slots:
	void SendClose();
public:
signals:
	void Closed(int i);
	void AlarmInfoSignal(QString szDesc);
private:
	Ui::WidgetAlarmInfo ui;

	float m_fZoom;
	bool m_blPlayCloseAnimation;
	bool m_blPlayShowAnimation;

	bool m_blCloseAnimationState;
	bool m_blShowAnimationState;
};
