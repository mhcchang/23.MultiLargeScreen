#include "WidgetTargetList.h"
#include <QScrollBar>
#include <QFile>
#include <QDebug>

#include <QTreeWidget>
#include <QListWidget>
#include <QHeaderView>

WidgetTargetList::WidgetTargetList(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_treeWidgetTarget = new QTreeWidget();
	m_treeWidgetCamera = new QTreeWidget();
	m_treeWidgetCamera->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_treeWidgetCamera->header()->setStretchLastSection(false);
	m_treeWidgetTarget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_treeWidgetTarget->header()->setStretchLastSection(false);
	//ui.treeWidget->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerItem);
	////ui.treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
	//ui.treeWidget->header()->setStretchLastSection(false);
	//ui.treeWidget->setAutoScroll(true);

	//ui.treeWidget->horizontalScrollBar()->setEnabled(true);
	//ui.treeWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QFile qss(":/UI/qss/treeWidget.qss");
	qss.open(QFile::ReadOnly);
	QByteArray ba = qss.readAll();
	qss.close();
	m_treeWidgetCamera->setStyleSheet(ba.data());
	m_treeWidgetTarget->setStyleSheet(ba.data());

	//ui.treeWidget->setStyleSheet("QTreeWidget{background-color:transparent; border:0px; margin:0px 0px 0px 0px;}"
	//	"QTreeWidget::Item{height:40px; border:0px; padding-left:14px; color: #FEFEFE;}"
	//	"QTreeWidget::Item:hover{color:rgb(0, 252, 252);}"
	//	"QTreeWidget::Item:selected{background-color:transparent; color:rgb(0, 252, 252);}"
	//    "QTreeView::branch:has-children:!has-siblings:closed,"
	//	"QTreeView::branch:closed:has-children:has-siblings{border-image: none; image: url(:/UI/button/expand2xL.png);}"
	//	"QTreeView::branch:open:has-children:!has-siblings,"
	//	"QTreeView::branch:open:has-children:has-siblings{border-image: none; image: url(:/UI/button/expand2x.png);}");
	qss.setFileName(":/UI/qss/verticalScrollBar.qss");
	qss.open(QFile::ReadOnly);
	ba = qss.readAll();
	qss.close();
	m_treeWidgetCamera->verticalScrollBar()->setStyleSheet(ba.data());
	m_treeWidgetTarget->verticalScrollBar()->setStyleSheet(ba.data());

	qss.setFileName(":/UI/qss/horizontalScrollBar.qss");
	qss.open(QFile::ReadOnly);
	ba = qss.readAll();
	qss.close();
	m_treeWidgetCamera->verticalScrollBar()->setStyleSheet(ba.data());
	m_treeWidgetTarget->verticalScrollBar()->setStyleSheet(ba.data());

	m_treeWidgetCamera->header()->setVisible(false);
	m_treeWidgetTarget->header()->setVisible(false);

	//ui.treeWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{ width:8px; background:#15808C; margin:0px,0px,0px,0px; padding-top:0px; padding-bottom:0px;}"
	//	"QScrollBar::handle:vertical{ width:8px; background:#00B3BE; border-radius:4px; min-height:20;}"
	//	"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical { background:#15808C; border-radius:4px;}");

	//ui.treeWidget->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{ height:8px; background:#15808C; margin:0px,0px,0px,0px; padding-left:0px; padding-right:0px;}"
	//	"QScrollBar::handle:horizontal{ width:8px; background:#00B3BE; border-radius:4px; min-width:20;}"
	//	"QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal { background:#15808C; border-radius:4px;}");
	connect(m_treeWidgetCamera, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemChangedSlot(QTreeWidgetItem*, int)), Qt::UniqueConnection);
	connect(m_treeWidgetTarget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemChangedSlot(QTreeWidgetItem*, int)), Qt::UniqueConnection);

	connect(ui.btnCamera, &LabelClick::clicked, this, &WidgetTargetList::showCameraListSlot, Qt::UniqueConnection);
	connect(ui.btnImportance, &LabelClick::clicked, this, &WidgetTargetList::showTargetListSlot, Qt::UniqueConnection);

	ui.btnCamera->CheckEnable(true);
	ui.btnImportance->CheckEnable(true);

	//connect(ui.treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(startVideoNum(QTreeWidgetItem*, int)), Qt::UniqueConnection);
}

