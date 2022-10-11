#include "WidgetIconLabel.h"
#include "MultiLargeScreenWidget.h"
extern MultiLargeScreenWidget * g_pMain;

WidgetIconLabel::WidgetIconLabel(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_blDrag = false;
	this->resize(65, 96);
	m_parent = parent;
	ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	ui.label->setAttribute(Qt::WA_TranslucentBackground, true);
	m_blShowDetail = false;
	
	m_widgetDetailInfo = new WidgetDetailInfo(m_parent);
	if (g_pMain)
	{
		m_widgetDetailInfo->Resizefont(g_pMain->GetScreenZoomRatio());
	}
}

WidgetIconLabel::~WidgetIconLabel()
{
}

void WidgetIconLabel::mousePressEvent(QMouseEvent * event)
{
	if (!m_blShowDetail)
	{
		m_widgetDetailInfo->SetParamLabel(m_sceneId, m_id);
		QRect rt = this->geometry();
		QRect rtparent = m_parent->geometry();
		int sdx = rt.x() - m_widgetDetailInfo->width() - 20;
		int sdy = rt.y();
		//左边界
		if (sdx < 20)
		{
			sdx = rt.x() + 20;
		}
		//右边界
		if (sdx + m_widgetDetailInfo->width() > rtparent.width() - 20)
		{
			sdx = rtparent.width() - 20 - m_widgetDetailInfo->width();
		}

		if (sdy < 20)
		{
			sdy += 20;
		}
		//下边界
		if (sdy + m_widgetDetailInfo->height() > rtparent.height() - 20)
		{
			sdy = rtparent.height() - 20 - m_widgetDetailInfo->height();
		}

		m_widgetDetailInfo->move(sdx, sdy);

		//m_widgetDetailInfo->move(rt.x() - m_widgetDetailInfo->width() - 20, rt.y());
		m_widgetDetailInfo->SetPlayCloseAnimation(false);
		m_widgetDetailInfo->SetPlayShowAnimation(false);
		

		connect(m_widgetDetailInfo, &WidgetDetailInfo::Closed, this, &WidgetIconLabel::CloseSubWin, Qt::UniqueConnection);

		m_widgetDetailInfo->show();

		if (g_pMain && m_lsDeviceid.size() > 0)
		{
			QString szurl = g_pMain->GetStreamUrl(m_lsDeviceid[0], "1");
			if (szurl != "")
			{
				m_widgetDetailInfo->OpenStream(szurl);
			}
		}
		
		emit ShowDetailSignle(m_id);
	}
	else
	{
		//m_widgetDetailInfo->CloseStream();
		m_widgetDetailInfo->hide();
	}
}

//void WidgetIconLabel::mousePressEvent(QMouseEvent *event)
//{
//	m_blDrag = false;
//
//	if (event->button() == Qt::LeftButton) {
//		relativePos = this->pos() - event->globalPos();
//	}
//}

void WidgetIconLabel::mouseMoveEvent(QMouseEvent *event)
{
	if (!m_blDrag)
		return;

	this->move(event->globalPos() + relativePos);
}

void WidgetIconLabel::mouseReleaseEvent(QMouseEvent *event)
{
	m_blDrag = false;
}

void WidgetIconLabel::SetInfo(QString sceneId, QString id, int flag, QList<QString> lsDeviceid)
{
	m_id = id;
	m_sceneId = sceneId;

	m_lsDeviceid = lsDeviceid;
	switch (flag)
	{
	case 0:
	case 1:
	default:
		ui.label->setPixmap(QPixmap(QString::fromLocal8Bit(":/UI/image/dome2x.png")));
		break;
	case 2:
		ui.label->setPixmap(QPixmap(QString::fromLocal8Bit(":/UI/image/fixcam2x.png")));
		break;
	}
}

void WidgetIconLabel::CloseSubWin()
{
	//if (m_widgetDetailInfo)
	//{
	//	m_widgetDetailInfo->hide();
	//}
	m_blShowDetail = false;
}
