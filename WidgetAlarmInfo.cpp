#include "WidgetAlarmInfo.h"
#include <QTreeWidget>
#include <QListWidget>
#include <QDebug>

#include <QPropertyAnimation>

#include "MultiLargeScreenWidget.h"
extern MultiLargeScreenWidget* g_pMain;

WidgetAlarmInfo::WidgetAlarmInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	hide();
	m_fZoom = 1.0f;
//	connect(this, &MultiLargeScreenFixedWidget::showYuvSignal5, ui.widgetRef, &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
	setAttribute(Qt::WA_DeleteOnClose);
	connect(ui.btnClose, &QPushButton::clicked, this, &WidgetAlarmInfo::SendClose);
	m_blPlayCloseAnimation = false;
	m_blPlayShowAnimation = false;
	m_blCloseAnimationState = false;
	m_blShowAnimationState = false;
}

WidgetAlarmInfo::~WidgetAlarmInfo()
{
}

void WidgetAlarmInfo::SendClose()
{
	hide();
	emit Closed(0);
}

void WidgetAlarmInfo::InitInf(MqAlarmInfo* pInf)
{
	if (!static_cast<MqAlarmInfo*>(pInf))
		return;

	if (!g_pMain)
		return;

	ui.lblAlarmType->setText(QString::fromUtf8(pInf->msgData.rule.ruleName.toUtf8()));
	QString szlocation;

	float lng;
	float lat;
	if (pInf->msgData.rule.algoType == "Density")
	{
		lat = pInf->msgData.densityInfo.latitude;
		lng = pInf->msgData.densityInfo.longitude;
	}
	else
	{
		lat = pInf->msgData.detectInfo.latitude;
		lng = pInf->msgData.detectInfo.longitude;
	}
	
	if (lng > 0)
	{
		if (lat > 0)
			szlocation = QString::fromUtf8("E %1, N %2").arg(lng).arg(lat);
		else
			szlocation = QString::fromUtf8("E %1, S %2").arg(lng).arg(lat);
	}
	else
	{
		if (lat > 0)
			szlocation = QString::fromUtf8("W %1, N %2").arg(lng).arg(lat);
		else
			szlocation = QString::fromUtf8("W %1, S %2").arg(lng).arg(lat);
	}
	
	ui.lblAddress->setText(szlocation);
	ui.lblTime->setText(QString::fromUtf8(pInf->msgData.alertDateTime.toUtf8()));
	ui.lblName->setText(QString::fromUtf8(pInf->msgData.deviceIp.toUtf8()));
	ui.lblTitle->setText(QString::fromUtf8(pInf->msgData.rule.ruleDescription.toUtf8()));

	STRU_DB_DEVICE* pp = nullptr;

	emit AlarmInfoSignal(pInf->msgData.rule.ruleDescription);
	//todo ？？？？ 设备id如何与sceneid对应？？
	if (!g_pMain->FindDeviceById(pInf->msgData.sceneId, pp))
		return;
	ui.lblPhone->setText(pp->phoneNo.toUtf8());
}

void WidgetAlarmInfo::Resizefont(float fZoom)
{
	m_fZoom = fZoom;

	QList<QLabel *> lsLbl = this->findChildren<QLabel *>();
	QFont ft;
	for (int i = 0; i < lsLbl.size(); i++)
	{
		QLabel * lbl = lsLbl.at(i);

		qDebug() << lbl->objectName();

		ft = lbl->font();

		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(ft.pointSize() / 2.0 * m_fZoom);

		lbl->setFont(ft);
	}

	QList<QTreeWidget *> lsTree = this->findChildren<QTreeWidget *>();
	for (int i = 0; i < lsTree.size(); i++)
	{
		QTreeWidget * tr = lsTree.at(i);
		qDebug() << tr->objectName() << " parent: " << tr->parent()->objectName();
		ft = tr->font();
		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(ft.pointSize() / 2.0 * m_fZoom);
		tr->setFont(ft);
	}

	QList<QListWidget *> lsLs = this->findChildren<QListWidget *>();
	for (int i = 0; i < lsLs.size(); i++)
	{
		QListWidget * ls = lsLs.at(i);

		qDebug() << ls->objectName() << " parent: " << ls->parent()->objectName();
		ft = ls->font();
		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(ft.pointSize() / 2.0 * m_fZoom);

		ls->setFont(ft);
	}
}

void WidgetAlarmInfo::playCloseAnimation()
{
	setMinimumSize(0, 0);
	QPropertyAnimation* closeAnimation = new QPropertyAnimation(this, "geometry");
	closeAnimation->setStartValue(geometry());
	closeAnimation->setEndValue(QRect(geometry().x() / 4, geometry().y() + height(), width(), 0));
	closeAnimation->setDuration(300);
	m_blCloseAnimationState = true;
	connect(closeAnimation, SIGNAL(finished()), this, SLOT(close()));
	closeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void WidgetAlarmInfo::playShowAnimation()
{
	setMinimumSize(0, 0);
	QPropertyAnimation* showAnimation = new QPropertyAnimation(this, "geometry");
	showAnimation->setStartValue(QRect(geometry().x() / 4, geometry().y() + height(), width(), 0));
	showAnimation->setEndValue(geometry());
	showAnimation->setDuration(300);
	m_blShowAnimationState = true;
	//connect(showAnimation, SIGNAL(finished()), this, SLOT(close()));
	showAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void WidgetAlarmInfo::showEvent(QShowEvent *event)
{
	if (m_blPlayShowAnimation && m_blShowAnimationState == false)
	{
		playShowAnimation();
		event->ignore();
	}
	else {
		event->accept();
	}
}

void WidgetAlarmInfo::closeEvent(QCloseEvent *event)
{
	if (m_blPlayCloseAnimation && m_blCloseAnimationState == false)
	{
		playCloseAnimation();
		event->ignore();
	}
	else 
	{
		event->accept();
	}
}