WidgetTargetList::~WidgetTargetList()
{
}

void AddListInfo(QStringList* header, QTreeWidget *treeWidget, QMap<int, GM_DEVICEGROUP_STRU> mapStrus)
{
	treeWidget->clear();
	QStringList * tmpheader = (QStringList*)header;
	if (header == NULL)
	{
		tmpheader = new QStringList();
		*tmpheader << QString::fromUtf8("摄像机组");
	}

	treeWidget->setHeaderLabels(*tmpheader);

	QTreeWidgetItem *imageItemParent = NULL;
	QMap<int, GM_DEVICEGROUP_STRU>::iterator it;
	for (it = mapStrus.begin(); it != mapStrus.end(); it++)
	{
		PGM_DEVICEGROUP_STRU deviceGroup = &(it.value());
		if (deviceGroup->nodeType == 0)
		{
			QString strInfo = QString::fromUtf8(deviceGroup->nodeName.toUtf8());
			QTreeWidgetItem *imageItem1 = new QTreeWidgetItem(treeWidget, QStringList(strInfo));
			//显示根节点图标
			if (deviceGroup->devicetype == 100)
			{
				imageItem1->setIcon(0, QIcon(":/UI/icon/cameraicon2x.png"));
			}
			else 
			{
				
			}
			imageItemParent = imageItem1;
			if (deviceGroup->roleType == 0)
			{
				imageItem1->setData(0, Qt::UserRole, deviceGroup->cameraid);
				imageItem1->setCheckState(0, Qt::Unchecked);
			}
			else
				imageItem1->setData(0, Qt::UserRole, deviceGroup->deviceautoId);

			::printf("LINE %d, nodeType[%d] devicetype[%d] roleType[%d], cameraid[%s], deviceautoId", __LINE__, deviceGroup->nodeType, deviceGroup->devicetype, deviceGroup->roleType, 
				deviceGroup->cameraid.toStdString().c_str(), deviceGroup->deviceautoId);
		}
		else if (deviceGroup->nodeType == 1)
		{  //checkbox
			if (deviceGroup->deviceautoParent != -1)
			{
				//获取父节点
				imageItemParent = GetTreeViewItem(treeWidget, mapStrus, deviceGroup->deviceautoParent);
				if (imageItemParent == NULL)
					continue;

				QString strInfo = QString::fromUtf8(deviceGroup->nodeName.toUtf8());
				QTreeWidgetItem *imageItem1_1 = new QTreeWidgetItem(imageItemParent, QStringList(strInfo));
				imageItem1_1->setIcon(0, QIcon(":/UI/icon/camtarget.png"));
				
				if (deviceGroup->roleType == 0)
				{
					imageItem1_1->setData(0, Qt::UserRole, deviceGroup->cameraid);
					imageItem1_1->setCheckState(0, Qt::Unchecked);
				}
				else
					imageItem1_1->setData(0, Qt::UserRole, deviceGroup->deviceautoId);
				imageItemParent->addChild(imageItem1_1);
			}
		}
		else if (deviceGroup->nodeType == 9 || deviceGroup->nodeType == 1000)
		{ //icon
			if (deviceGroup->deviceautoParent != -1)
			{
				imageItemParent = GetTreeViewItem(treeWidget, mapStrus, deviceGroup->deviceautoParent);
				if (imageItemParent == NULL)
					continue;

				QString strInfo = QString::fromUtf8(deviceGroup->nodeName.toUtf8());
				QTreeWidgetItem *imageItem1_2 = new QTreeWidgetItem(imageItemParent, QStringList(strInfo));

				imageItem1_2->setIcon(0, QIcon(":/UI/icon/pantarget.png"));
				//imageItem1_2->setCheckState(0, Qt::Unchecked);
				if (deviceGroup->roleType == 0)
				{
					imageItem1_2->setData(0, Qt::UserRole, deviceGroup->cameraid);
					imageItem1_2->setCheckState(0, Qt::Unchecked);
				}
				else
					imageItem1_2->setData(0, Qt::UserRole, deviceGroup->deviceautoId);
				if (deviceGroup->devicetype == 0)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/camtarget.png"));
				else if (deviceGroup->devicetype == 2)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/cam.png"));
				else if (deviceGroup->devicetype == 1)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/pan.png"));
				else
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/r4.png"));
				imageItemParent->addChild(imageItem1_2);
			}
		}
	}

	treeWidget->setIconSize(QSize(24, 24));
	treeWidget->expandAll();
}

