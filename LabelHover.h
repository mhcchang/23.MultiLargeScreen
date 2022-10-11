#pragma once

#include <QLabel>

class LabelHover : public QLabel
{
	Q_OBJECT

public:
	LabelHover(QWidget *parent = NULL);
	~LabelHover();

	void LabelHover::enterEvent(QEvent* event) Q_DECL_OVERRIDE;
	void LabelHover::leaveEvent(QEvent* event) Q_DECL_OVERRIDE;
};
