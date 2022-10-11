#include "LabelRefIcon.h"

#include "MultiLargeScreenWidget.h"

extern MultiLargeScreenWidget * g_pMain;

LabelRefIcon::LabelRefIcon(QWidget *parent)
	: LabelClick(parent)
{
	this->hide();

	this->setAttribute(Qt::WA_DeleteOnClose);
	m_parent = parent;

	m_tm.start();
}

LabelRefIcon::~LabelRefIcon()
{
}

void LabelRefIcon::Init(int flag, STRU_DB_DEVICE inf, QString lblId, bool blShow)
{
	switch (flag)
	{
	case 0:
	default:
		this->setPixmap(QPixmap(QString::fromLocal8Bit(":/UI/icon/reficon2x.png")));
		break;
	}

	m_szLabelId = lblId;
	uint64_t tm = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_inf = inf;
	qDebug() << "init:" << lblId << " tm " << tm << " this " << this << " m_szLabelId :" << m_szLabelId;

	if (blShow)
		this->show();
	else
		this->hide();
}

void LabelRefIcon::GetLabelInfo(QString & lblid, QString &lat, QString & lng)
{
	lblid = m_szLabelId;
	lat = QString::number(m_inf.latitude);
	lng = QString::number(m_inf.longtitude);
}

void LabelRefIcon::ShowLabelPlaceSlot(QString lblid, QString strx, QString stry)
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


		this->show();
	}
	else
	{
		this->hide();
	}
}

uint64_t LabelRefIcon::GetLiveMillSecond()
{
	return m_tm.elapsed();
}

void LabelRefIcon::mousePressEvent(QMouseEvent* event)
{
	//if (!m_blShowDetail)
	//{
	//	QRect rt = this->geometry();
	//	QRect rtparent = m_parent->geometry();

	//	connect(m_widgetDetailInfo, &WidgetDetailInfoAlarm::Closed, this, &LabelAlarmIcon::CloseSubWin, Qt::UniqueConnection);
	//	connect(m_widgetAlarmInfo, &WidgetAlarmInfo::Closed, this, &LabelAlarmIcon::CloseSubWin, Qt::UniqueConnection);

	//	if (g_pMain)
	//		connect(m_widgetAlarmInfo, &WidgetAlarmInfo::AlarmInfoSignal, g_pMain, &MultiLargeScreenWidget::AlarmInfoStatusSlot, Qt::UniqueConnection);
	//	
	//	int sdx = rt.x() - m_widgetDetailInfo->width() - 20;
	//	int sdy = rt.y();
	//	int sax = rt.x() + rt.width() + 20;
	//	int say = rt.y();

	//	//左边界
	//	if (sdx < 20)
	//	{
	//		sdx = rt.x() + 20;
	//		sax = rt.x() + rt.width() + m_widgetDetailInfo->width() + 20;
	//	}
	//	//右边界
	//	if (sax + m_widgetAlarmInfo->width() >  rtparent.width() - 20)
	//	{
	//		sdx = rtparent.width() - 20 - m_widgetDetailInfo->width();
	//		sax = rtparent.width() - 20;
	//	}

	//	if (sdy < 20)
	//	{
	//		sdy += 20;
	//		say = sdy;
	//	}
	//	//下边界
	//	if (say + m_widgetDetailInfo->height() > rtparent.height() - 20)
	//	{
	//		say = rtparent.height() - 20 - m_widgetDetailInfo->height();
	//		sdy = say;
	//	}

	//	m_widgetDetailInfo->move(sdx, sdy);
	//	m_widgetAlarmInfo->move(sax, say);

	//	m_widgetDetailInfo->show();
	//	m_widgetAlarmInfo->show();
	//}
	//else
	//{
	//	if (m_widgetDetailInfo)
	//	{
	//		m_widgetDetailInfo->hide();
	//	}
	//	if (m_widgetAlarmInfo)
	//		m_widgetAlarmInfo->hide();
	//}

	//m_blShowDetail = !m_blShowDetail;
}

void LabelRefIcon::CloseSubWin(int i)
{
}

void LabelRefIcon::closeEvent(QCloseEvent* event)
{
	CloseSubWin(2);
	event->accept();
}
