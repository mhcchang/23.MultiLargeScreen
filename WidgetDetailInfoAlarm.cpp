#include "WidgetDetailInfoAlarm.h"
#include <QFile>
#include <QTreeWidget>
#include <QListWidget>
#include <QDebug>
#include <QPropertyAnimation>

#include "MultiLargeScreenWidget.h"

extern MultiLargeScreenWidget* g_pMain;

WidgetDetailInfoAlarm::WidgetDetailInfoAlarm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//关闭自动删除 下次显示重建
	setAttribute(Qt::WA_DeleteOnClose);
	hide();
	QFile qss(":/UI/qss/pushButton.qss");
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

	m_lbl = new QLabel(ui.widgetVideo);
	connect(this, &WidgetDetailInfoAlarm::ShowFullDomeSignle, g_pMain, &MultiLargeScreenWidget::ShowFullDomeSlot, Qt::UniqueConnection);
	connect(ui.btnClose, &QPushButton::clicked, this, &WidgetDetailInfoAlarm::SendClose);
	////url(:/UI/qss/radioButton.qss)
	//ui.radioButton->setStyleSheet("QRadioButton::indicator{ width:12px;height:12px; border-radius:6px;  }"
	//	"QRadioButton::indicator:checked{ background-color: #00FCFC; border: 3px solid #00FCFC; }"
	//	"QRadioButton::indicator:unchecked{ background-color: transparent; border: 3px solid white;}");
	//ui.radioButton_2->setStyleSheet("QRadioButton::indicator{ width:15px;height:15px;border-radius:3px }"
	//	"QRadioButton::indicator:checked{ background-color: green; border: 1px solid green; }"
	//	" QRadioButton::indicator:unchecked{ background-color: white; border: 1px solid white; }");
	////ui.radioButton_3->setStyleSheet("QRadioButton::indicator{ width:15px;height:15px;border-radius:3px }"
	////	"QRadioButton::indicator:checked{ background-color:green; border: 1px solid green; }"
	////	" QRadioButton::indicator:unchecked{ background-color: white; border: 1px solid white; }");
	//ui.radioButton_3->setStyleSheet("QRadioButton::indicator:checked{ color: rgb(0, 252, 252);border:0px #00FCFC; }"
	//	"QRadioButton::indicator:unchecked{ color: white;border:0px #00FCFC; }");

	//ui.radioButton_4->setStyleSheet("QRadioButton::indicator:checked{ color: rgb(0, 252, 252);border:0px #00FCFC; }");

	connect(ui.btnPreset1, &QPushButton::clicked, this, &WidgetDetailInfoAlarm::Preset1Slot);
	connect(ui.btnPreset2, &QPushButton::clicked, this, &WidgetDetailInfoAlarm::Preset2Slot);
	connect(ui.btnPreset3, &QPushButton::clicked, this, &WidgetDetailInfoAlarm::Preset3Slot);
	connect(ui.btnPreset4, &QPushButton::clicked, this, &WidgetDetailInfoAlarm::Preset4Slot);

	ui.labelNext->setVisible(false);

	ui.lblFull->setVisible(false);
}

WidgetDetailInfoAlarm::~WidgetDetailInfoAlarm()
{
}

void WidgetDetailInfoAlarm::SendClose()
{
	//CloseStream();
	hide();
	emit Closed(1);
}

void WidgetDetailInfoAlarm::Resizefont(float fZoom)
{
	m_fZoom = fZoom;

	QList<QLabel *> lsLbl = this->findChildren<QLabel *>();
	QFont ft;
	for (int i = 0; i < lsLbl.size(); i++)
	{
		QLabel * lbl = lsLbl.at(i);

		qDebug() << lbl->objectName();

		ft = lbl->font();
		int zz = ft.pointSize() / 2.0 * m_fZoom;
		QString szm = lbl->text();

		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(ft.pointSize() / 2.0 * m_fZoom);

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

void WidgetDetailInfoAlarm::InitInf(MqAlarmInfo* pInf)
{
	if (!static_cast<MqAlarmInfo*>(pInf))
		return;

	QPixmap img;
	QString szimg;
	if (pInf->msgData.rule.algoType == "Density")
		szimg = pInf->msgData.densityInfo.image;
	else
		szimg = pInf->msgData.detectInfo.detectImage;

	img.loadFromData(QByteArray::fromBase64(szimg.toUtf8()));
	m_lbl->setPixmap(img);

	QRect rt(img.width() / 2, 0, img.width(), img.height());
	m_lbl->setGeometry(rt);
	m_lbl->show();

	if (!g_pMain)
		return;
	STRU_DB_DEVICE* pp = nullptr;
	if (!g_pMain->FindDeviceById(pInf->msgData.sceneId, pp))
	{
		ui.lblName->setText("");
		ui.lblType->setText("");
		ui.lblIp->setText(pInf->msgData.deviceIp);
		ui.lblStatus->setText("");
		ui.lblContacts->setText("");
		ui.lblPhone->setText("");
		return;
	}

	//todo 关联到什么设备？ 全局？
	m_sceneId = pInf->msgData.sceneId;
	m_deviceid = pInf->msgData.sceneId;

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
			if (i >= 3)
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

void WidgetDetailInfoAlarm::playCloseAnimation()
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

void WidgetDetailInfoAlarm::playShowAnimation()
{
	setMinimumSize(0, 0);
	QPropertyAnimation* showAnimation = new QPropertyAnimation(this, "geometry");
	showAnimation->setStartValue(QRect(geometry().x() / 4, geometry().y() + height(), width(), 0));
	showAnimation->setEndValue(geometry());
	showAnimation->setDuration(300);
	m_blShowAnimationState = true;
	showAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void WidgetDetailInfoAlarm::showEvent(QShowEvent *event)
{
	if (m_blPlayShowAnimation && m_blShowAnimationState == false) {
		playShowAnimation();
		event->ignore();
	}
	else {
		event->accept();
	}
}

void WidgetDetailInfoAlarm::closeEvent(QCloseEvent *event)
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

void WidgetDetailInfoAlarm::OpenStream(QString url)
{
	m_widgetVideo->OpenStream(url.toStdString());
}

void WidgetDetailInfoAlarm::CloseStream()
{
	//m_widgetVideo->CloseStream();
}

void WidgetDetailInfoAlarm::Preset1Slot()
{
	for (auto &aa : m_presets)
	{
		if (aa.name == ui.btnPreset1->text())
		{
			if (g_pMain)
				g_pMain->GetSvrInterface()->GotoPreset(m_deviceid, aa.id);
		}
	}
}

void WidgetDetailInfoAlarm::Preset2Slot()
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

void WidgetDetailInfoAlarm::Preset3Slot()
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

void WidgetDetailInfoAlarm::Preset4Slot()
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
