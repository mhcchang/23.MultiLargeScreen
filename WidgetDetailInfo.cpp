#include "WidgetDetailInfo.h"
#include <QFile>
#include <QTreeWidget>
#include <QListWidget>
#include <QDebug>
#include <QPropertyAnimation>

#include "MultiLargeScreenWidget.h"

extern MultiLargeScreenWidget* g_pMain;

WidgetDetailInfo::WidgetDetailInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//关闭自动删除 下次显示重建
	setAttribute(Qt::WA_DeleteOnClose);

	QFile qss(":/UI/qss/radioButton.qss");
	qss.open(QFile::ReadOnly);
	QByteArray ba = qss.readAll();
	qss.close();
	
	ui.btnPreset1->setStyleSheet(ba.data());
	ui.btnPreset2->setStyleSheet(ba.data());
	ui.btnPreset3->setStyleSheet(ba.data());
	ui.btnPreset4->setStyleSheet(ba.data());

	m_blPlayCloseAnimation = false;
	m_blPlayShowAnimation = false;
	m_blCloseAnimationState = false;
	m_blShowAnimationState = false;

	m_widgetVideo = new WidgetVideo(ui.widgetVideo, "");
	QVBoxLayout *verticalLayout = new QVBoxLayout(ui.widgetVideo);
	verticalLayout->setSpacing(0);
	verticalLayout->setContentsMargins(11, 11, 11, 11);
	verticalLayout->setContentsMargins(0, 0, 0, 0);
	verticalLayout->addWidget(m_widgetVideo);
	ui.lblFull->raise();

	connect(ui.btnClose, &QPushButton::clicked, this, &WidgetDetailInfo::SendClose);
	connect(ui.lblFull, &LabelClick::clicked, this, &WidgetDetailInfo::SendFullSlot);

	connect(ui.btnPreset1, &QPushButton::clicked, this, &WidgetDetailInfo::Preset1Slot);
	connect(ui.btnPreset2, &QPushButton::clicked, this, &WidgetDetailInfo::Preset2Slot);
	connect(ui.btnPreset3, &QPushButton::clicked, this, &WidgetDetailInfo::Preset3Slot);
	connect(ui.btnPreset4, &QPushButton::clicked, this, &WidgetDetailInfo::Preset4Slot);

	connect(this, &WidgetDetailInfo::ShowFullDomeSignle, g_pMain, &MultiLargeScreenWidget::ShowFullDomeSlot, Qt::UniqueConnection);

	ui.labelNext->setVisible(false);
}

WidgetDetailInfo::~WidgetDetailInfo()
{
}

void WidgetDetailInfo::SendClose()
{
	CloseStream();
	hide();

	emit Closed();
}

