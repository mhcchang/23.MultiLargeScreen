#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QMouseEvent>
#include <QLabel>

#include "ui_WidgetSlider.h"

class WidgetSlider : public QWidget
{
	Q_OBJECT

public:
	WidgetSlider(QWidget *parent = Q_NULLPTR);
	~WidgetSlider();

	void SetMinValue(int nMin);
	void SetMaxValue(int nMax);
	void SetValue(int nv);
	int GetMinValue() { return m_nMin; };
	int GetMaxValue() { return m_nMax; };

	int GetValue() { return m_nValue; };

	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	void WidgetSlider::mousePressEvent(QMouseEvent* event)Q_DECL_OVERRIDE;
	void WidgetSlider::mouseMoveEvent(QMouseEvent* event)Q_DECL_OVERRIDE;
	void WidgetSlider::mouseReleaseEvent(QMouseEvent *event)Q_DECL_OVERRIDE;
public slots:
	void btnBig();
	void btnSmall();
	void btnStop();
public:
signals:
	void btnBigSignal();
	void btnSmallSignal();
	void btnStopSignal();
private:
	Ui::WidgetSlider ui;

private:
	int m_nMin;
	int m_nMax;

	int m_nValue;

	bool m_blMouseDown;
	QLabel * m_lbl;
	void UpdateUI();
};
