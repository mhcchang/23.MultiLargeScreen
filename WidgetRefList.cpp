#include "WidgetRefList.h"
#include <QDebug>
#include <QScrollBar>
#include <QPainter>
#include <QTreeWidget>
#include <QListWidget>
#include <QLabel>

WidgetRefList::WidgetRefList(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.listWidget->setStyleSheet("QListWidget{background-color:transparent; border:0px; margin:0px 0px 0px 0px;}"
		"QListWidget::Item{height:40px; border:0px; padding-left:14px; color: #FEFEFE;}"
		"QListWidget::Item:hover{color:rgb(0, 252, 252);}"
		"QListWidget::Item:selected{background-color:transparent; color:rgb(0, 252, 252);}");

	//QString str = ui.listWidget->styleSheet();
	//qDebug() << str;

	ui.listWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{ width:8px; background:#15808C; margin:0px,0px,0px,0px; padding-top:0px; padding-bottom:0px;}"
		"QScrollBar::handle:vertical{ width:8px; background:#00B3BE; border-radius:4px; min-height:20;}"
		"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical { background:#15808C; border-radius:4px;}");

	connect(ui.listWidget, &QListWidget::itemDoubleClicked, this, &WidgetRefList::DbClickSignal, Qt::AutoConnection);
	m_nPos = -1;
}

WidgetRefList::~WidgetRefList()
{
}

//void WidgetRefList::paintEvent(QPaintEvent *)
//{
//	QStyleOption opt;
//	opt.init(this);
//	QPainter p(this);
//	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//}

void WidgetRefList::AddDevice(STRU_DB_DEVICE &deviceInf)
{
	m_lsDevice.push_back(deviceInf);
}

void WidgetRefList::Resizefont(float fZoom)
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

void WidgetRefList::SetScene(int npos)
{
	m_nPos = npos;
	if (m_nPos >= 0 && m_nPos < m_lsDevice.size())
	{
		ui.listWidget->setCurrentRow(m_nPos);
		QListWidgetItem *item = ui.listWidget->currentItem();
		item->setSelected(true);
	}
}

void WidgetRefList::DbClickSignal(QListWidgetItem *item)
{
	QString sz = item->text();

	for (STRU_DB_DEVICE & dd : m_lsDevice)
	{
		if (dd.name == sz)
		{
			emit ChangeScene(dd.deviceId);
			return;
		}
	}

	qDebug() << " scene== device not found " << sz;
}
