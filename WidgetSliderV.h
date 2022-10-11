#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QMouseEvent>
#include <QLabel>

#include "ui_WidgetSliderV.h"

class WidgetSliderV : public QWidget
{
	Q_OBJECT

public:
	WidgetSliderV(QWidget *parent = Q_NULLPTR);
	~WidgetSliderV();

	void SetMinValue(int nMin);
	void SetMaxValue(int nMax);
	void SetValue(int nv);
	int GetMinValue() { return m_nMin; };
	int GetMaxValue() { return m_nMax; };

	int GetValue() { return m_nValue; };

	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

	void mousePressEvent(QMouseEvent* event)Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event)Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event)Q_DECL_OVERRIDE;
public slots:
	void btnBig();
	void btnSmall();
public:
signals:
	void btnBigSignal();
	void btnSmallSignal();
	void ValueChangeSignal(float nValue);
private:
	Ui::WidgetSliderV ui;

private:
	int m_nMin;
	int m_nMax;

	int m_nValue;

	bool m_blMouseDown;
	QLabel * m_lbl;
	void UpdateUI();
};