void AddListInfoList(QStringList* header, QTreeWidget *treeWidget, QList<GM_DEVICEGROUP_STRU> vStrus)
{
	treeWidget->clear();
	QStringList * tmpheader = (QStringList*)header;
	if (header == NULL)
	{
		tmpheader = new QStringList();
		*tmpheader << QString::fromUtf8("摄像机组");
	}

	treeWidget->setHeaderLabels(*tmpheader);

	QTreeWidgetItem *imageItemParent = NULL;
	//QMap<QString, GM_DEVICEGROUP_STRU>::iterator it;
	for (GM_DEVICEGROUP_STRU& it : vStrus)
	{
		PGM_DEVICEGROUP_STRU deviceGroup = &it;
		if (deviceGroup->nodeType == 0)
		{
			QString strInfo = QString::fromUtf8(deviceGroup->nodeName.toUtf8());
			QTreeWidgetItem *imageItem1 = new QTreeWidgetItem(treeWidget, QStringList(strInfo));
			//显示根节点图标
			if (deviceGroup->devicetype == 100)
			{
				imageItem1->setIcon(0, QIcon(":/UI/icon/camgroup.png"));
			}
			else 
			{
				imageItem1->setCheckState(0, Qt::Unchecked);
			}
			imageItemParent = imageItem1;
			imageItem1->setData(0, Qt::UserRole, deviceGroup->nodeId);
		}
		else if (deviceGroup->nodeType == 1)
		{  //checkbox
			if (deviceGroup->parentId != "")
			{
				//获取父节点
				imageItemParent = GetTreeViewItemList(treeWidget, vStrus, deviceGroup->parentId);
				if (imageItemParent == NULL)
					continue;

				QString strInfo = QString::fromUtf8(deviceGroup->nodeName.toUtf8());
				QTreeWidgetItem *imageItem1_1 = new QTreeWidgetItem(imageItemParent, QStringList(strInfo));
				imageItem1_1->setCheckState(0, Qt::Unchecked);
				imageItem1_1->setData(0, Qt::UserRole, deviceGroup->nodeId);
				imageItemParent->addChild(imageItem1_1);
			}
		}
		else if (deviceGroup->nodeType == 9 || deviceGroup->nodeType == 1000)
		{ //icon
			if (deviceGroup->parentId != "")
			{
				imageItemParent = GetTreeViewItemList(treeWidget, vStrus, deviceGroup->parentId);
				if (imageItemParent == NULL)
					continue;

				QString strInfo = QString::fromUtf8(deviceGroup->nodeName.toUtf8());
				QTreeWidgetItem *imageItem1_2 = new QTreeWidgetItem(imageItemParent, QStringList(strInfo));

				imageItem1_2->setIcon(0, QIcon(":/UI/icon/pantarget.png"));
				//imageItem1_2->setCheckState(0, Qt::Unchecked);
				imageItem1_2->setData(0, Qt::UserRole, deviceGroup->nodeId);

				if (deviceGroup->devicetype == 1)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/camtarget.png"));
				else if (deviceGroup->devicetype == 2)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/cam.png"));
				else if (deviceGroup->devicetype == 3)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/pan.png"));
				else
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/r4.png"));
				imageItemParent->addChild(imageItem1_2);
			}
		}
	}

	treeWidget->setIconSize(QSize(24, 24));
	treeWidget->expandAll();
}