void WidgetDetailInfo::Resizefont(float fZoom)
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

		int zz = ft.pointSize() / 2.0 * m_fZoom;
		QString szm = lbl->text();

		lbl->setFont(ft);
	}

	QList<QPushButton *> lsBtn = this->findChildren<QPushButton *>();
	for (int i = 0; i < lsBtn.size(); i++)
	{
		QPushButton * btn = lsBtn.at(i);

		qDebug() << btn->objectName();

		ft = btn->font();

		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(ft.pointSize() / 2.0 * m_fZoom);

		btn->setFont(ft);
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

void WidgetDetailInfo::SetParamLabel(QString sceneId, QString id)
{
	m_sceneId = sceneId;
	m_labelid = id;

	ui.lblName->setText("");
	ui.lblType->setText("");
	ui.lblIp->setText("");
	ui.lblStatus->setText("");
	ui.lblContacts->setText("");
	ui.lblPhone->setText("");

	if (!g_pMain)
		return;
	STRU_DB_DEVICE *pp;
	STRU_DB_LABEL *ll ;
	//QString sceneId, QString lblId, STRU_DB_LABEL* pp

	//1. 先找label
	if (!g_pMain->FindLabelById(sceneId, id, ll))
	{
		return;
	}
	//2. 再找关联设备
	if (ll->deviceLinks.size() == 0)
	{
		return;
	}
	
	m_deviceid = ll->deviceLinks[0].id;
	if (!g_pMain->FindDeviceById(ll->deviceLinks[0].id, pp))
	{
		return;
	}

	m_presets.clear();
	if (g_pMain->GetSvrInterface()->GetPreset(m_deviceid, "0", m_presets))
	{
		ui.btnPreset1->setVisible(false);
		ui.btnPreset2->setVisible(false);
		ui.btnPreset3->setVisible(false);
		ui.btnPreset4->setVisible(false);

		ui.btnPreset1->setText("");
		ui.btnPreset2->setText("");
		ui.btnPreset3->setText("");
		ui.btnPreset4->setText("");

		int i = 0;
		for (STRU_DB_PRESET& it : m_presets)
		{
			switch (i)
			{
			case 0:
				ui.btnPreset1->setText(it.name);
				ui.btnPreset1->setVisible(true);
				break;
			case 1:
				ui.btnPreset2->setText(it.name);
				ui.btnPreset2->setVisible(true);
				break;
			case 2:
				ui.btnPreset3->setText(it.name);
				ui.btnPreset3->setVisible(true);
				break;
			case 3:
				ui.btnPreset4->setText(it.name);
				ui.btnPreset4->setVisible(true);
				break;
			}
			i++;
			if (i >= 4)
				break;
		}

		ui.widget_3->setVisible(true);
	}
	else
	{
		ui.widget_3->setVisible(false);
	}

	ui.lblName->setText(QString::fromUtf8(pp->name.toUtf8()));
	ui.lblType->setText(QString::fromUtf8(pp->type.toUtf8()));
	ui.lblIp->setText(pp->ip.toUtf8());
	ui.lblStatus->setText(QString::number(pp->status));
	ui.lblContacts->setText(QString::fromUtf8(pp->contactsName.toUtf8()));
	ui.lblPhone->setText(pp->phoneNo.toUtf8());
}

void WidgetDetailInfo::SetParamDevice(QString sceneId, QString deviceid)
{
	m_sceneId = sceneId;
	m_deviceid = deviceid;

	ui.lblName->setText("");
	ui.lblType->setText("");
	ui.lblIp->setText("");
	ui.lblStatus->setText("");
	ui.lblContacts->setText("");
	ui.lblPhone->setText("");

	if (!g_pMain)
		return;
	STRU_DB_DEVICE *pp;

	if (!g_pMain->FindDeviceById(deviceid, pp))
	{
		return;
	}

	m_presets.clear();
	if (g_pMain->GetSvrInterface()->GetPreset(m_deviceid, "0", m_presets))
	{
		ui.btnPreset1->setVisible(false);
		ui.btnPreset2->setVisible(false);
		ui.btnPreset3->setVisible(false);
		ui.btnPreset4->setVisible(false);

		ui.btnPreset1->setText("");
		ui.btnPreset2->setText("");
		ui.btnPreset3->setText("");
		ui.btnPreset4->setText("");

		int i = 0;
		for (STRU_DB_PRESET& it : m_presets)
		{
			switch (i)
			{
			case 0:
				ui.btnPreset1->setText(it.name);
				ui.btnPreset1->setVisible(true);
				break;
			case 1:
				ui.btnPreset2->setText(it.name);
				ui.btnPreset2->setVisible(true);
				break;
			case 2:
				ui.btnPreset3->setText(it.name);
				ui.btnPreset3->setVisible(true);
				break;
			case 3:
				ui.btnPreset4->setText(it.name);
				ui.btnPreset4->setVisible(true);
				break;
			}
			i++;
			if (i >= 4)
				break;
		}

		ui.widget_3->setVisible(true);
	}
	else
	{
		ui.widget_3->setVisible(false);
	}

	//ui.widget_4
	ui.lblName->setText(QString::fromUtf8(pp->name.toUtf8()));
	ui.lblType->setText(QString::fromUtf8(pp->type.toUtf8()));
	ui.lblIp->setText(pp->ip.toUtf8());
	ui.lblStatus->setText(QString::number(pp->status));
	ui.lblContacts->setText(QString::fromUtf8(pp->contactsName.toUtf8()));
	ui.lblPhone->setText(pp->phoneNo.toUtf8());
}

void WidgetDetailInfo::playCloseAnimation()
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

void WidgetDetailInfo::playShowAnimation()
{
	setMinimumSize(0, 0);
	QPropertyAnimation* showAnimation = new QPropertyAnimation(this, "geometry");
	showAnimation->setStartValue(QRect(geometry().x() / 4, geometry().y() + height(), width(), 0));
	showAnimation->setEndValue(geometry());
	showAnimation->setDuration(300);
	m_blShowAnimationState = true;
	showAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void WidgetDetailInfo::showEvent(QShowEvent *event)
{
	if (m_blPlayShowAnimation && m_blShowAnimationState == false) {
		playShowAnimation();
		event->ignore();
	}
	else {
		event->accept();
	}
}

void WidgetDetailInfo::closeEvent(QCloseEvent *event)
{
	if (m_blPlayCloseAnimation && m_blCloseAnimationState == false) {
		playCloseAnimation();
		event->ignore();
	}
	else 
	{
		event->accept();
	}
}

void WidgetDetailInfo::OpenStream(QString url)
{
	m_widgetVideo->OpenStream(url.toStdString());
}

void WidgetDetailInfo::CloseStream()
{
	m_widgetVideo->CloseStream();
}

void WidgetDetailInfo::SendFullSlot()
{
	emit ShowFullDomeSignle(m_deviceid);
}

void WidgetDetailInfo::Preset1Slot()
{
	for (auto &aa : m_presets)
	{
		QString sz = ui.btnPreset1->text();
		if (aa.name == sz)
		{
			if (g_pMain)
				g_pMain->GetSvrInterface()->GotoPreset(m_deviceid, aa.id);
		}
	}
}

void WidgetDetailInfo::Preset2Slot()
{
	for (auto &aa : m_presets)
	{
		if (aa.name == ui.btnPreset2->text())
		{
			if (g_pMain)
				g_pMain->GetSvrInterface()->GotoPreset(m_deviceid, aa.id);
		}
	}
}

void WidgetDetailInfo::Preset3Slot()
{
	for (auto &aa : m_presets)
	{
		if (aa.name == ui.btnPreset3->text())
		{
			if (g_pMain)
				g_pMain->GetSvrInterface()->GotoPreset(m_deviceid, aa.id);
		}
	}
}

void WidgetDetailInfo::Preset4Slot()
{
	for (auto &aa : m_presets)
	{
		if (aa.name == ui.btnPreset4->text())
		{
			if (g_pMain)
				g_pMain->GetSvrInterface()->GotoPreset(m_deviceid, aa.id);
		}
	}
}
