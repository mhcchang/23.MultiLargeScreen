#include "LabelAlarmIcon.h"

#include "MultiLargeScreenWidget.h"

extern MultiLargeScreenWidget * g_pMain;
LabelAlarmIcon::LabelAlarmIcon(QWidget *parent)
	: LabelClick(parent)
{
	this->hide();

	this->setAttribute(Qt::WA_DeleteOnClose);
	m_parent = parent;
	m_pInf = nullptr;
	m_blShowDetail = false;
	m_tm.start();

	m_widgetDetailInfo = nullptr;
	m_widgetAlarmInfo = nullptr;

	m_widgetDetailInfo = new WidgetDetailInfoAlarm(m_parent);
	m_widgetAlarmInfo = new WidgetAlarmInfo(m_parent);

	if (g_pMain)
	{
		//m_widgetDetailInfo->Resizefont(g_pMain->GetScreenZoomRatio());
		//m_widgetAlarmInfo->Resizefont(g_pMain->GetScreenZoomRatio());
	}
}

LabelAlarmIcon::~LabelAlarmIcon()
{
}

void LabelAlarmIcon::Init(int flag, QString lblId, MqAlarmInfo * pInf, bool blShow)
{
	switch (flag)
	{
	case 0:
	default:
		this->setPixmap(QPixmap(QString::fromLocal8Bit(":/UI/image/redalarm2x.png")));
		break;
	case 1:
		this->setPixmap(QPixmap(QString::fromLocal8Bit(":/UI/image/orangealarm2x.png")));
		break;
	case 2:
		this->setPixmap(QPixmap(QString::fromLocal8Bit(":/UI/image/yellowalarm2x.png")));
		break;
	case 3:
		this->setPixmap(QPixmap(QString::fromLocal8Bit(":/UI/image/cyanalarm2x.png")));
		break;
	}

	if (pInf)
	{
		if (m_pInf)
			delete m_pInf;
		m_pInf = new MqAlarmInfo(*pInf);
	}

	m_szLabelId = lblId;
	uint64_t tm = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	qDebug() << "init:" << lblId << " tm " << tm << " this " << this << " m_szLabelId :" << m_szLabelId;

	if (blShow)
		this->show();
	else
		this->hide();
}

void LabelAlarmIcon::GetLabelInfo(QString & lblid, QString &lat, QString & lng)
{
	lblid = m_szLabelId;
	if (!m_pInf)
		return;
	if (m_pInf->msgData.rule.algoType == "Density")
	{
		lng = QString::number(m_pInf->msgData.densityInfo.longitude);
		lat = QString::number(m_pInf->msgData.densityInfo.latitude);
	}
	else
	{
		lng = QString::number(m_pInf->msgData.detectInfo.longitude);
		lat = QString::number(m_pInf->msgData.detectInfo.latitude);
	}
}

void LabelAlarmIcon::ShowLabelPlaceSlot(QString lblid, QString strx, QString stry)
{
	uint64_t tm = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	qDebug() << "ShowLabelPlaceSlot:" << lblid << " tm " << tm << " this " << this << " m_szLabelId :" << m_szLabelId;
	if (lblid != m_szLabelId)
	{
		//todo log
		return;
	}

	int x = strx.toInt();
	int y = stry.toInt();
	this->move(x, y);
	QRect rtp;
	if (this->parent())
		rtp = ((QWidget*)(this->parent()))->geometry();
	else
		return;
	rtp.setX(0);
	if (rtp.contains(x, y))
	{
		if (m_blShowDetail && m_widgetDetailInfo)
			m_widgetDetailInfo->move(rtp.x() - m_widgetDetailInfo->width() - 20, rtp.y());
		if (m_blShowDetail && m_widgetAlarmInfo)
			m_widgetAlarmInfo->move(rtp.x() + rtp.width() + 20, rtp.y());

		this->show();
	}
	else
	{
		this->hide();
		if (!m_blShowDetail)
		{
			if (m_widgetDetailInfo)
			{
				m_widgetDetailInfo->hide();
			}
			if (m_widgetAlarmInfo)
				m_widgetAlarmInfo->hide();
		}

		m_blShowDetail = false;
	}
}

uint64_t LabelAlarmIcon::GetLiveMillSecond()
{
	return m_tm.elapsed();
}