QTreeWidgetItem* GetTreeViewItem(QTreeWidget *treeWidget, QMap<int, GM_DEVICEGROUP_STRU> mapStrus, int parentId)
{
	PGM_DEVICEGROUP_STRU tmp = NULL;
	QMap<int, GM_DEVICEGROUP_STRU>::Iterator it = mapStrus.find(parentId);
	if (it != mapStrus.end())
		tmp = &it.value();
	//QMap<int, GM_DEVICEGROUP_STRU>::Iterator it;
	//for (it = mapStrus.begin(); it != mapStrus.end(); it++)
	//{
	//	if (it->deviceautoParent == parentId)
	//	{
	//		tmp = &it.value();
	//		break;
	//	}
	//}

	if (tmp == NULL)
		return NULL;
	bool bl = tmp->roleType == 0 ? true : false;
	QTreeWidgetItem * pItem = FindTreeViewItem(treeWidget->invisibleRootItem(), tmp, bl);
	return pItem;
}

QTreeWidgetItem* GetTreeViewItemList(QTreeWidget *treeWidget, QList<GM_DEVICEGROUP_STRU> vStrus, QString szId)
{
	PGM_DEVICEGROUP_STRU tmp = NULL;
	
	for (GM_DEVICEGROUP_STRU& it : vStrus)
	{
		PGM_DEVICEGROUP_STRU tmp1 = &it;
		if (tmp1->nodeId == szId)
		{
			*tmp = *tmp1;
			break;
		}
	}

	if (tmp == NULL)
		return NULL;
	QTreeWidgetItem * pItem = FindTreeViewItem(treeWidget->invisibleRootItem(), tmp, false);
	return pItem;
}

bool isTopItem(QTreeWidgetItem* item)
{
	if (!item) 
		return false;
	if (!item->parent())
		return true;
	return false;
}

void WidgetTargetList::setChildCheckState(QTreeWidgetItem *item, Qt::CheckState cs)
{
	if (!item) 
		return;

	for (int i = 0; i < item->childCount(); i++)
	{
		QTreeWidgetItem* child = item->child(i);
		child->setCheckState(0, cs);

		if (child->checkState(0) == Qt::CheckState::Checked)
		{
			emit showLabelSignal(true, child->data(0, Qt::UserRole).toString());
		}
		else if (child->checkState(0) == Qt::CheckState::Unchecked)
		{
			emit showLabelSignal(false, child->data(0, Qt::UserRole).toString());
		}

		setChildCheckState(child, cs);
	}
}

void setParentCheckState(QTreeWidgetItem *item)
{
	if (!item) 
		return;	
	QTreeWidgetItem* parentItem = item->parent();
	if (parentItem == NULL) return;

	int nChildCount = parentItem->childCount();
	int nSelected = 0;
	int nPartSelected = 0;
	for (int i = 0; i < nChildCount; i++) 
	{
		if (parentItem->child(i)->checkState(0) == Qt::Checked) 
		{
			nSelected++;
		}
		else if (parentItem->child(i)->checkState(0) == Qt::PartiallyChecked) 
		{
			nPartSelected++;
		}
	}
	if ((nSelected > 0 && nSelected < nChildCount) || nPartSelected > 0) 
	{
		parentItem->setCheckState(0, Qt::PartiallyChecked);
	}
	else if (nSelected == nChildCount)
	{
		parentItem->setCheckState(0, Qt::Checked);
	}
	else 
	{
		parentItem->setCheckState(0, Qt::Unchecked);
	}
	setParentCheckState(parentItem);
}

