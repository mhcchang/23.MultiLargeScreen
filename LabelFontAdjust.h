/*******************************************************************************
* @brief ������Ļ�ֱ��ʷŴ��������ֺ�
* @detail  ͨ��SetZoom ������Ļ�ֱ��ʵķŴ���
* @author  zhuohe tech
* @date    1/7/2022
*******************************************************************************/

#pragma once

#include <QLabel>

class LabelFontAdjust : public QLabel
{
	Q_OBJECT

public:
	explicit LabelFontAdjust(QWidget *parent = Q_NULLPTR);
	~LabelFontAdjust();

	void SetZoom(float zoom);
	void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;

private:
	
	float m_fZoom;
	int m_fFontSize;
};