void LabelAlarmIcon::mousePressEvent(QMouseEvent* event)
{
	if (!m_blShowDetail)
	{
		if (!m_pInf)
		{
			MqAlarmInfo* pinf = static_cast<MqAlarmInfo*>(m_pInf);
		}
		QRect rt = this->geometry();
		QRect rtparent = m_parent->geometry();

		//m_widgetDetailInfo->move(rt.x() - m_widgetDetailInfo->width() - 20, rt.y());
		m_widgetDetailInfo->InitInf(m_pInf);
		
		m_widgetDetailInfo->SetPlayCloseAnimation(false);
		m_widgetDetailInfo->SetPlayShowAnimation(false);

		m_widgetAlarmInfo->SetPlayCloseAnimation(false);
		m_widgetAlarmInfo->SetPlayShowAnimation(false);
		m_widgetAlarmInfo->InitInf(m_pInf);

		connect(m_widgetDetailInfo, &WidgetDetailInfoAlarm::Closed, this, &LabelAlarmIcon::CloseSubWin, Qt::UniqueConnection);
		connect(m_widgetAlarmInfo, &WidgetAlarmInfo::Closed, this, &LabelAlarmIcon::CloseSubWin, Qt::UniqueConnection);

		if (g_pMain)
			connect(m_widgetAlarmInfo, &WidgetAlarmInfo::AlarmInfoSignal, g_pMain, &MultiLargeScreenWidget::AlarmInfoStatusSlot, Qt::UniqueConnection);
		
		int sdx = rt.x() - m_widgetDetailInfo->width() - 20;
		int sdy = rt.y();
		int sax = rt.x() + rt.width() + 20;
		int say = rt.y();

		//左边界
		if (sdx < 20)
		{
			sdx = rt.x() + 20;
			sax = rt.x() + rt.width() + m_widgetDetailInfo->width() + 20;
		}
		//右边界
		if (sax + m_widgetAlarmInfo->width() >  rtparent.width() - 20)
		{
			sdx = rtparent.width() - 20 - m_widgetDetailInfo->width();
			sax = rtparent.width() - 20;
		}

		if (sdy < 20)
		{
			sdy += 20;
			say = sdy;
		}
		//下边界
		if (say + m_widgetDetailInfo->height() > rtparent.height() - 20)
		{
			say = rtparent.height() - 20 - m_widgetDetailInfo->height();
			sdy = say;
		}

		m_widgetDetailInfo->move(sdx, sdy);
		m_widgetAlarmInfo->move(sax, say);

		m_widgetDetailInfo->show();
		m_widgetAlarmInfo->show();
	}
	else
	{
		if (m_widgetDetailInfo)
		{
			m_widgetDetailInfo->hide();
		}
		if (m_widgetAlarmInfo)
			m_widgetAlarmInfo->hide();
	}

	m_blShowDetail = !m_blShowDetail;
}

void LabelAlarmIcon::CloseSubWin(int i)
{
	//if (i == 1)
	//{
	//	if (m_blExistsDetailWin)
	//		m_widgetDetailInfo->SetPlayShowAnimation(false);

	//}
	//else
	//{
	//	if (m_blExistsAlarmWin)
	//		m_widgetAlarmInfo->SetPlayShowAnimation(false);
	//}
	if (i == 2)
	{
		if (m_widgetAlarmInfo)
		{
			m_widgetAlarmInfo->close();
		}
		if (m_widgetDetailInfo)
		{
			m_widgetDetailInfo->close();
		}
	}
	if (!m_blShowDetail)
	{
		if (m_widgetAlarmInfo)
		{
			m_widgetAlarmInfo->hide();
			//m_widgetAlarmInfo = nullptr;
		}
		if (m_widgetDetailInfo)
		{
			m_widgetDetailInfo->hide();
		}
	}
	else
	{
		if (m_widgetAlarmInfo)
		{
			m_widgetAlarmInfo->hide();
		}
		if (m_widgetDetailInfo)
		{
			m_widgetDetailInfo->hide();
		}
	}

	m_blShowDetail = false;
}

void LabelAlarmIcon::closeEvent(QCloseEvent* event)
{
	CloseSubWin(2);
	event->accept();
}