QTreeWidgetItem* FindTreeViewItem(QTreeWidgetItem *treeWidgetItem, GM_DEVICEGROUP_STRU* mapStrus, bool blString)
{
	QTreeWidgetItemIterator it(treeWidgetItem);

	while (*it)
	{
		QTreeWidgetItem *pItem = (*it);
		if (blString)
		{
			QString qvs = "";
			if (pItem != NULL)
			{
				qvs = pItem->data(0, Qt::UserRole).toString();
			}

			if (qvs == mapStrus->cameraid)
			{
				return pItem;
			}
		}
		else
		{
			int qv = -1;
			if (pItem != NULL)
			{
				qv = pItem->data(0, Qt::UserRole).toInt();
			}

			if (qv == mapStrus->nodeId)
			{
				return pItem;
			}
		}
		it++;
	}
	return NULL;
}

void WidgetTargetList::showCameraListSlot()
{
	QRect rt = ui.widget->geometry();
	m_treeWidgetTarget->setVisible(false);
	m_treeWidgetTarget->setParent(nullptr);
	m_treeWidgetCamera->setParent(ui.widget);
	m_treeWidgetCamera->setGeometry(0, 0, rt.width(), rt.height());
	m_treeWidgetCamera->setVisible(true);

	ui.btnCamera->SetCheckStatus(true);
	ui.btnImportance->SetCheckStatus(false);
	//ui.btnImportance->setStyleSheet("color: rgb(222, 223, 225)");
	//ui.btnCamera->setStyleSheet("color: rgb(0, 252, 252)");
	//emit showCameraListSignal();
}

void WidgetTargetList::showTargetListSlot()
{
	QRect rt = ui.widget->geometry();
	m_treeWidgetCamera->setVisible(false);
	m_treeWidgetTarget->setParent(ui.widget);
	m_treeWidgetTarget->setGeometry(0, 0, rt.width(), rt.height());
	m_treeWidgetCamera->setParent(nullptr);
	m_treeWidgetTarget->setVisible(true);

	ui.btnCamera->SetCheckStatus(false);
	ui.btnImportance->SetCheckStatus(true);
	//ui.btnImportance->setStyleSheet("color: rgb(0, 252, 252)");
	//ui.btnCamera->setStyleSheet("color: rgb(222, 223, 225)");
	//emit showTargetListSignal();
}

void WidgetTargetList::itemChangedSlot(QTreeWidgetItem* item, int column)
{
	disconnect(m_treeWidgetTarget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemChangedSlot(QTreeWidgetItem*, int)));
	disconnect(m_treeWidgetCamera, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemChangedSlot(QTreeWidgetItem*, int)));
	setChildCheckState(item, item->checkState(0));
	setParentCheckState(item);

	if (item->checkState(0) == Qt::CheckState::Checked)
	{
		emit showLabelSignal(true, item->data(0, Qt::UserRole).toString());
	}
	else if (item->checkState(0) == Qt::CheckState::Unchecked)
	{
		emit showLabelSignal(false, item->data(0, Qt::UserRole).toString());
	}

	connect(m_treeWidgetTarget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemChangedSlot(QTreeWidgetItem*, int)), Qt::UniqueConnection);
	connect(m_treeWidgetCamera, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemChangedSlot(QTreeWidgetItem*, int)), Qt::UniqueConnection);
}

void WidgetTargetList::showEvent(QShowEvent * event)
{
	QWidget::showEvent(event);
	QRect rt = ui.widget->geometry();
	m_treeWidgetCamera->setGeometry(0, 0, rt.width(), rt.height());
	m_treeWidgetTarget->setGeometry(0, 0, rt.width(), rt.height());
}

void WidgetTargetList::Resizefont(float fZoom)
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
	for (int i = 0; i < lsTree.size(); i++)
	{
		QListWidget * ls = lsLs.at(i);

		qDebug() << ls->objectName() << " parent: " << ls->parent()->objectName();
		ft = ls->font();
		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(ft.pointSize() / 2.0 * m_fZoom);

		ls->setFont(ft);
	}


}

