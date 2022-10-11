#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_WidgetTargetList.h"
#include "LabelClick.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>

typedef struct gm_devicegroup_stru {
	int deviceautoId;
	int deviceautoParent;
	QString nodeId;
	QString nodeName;
	QString nodeContent;
	QString parentId;
	int nodeType;
	QString cameraid;
	int deviceParentAutoId;
	int devicetype;
	int roleType;
	QString url;
	QString urlSub;
} GM_DEVICEGROUP_STRU, *PGM_DEVICEGROUP_STRU;

QTreeWidgetItem* FindTreeViewItem(QTreeWidgetItem *treeWidgetItem, GM_DEVICEGROUP_STRU* mapStrus, bool blString);
QTreeWidgetItem* GetTreeViewItem(QTreeWidget *treeWidget, QMap<int, GM_DEVICEGROUP_STRU> mapStrus, int parentId);
QTreeWidgetItem* GetTreeViewItemList(QTreeWidget *treeWidget, QList<GM_DEVICEGROUP_STRU> vStrus, QString szId);

void setParentCheckState(QTreeWidgetItem *item);
bool isTopItem(QTreeWidgetItem* item);
void AddListInfo(QStringList* header, QTreeWidget *treeWidget, QMap<int, GM_DEVICEGROUP_STRU> mapStrus);
void AddListInfoList(QStringList* header, QTreeWidget *treeWidget, QList<GM_DEVICEGROUP_STRU> vStrus);

class WidgetTargetList : public QWidget
{
	Q_OBJECT

public:
	WidgetTargetList(QWidget *parent = Q_NULLPTR);
	~WidgetTargetList();
	Ui::WidgetTargetInfo ui;
	void Resizefont(float fZoom);
	void setChildCheckState(QTreeWidgetItem *item, Qt::CheckState cs);

	void AddList(QStringList* header, QTreeWidget *treeWidget, QMap<int, GM_DEVICEGROUP_STRU> mapStrus);
	
	QTreeWidget * m_treeWidgetTarget;
	QTreeWidget * m_treeWidgetCamera;

	void showEvent(QShowEvent * event) Q_DECL_OVERRIDE;

public slots:
	void itemChangedSlot(QTreeWidgetItem* item, int column);
	void showCameraListSlot();
	void showTargetListSlot();
signals:
	void showLabelSignal(bool blShow, QString nodeid /*struDeviceInfo.nodeid*/);

	//void showTargetListSignal();
	//void showCameraListSignal();
private:
	float m_fZoom;


};
