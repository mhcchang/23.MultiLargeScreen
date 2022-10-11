#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_WidgetVideoSegment.h"

#include "PubDefs.h"
#include "WidgetCam.h"
#include <QStringList>

class WidgetVideoSegment : public QWidget
{
	Q_OBJECT

public:
	WidgetVideoSegment(QWidget *parent = Q_NULLPTR);
	~WidgetVideoSegment();

	//ֻ����1, 4, 9���� 16��25 ����
	bool SetSegmentCount(int ns = 4);
	int GetCurrentPos() { return m_nCurrentPos; };
	bool SetCurrentPos(int nPos) {
		if (nPos >= 0 && nPos < m_nSegment)
		{
			m_nCurrentPos = nPos;
			return true;
		}
		else
			return false;
	}

	QString GetCurrentDeviceId() { return m_lsDeviceId[m_nCurrentPos]; };
	
	QStringList GetCurrentDeviceIds();

	void CloseAllStream();
public slots:
	void OpenStreamSlot(QString deviceId, int nPos, std::string url, QString szinfo = "", bool blAutoInc = false);
	void ResizeUI(QRect rt);
	//�ڲ��Ŀؼ�˳��
	void ChangeIndexSlot(int nIndex);
	void ShowFullSlot(int nIndex);
public :
	signals:
	void IndexChangeSignal(int nCurrentPos, QString deviceId);
	void ShowFullIndexSignal(QString deviceId);
	
private:
	Ui::WidgetVideoSegment ui;
	//��������
	int m_nSegment;
	//��ǰ����λ��
	int m_nCurrentPos;

	WidgetCam* m_widgetCams[M_MAX_SEG];
	QString m_lsDeviceId[M_MAX_SEG];
};
