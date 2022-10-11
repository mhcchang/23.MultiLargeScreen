#pragma once

#include <QLabel>

class LabelClick : public QLabel
{
	Q_OBJECT
public:
	LabelClick(QWidget* parent = NULL);
	~LabelClick();

	void CheckEnable(bool blCheckEnable);
	void SetCheckStatus(bool blChecked);
	void enterEvent(QEvent* event) Q_DECL_OVERRIDE;
	void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;
signals:
	void clicked();
protected:
	void mousePressEvent(QMouseEvent* event);

private:
	QString m_szStyleSheet;
	bool m_blChecked;
	bool m_blCheckEnable;
};
