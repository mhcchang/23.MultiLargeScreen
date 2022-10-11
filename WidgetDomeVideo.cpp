#include "WidgetDomeVideo.h"
#include "MultiLargeScreenWidget.h"
#include <QScrollBar>
extern MultiLargeScreenWidget * g_pMain;
WidgetDomeVideo::WidgetDomeVideo(QWidget *parent)
	: m_strImg(""), WidgetVideo(parent, "")
{
	ui.setupUi(this);

	this->setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	//this->setWindowModality(Qt::ApplicationModal);
	this->hide();

	this->setAttribute(Qt::WA_DeleteOnClose);

	ui.listWidget->setStyleSheet("QListWidget{background-color:transparent; border:0px; margin:0px 0px 0px 0px;}"
		"QListWidget::Item{height:40px; border:0px; padding-left:14px; color: #FEFEFE;}"
		"QListWidget::Item:hover{color:rgb(0, 252, 252);}"
		"QListWidget::Item:selected{background-color:transparent; color:rgb(0, 252, 252);}" );

	ui.listWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{ width:8px; background:#15808C; margin:0px,0px,0px,0px; padding-top:0px; padding-bottom:0px;}"
		"QScrollBar::handle:vertical{ width:8px; background:#00B3BE; border-radius:4px; min-height:20;}"
		"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical { background:#15808C; border-radius:4px;}");

	ui.listWidget->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{ height:8px; background:#15808C; margin:0px,0px,0px,0px; padding-left:0px; padding-right:0px;}"
		"QScrollBar::handle:horizontal{ width:8px; background:#00B3BE; border-radius:4px; min-width:20; }"
		"QScrollBar::add-page : horizontal, QScrollBar::sub-page : horizontal{ background:#15808C; border-radius:4px; }");


	m_widgetSlider = new WidgetSlider(ui.widget_6);
	m_parent = parent;

	m_blHasPtz = false;
	connect(ui.btnClose, &QPushButton::clicked, this, &QWidget::close, Qt::UniqueConnection);

	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickTop, this, &WidgetDomeVideo::CtrlPanelTopSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickTopRight, this, &WidgetDomeVideo::CtrlPanelTopRightSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickRight, this, &WidgetDomeVideo::CtrlPanelRightSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickBottomRight, this, &WidgetDomeVideo::CtrlPanelBottomRightSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickBottom, this, &WidgetDomeVideo::CtrlPanelBottomSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickBottomLeft, this, &WidgetDomeVideo::CtrlPanelBottomLeftSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickLeft, this, &WidgetDomeVideo::CtrlPanelLeftSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickTopLeft, this, &WidgetDomeVideo::CtrlPanelTopLeftSlot, Qt::QueuedConnection);

	connect(ui.lblControlPanel, &WidgetControlPanel::OnStop, this, &WidgetDomeVideo::CtrlPanelStop, Qt::QueuedConnection);

	connect(m_widgetSlider, &WidgetSlider::btnBigSignal, this, &WidgetDomeVideo::ZoominSlot, Qt::QueuedConnection);
	connect(m_widgetSlider, &WidgetSlider::btnSmallSignal, this, &WidgetDomeVideo::ZoomoutSlot, Qt::QueuedConnection);
	connect(m_widgetSlider, &WidgetSlider::btnStopSignal, this, &WidgetDomeVideo::CtrlPanelStop, Qt::QueuedConnection);

	connect(ui.listWidget, &QListWidget::itemClicked, this, &WidgetDomeVideo::PresetChangeSlot, Qt::AutoConnection);
}

WidgetDomeVideo::~WidgetDomeVideo()
{
}

void WidgetDomeVideo::mouseDoubleClickEvent(QMouseEvent* event)
{
	return QWidget::mouseDoubleClickEvent(event);
}

void WidgetDomeVideo::closeEvent(QCloseEvent *event)
{
	//if (this->windowType() & (Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
	//	== (Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint))
	//{
	//	this->setWindowFlags(m_winType);
	//	this->setParent(m_parent);
	//	this->setGeometry(m_rtDefault);
	//	this->move(0, 0);
	//	this->show();

	//	event->ignore();
	//}
	//else
	QWidget::closeEvent(event);
}

void WidgetDomeVideo::enterEvent(QEvent *event)
{

}

void WidgetDomeVideo::leaveEvent(QEvent *event)
{
}

void WidgetDomeVideo::resizeEvent(QResizeEvent *event)
{
	//QRect rtCtl = ui.widget_CtlPnl->geometry();
	//ui.lblControlPanel->ResizeUI(QSize(qMin(rtCtl.width(), rtCtl.height()), qMin(rtCtl.width(), rtCtl.height())));
	//ui.lblControlPanel->move(10, 0);

	//ui.listWidget->setGeometry(qMin(rtCtl.width(), rtCtl.height()) + 10, 0, rtCtl.width() - qMin(rtCtl.width(), rtCtl.height()) - 22, rtCtl.height());

	return GLYuvWidget::resizeEvent(event);
}

void WidgetDomeVideo::PresetChangeSlot(QListWidgetItem *item)
{
	if (!item)
		return;

	QString preid = item->data(Qt::UserRole).toString();
	if (g_pMain->GetSvrInterface()->GotoPreset(m_szDeviceId, preid))
	{

	}
	else
		qDebug() << "goto preset error";
}

void WidgetDomeVideo::CtrlPanelStop()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 0, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::ZoominSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 1, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::ZoomoutSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 2, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::CtrlPanelTopSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 21, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::CtrlPanelTopRightSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 26, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::CtrlPanelRightSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 24, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::CtrlPanelBottomRightSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 28, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::CtrlPanelBottomSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 22, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::CtrlPanelBottomLeftSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 27, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::CtrlPanelLeftSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 23, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::CtrlPanelTopLeftSlot()
{
	g_pMain->CtrlPanelToSvr(m_szDeviceId, 25, m_widgetSlider->GetValue());
}

void WidgetDomeVideo::SetInfo(QString deviceid, bool blHasPtz)
{
	m_szDeviceId = deviceid;
	m_blHasPtz = blHasPtz;
}
