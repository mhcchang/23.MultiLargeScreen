#include "WidgetCameraList.h"
#include <QTreeWidget>
#include <QListWidget>
#include <QDebug>
#include <QMessageBox>

#include "MultiLargeScreenWidget.h"

extern MultiLargeScreenWidget* g_pMain;
WidgetCameraList::WidgetCameraList(QWidget *parent)
	: QWidget(parent)
{
	m_blShowDetail = false;

	ui.setupUi(this);

	ui.listWidget->setStyleSheet("QListWidget{background-color:transparent; border:0px; margin:0px 0px 0px 0px;}"
		"QListWidget::Item{height:40px; border:0px; padding-left:14px; color: #FEFEFE;}"
		"QListWidget::Item:hover{color:rgb(0, 252, 252);}"
		"QListWidget::Item:selected{background-color:transparent; color:rgb(0, 252, 252);}");
	
	m_parent = parent;
	setAttribute(Qt::WA_DeleteOnClose);
	connect(ui.btnClose, &QPushButton::clicked, this, &WidgetCameraList::close);
	//connect(ui.listWidget, &QListWidget::itemDoubleClicked, this, &WidgetCameraList::DbClickSignal, Qt::AutoConnection);
	connect(ui.listWidget, &QListWidget::itemClicked, this, &WidgetCameraList::DbClickSignal, Qt::AutoConnection);
}

WidgetCameraList::~WidgetCameraList()
{
}

void WidgetCameraList::Resizefont(float fZoom)
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

void WidgetCameraList::Init(QString szId)
{
	if (szId == "")
		return;

	STRU_DB_LABEL* pp;
	if (!g_pMain->FindLabelById(g_pMain->GetScene(), szId, pp))
	{
		qDebug() << "FindLabelById error ";
		return;
	}

	for (STRU_DB_LABELLINKDEVICE& it : pp->deviceLinks)
	{
		QListWidgetItem *item = new QListWidgetItem(it.name);
		item->setData(Qt::UserRole, it.id);
		ui.listWidget->addItem(item);
	}
}

void WidgetCameraList::DbClickSignal(QListWidgetItem* item)
{
	QString id = item->data(Qt::UserRole).toString();

	m_szDeviceId = id;
	ShowDetail();
}

void WidgetCameraList::ShowDetail()
{
	if (!m_blShowDetail)
	{
		m_widgetDetailInfo = new WidgetDetailInfo(m_parent);

		if (!m_widgetDetailInfo)
		{
			QMessageBox::warning(NULL, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("显存用尽") );
			exit(0);
		}
		m_widgetDetailInfo->SetParamDevice(g_pMain->GetScene(), m_szDeviceId);
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

		if (g_pMain)
		{
			m_widgetDetailInfo->Resizefont(g_pMain->GetScreenZoomRatio());
		}
		connect(m_widgetDetailInfo, &WidgetDetailInfo::Closed, this, &WidgetDetailInfo::close, Qt::UniqueConnection);

		m_widgetDetailInfo->show();

		if (g_pMain)
		{
			QString szurl = g_pMain->GetStreamUrl(m_szDeviceId, "1");
			if (szurl != "")
			{
				m_widgetDetailInfo->OpenStream(szurl);
			}
		}
	}
	else
	{
		m_widgetDetailInfo->close();
	}
}

void WidgetCameraList::closeEvent(QCloseEvent* event)
{
	emit WinClosed();
	event->accept();
}