void WidgetTargetList::AddList(QStringList* header, QTreeWidget *treeWidget, QMap<int, GM_DEVICEGROUP_STRU> mapStrus)
{
	treeWidget->clear();
	QStringList * tmpheader = (QStringList*)header;
	if (header == NULL)
	{
		tmpheader = new QStringList();
		*tmpheader << QString::fromUtf8("摄像机组");
	}

	treeWidget->setHeaderLabels(*tmpheader);

	QTreeWidgetItem *imageItemParent = NULL;
	QMap<int, GM_DEVICEGROUP_STRU>::iterator it;
	for (it = mapStrus.begin(); it != mapStrus.end(); it++)
	{
		PGM_DEVICEGROUP_STRU deviceGroup = &(it.value());
		if (deviceGroup->nodeType == 0)
		{
			QString strInfo = QString::fromUtf8(deviceGroup->nodeName.toUtf8());
			QTreeWidgetItem *imageItem1 = new QTreeWidgetItem(treeWidget, QStringList(strInfo));
			//显示根节点图标
			if (deviceGroup->devicetype == 100)
			{
				imageItem1->setIcon(0, QIcon(":/UI/icon/cameraicon2x.png"));
			}
			else
			{

			}
			imageItemParent = imageItem1;
			if (deviceGroup->roleType == 0)
			{
				imageItem1->setData(0, Qt::UserRole, deviceGroup->cameraid);
				imageItem1->setCheckState(0, Qt::Unchecked);
			}
			else
				imageItem1->setData(0, Qt::UserRole, deviceGroup->deviceautoId);

			::printf("LINE %d, nodeType[%d] devicetype[%d] roleType[%d], cameraid[%s], deviceautoId", __LINE__, deviceGroup->nodeType, deviceGroup->devicetype, deviceGroup->roleType,
				deviceGroup->cameraid.toStdString().c_str(), deviceGroup->deviceautoId);
		}
		else if (deviceGroup->nodeType == 1)
		{  //checkbox
			if (deviceGroup->deviceautoParent != -1)
			{
				//获取父节点
				imageItemParent = GetTreeViewItem(treeWidget, mapStrus, deviceGroup->deviceautoParent);
				if (imageItemParent == NULL)
					continue;

				QString strInfo = QString::fromUtf8(deviceGroup->nodeName.toUtf8());
				QTreeWidgetItem *imageItem1_1 = new QTreeWidgetItem(imageItemParent, QStringList(strInfo));
				imageItem1_1->setIcon(0, QIcon(":/UI/icon/camtarget.png"));

				if (deviceGroup->roleType == 0)
				{
					imageItem1_1->setData(0, Qt::UserRole, deviceGroup->cameraid);
					imageItem1_1->setCheckState(0, Qt::Unchecked);
				}
				else
					imageItem1_1->setData(0, Qt::UserRole, deviceGroup->deviceautoId);
				imageItemParent->addChild(imageItem1_1);
			}
		}
		else if (deviceGroup->nodeType == 9 || deviceGroup->nodeType == 1000)
		{ //icon
			if (deviceGroup->deviceautoParent != -1)
			{
				imageItemParent = GetTreeViewItem(treeWidget, mapStrus, deviceGroup->deviceautoParent);
				if (imageItemParent == NULL)
					continue;

				QString strInfo = QString::fromUtf8(deviceGroup->nodeName.toUtf8());
				QTreeWidgetItem *imageItem1_2 = new QTreeWidgetItem(imageItemParent, QStringList(strInfo));

				imageItem1_2->setIcon(0, QIcon(":/UI/icon/pantarget.png"));
				//imageItem1_2->setCheckState(0, Qt::Unchecked);
				if (deviceGroup->roleType == 0)
				{
					imageItem1_2->setData(0, Qt::UserRole, deviceGroup->cameraid);
					imageItem1_2->setCheckState(0, Qt::Unchecked);
				}
				else
					imageItem1_2->setData(0, Qt::UserRole, deviceGroup->deviceautoId);
				if (deviceGroup->devicetype == 0)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/camtarget.png"));
				else if (deviceGroup->devicetype == 2)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/cam.png"));
				else if (deviceGroup->devicetype == 1)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/pan.png"));
				else
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/r4.png"));
				imageItemParent->addChild(imageItem1_2);
			}
		}
	}

	treeWidget->setIconSize(QSize(24, 24));
	treeWidget->expandAll();

}
