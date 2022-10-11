#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QTimer>

#include "ui_MultiLargeScreenWidget.h"
#include "WidgetAlarmInfo.h"
#include "WidgetCam.h"
#include "WidgetCameraList.h"
#include "WidgetDetailInfo.h"
#include "WidgetTargetList.h"
#include "WidgetRefList.h"
#include "WidgetAlarmInfo.h"
#include "WidgetIconLabel.h"
#include "WidgetDomeVideo.h"
#include "WidgetChangeUser.h"
#include "WidgetLabel.h"
#include "LabelAlarmIcon.h"
#include "LabelClick.h"
#include "WidgetSlider.h"
#include "WidgetLogin.h"
#include "WidgetOfflineMap.h"
#include "WidgetRefList.h"
#include "SvrResourceMng.h"
#include "HttpClientSvr.h"
#include "SvrInterface.h"
#include "LabelRefIcon.h"

#include "Config.h"
#include "ConsumerMsg.h"

#include "MqAlarmSvr.h"
#include <thread>
#include <string>

#include "WidgetVideoSegment.h"

#include "PubDefs.h"

class MultiLargeScreenWidget : public QWidget
{
	Q_OBJECT

public:
	MultiLargeScreenWidget(QWidget *parent = Q_NULLPTR);
	~MultiLargeScreenWidget();

	bool Initialize();

	float GetScreenZoomRatio() { return m_fZoom; };

	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
	void changeEvent(QEvent* e) Q_DECL_OVERRIDE;
	void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
	QString GetStreamUrl(QString deviceId, QString channel);
	SvrInterface* GetSvrInterface() { return m_svrInterface; };
	void CtrlPanelToSvr(QString deviceId, int flag, float fspeed);
	QString GetScene() { return m_szSceneId; };

public:
	void logout();
	void changeUser();
	
public:
	bool FindDeviceById(QString deviceId, STRU_DB_DEVICE* &pp);
	bool FindOrganizationById(QString orgId, STRU_DB_ORGANIZATION* &pp);
	bool FindLabelById(QString sceneId, QString lblId, STRU_DB_LABEL* &pp);

private:
	//void GpsUpdate(double longitude, double latitude, QJsonArray json_arr, QString revsecond, QString revthird, double longitude1, double latitude1);
	//void GpsLocation(double longitude, double latitude);

	//void OpenStream(std::vector<std::string> urls);

	void ResizeUi();

	void Resizefont();
private:
	Ui::MultiLargeScreenWidget ui;
	//std::vector<std::string> m_urls;
	volatile bool m_blRunning;

	bool m_blResize;
	float m_fZoom;
	//cvcam::Frame m_frame;
	//cvcam::StreamManager m_manager;
	std::thread m_thread;

	int m_nNowCam;
	int m_nCurrentMaxCam;
	//分屏播放控件
	WidgetVideoSegment* m_widgetVideoSegment;

	//WidgetCam* m_widgetCams[M_MAX_SEG];
	//WidgetDomeVideo * m_widgetCams[M_MAX_SEG];

	//资源管理和 其他需要http的共用一个httpclientsvr
	//resource manage
	SvrResourceMng * m_svrResourceMng;
	//httpclient svr
	HttpClientSvr * m_httpClientSvr;
	SvrInterface* m_svrInterface;

	WidgetDomeVideo * m_widgetAlarm;
	//地图
	WidgetOfflineMap * m_mapWidget;
	//计算地图上的gps对应的屏幕坐标  需要调用js 异步返回
	QString m_strCalLng, m_strCalLat;
	QMap<int, void*> m_mapOverlay;
	QString m_strScreenPixelX, m_strScreenPixelY;
	
	WidgetOfflineMap * m_mapLayoutWidget;
	//
	WidgetChangeUser * m_changeUserWidget;
	WidgetLogin * m_loginWidget;
	//QLabel * m_lblRef;
	WidgetSlider* m_widgetSlider;

private:
	signals:
	//int showYuvSignal1(cvcam::Frame frame);
	//int showYuvSignal2(cvcam::Frame frame);
	//int showYuvSignal3(cvcam::Frame frame);
	//int showYuvSignal4(cvcam::Frame frame);
	//int showYuvSignal5(cvcam::Frame frame);

	//int showYuvSignal6(cvcam::Frame frame);

	//int showYuvSignal1(uchar *ptry, uchar *ptru, uchar *ptrv, int width, int height);
	//int showYuvSignal2(uchar *ptry, uchar *ptru, uchar *ptrv, int width, int height);
	//int showYuvSignal3(uchar *ptry, uchar *ptru, uchar *ptrv, int width, int height);
	//int showYuvSignal4(uchar *ptry, uchar *ptru, uchar *ptrv, int width, int height);
	//int showYuvSignalStitch(uchar *ptry, uchar *ptru, uchar *ptrv, int width, int height);

private:
#if 0
	bool isTopItem(QTreeWidgetItem* item);
	void setChildCheckState(QTreeWidgetItem *item, Qt::CheckState cs);
	void setParentCheckState(QTreeWidgetItem *item);
	QTreeWidgetItem* FindtreeViewItem(QTreeWidgetItem *treeWidgetItem, GM_DEVICEGROUP_STRU* mapStrus);
	QTreeWidgetItem* GettreeViewItem(QTreeWidget *treeWidget, QMap<int, PGM_DEVICEGROUP_STRU> mapStrus, int nId);
	void AddDevicesTotreeView(QTreeWidget *treeWidget, const QStringList *header, QMap<int, PGM_DEVICEGROUP_STRU> mapStrus);
#endif
private:
	void InitUI();
	bool ConnectMq();
	void DisconnectMq();

	//保存界面状态
	bool saveUiStatus();
	bool loadUiStatus();
public slots:
	void btnOnlySegSlot();
	void btnFourSegSlot();
	void btnNineSegSlot();
	void btnChangeUserSlot();

	void btnGlobalSlot();
	void btnLabelsSlot();

	//与输入的lng，lat 数组一一对应  地图缩放 拖动操作后 标签需要移动到相应的位置 显示隐藏...
	void MapReturnPixelsSlot(QString strx, QString stry);
	void getCoordinates(double lng, double lat);
	//暂缓
	void getCoordinateRect(double lngs, double lats, double lnge, double late);

	//void returnWayPoints(const QString &r_waypoints);
	void treeCamItemChangedSlot(QTreeWidgetItem* item, int column);
	void treeCamItemDoubleClickedSlot(QTreeWidgetItem* item, int column);

	void onTimeout();
	//双击场景列表 切换场景
	void ChangeSceneSlot(QString sceneId);
	//双击后显示标签
	void ShowLabelSlot(bool blShow, QString lblId);
	//从mq获取报警信息 生成一个alarmlabel 不显示 调用js 等待js异步返回后再显示
	void AlarmInfSlot(QString sceneId, qint64 timestamp, qreal lng, qreal lat, MqAlarmInfo inf);
	
	void AlarmInfoStatusSlot(QString szAlarmDesc);

	void ShowTargetListSlot();
	void ShowCameraListSlot();

	//全屏播放 前景
	void ShowFullDomeSlot(QString deviceid);

	void slot_keyPressed(QKeyEvent * keyEvent);
private slots:
	//控制盘
	void CtrlPanelTopSlot();
	void CtrlPanelTopRightSlot();
	void CtrlPanelRightSlot();
	void CtrlPanelBottomRightSlot();
	void CtrlPanelBottomSlot();
	void CtrlPanelBottomLeftSlot();
	void CtrlPanelLeftSlot();
	void CtrlPanelTopLeftSlot();
	void ZoominSlot();
	void ZoomoutSlot();

	void CtrlPanelStop();
	void PresetChangeSlot(QListWidgetItem *item);

	//控制盘是否可控制
	void CtrlPanelEnabledSlot(int npos, QString deviceid);

private:
	void CheckandCleanAlarmLabelCount();
	void ShowRefLabel();

	//void SaveLocalUIStatus(QString szScene);
	//void LoadLocalUIStatus(QString szScene);

private:
	QMap<QString, int> m_mapParent;
	QMap<int, GM_DEVICEGROUP_STRU> m_mapCameraGroup;
	//树形结构的父节点对应autoid
	QMap<QString, int> m_mapParentTarget;
	QMap<int, GM_DEVICEGROUP_STRU> m_mapTarget;
	//QMap<QString, int> m_mapParentTarget;
	QMap<int, GM_DEVICEGROUP_STRU> m_mapCameras;
	QMap<QString, int> m_mapParentCamera;
	QList<GM_DEVICEGROUP_STRU> m_vCameras;
	//fake data init
	void TransDataCameraGroup();

	void TransDataTargetChild(QString parentid, int &i);
	void TransDataCameraChild(QString parentid, int &i);

	//flag == 0 data flag== 1 camera
	void TransDataTarget();
	void TransDataCameras();
	
	QTimer timer;

	//login user
	QString m_strUser;

	Config * m_config;
	//全景相机列表 及下拉标签
	WidgetRefList* m_widgetGlobal;
	QLabel* m_lblExpandGlobal;

	WidgetTargetList* m_widgetTarget;
	WidgetTargetList* m_widgetTarget2;
	QLabel* m_lblExpandLabels;

	//std::vector<STRU_DB_ORGANIZATION> m_vtOrg;
	//std::vector<STRU_DB_DEVICE> m_vtDevices;

	//需要将此转换为m_mapCameraGroup的内容
	//key 为orguuid + '/' + deviceuuid, value为 deviceuuid
	std::map<QString, QString> m_mapOrgLinkDevice;

	//通过deviceid找label
	//key 为deviceid + '/' + labelid
	std::map<QString, QString> m_mapDeviceLinkLabel;
	//organization uuid
	std::map<QString, STRU_DB_ORGANIZATION> m_mapOrgs;
	QList<STRU_DB_ORGANIZATION> m_vCameraGroup;
	//device uuid
	std::map<QString, STRU_DB_DEVICE> m_mapDevices;
	//场景id
	QList<STRU_DB_DEVICE> m_lsScene;
	//场景id 对应标签
	std::vector<STRU_DB_LABELTYPE> m_vLabelGroup;
	std::map<QString, STRU_DB_LABEL> m_mapLabels;
	QList<STRU_DB_LABEL> m_vLabel;
	std::map<QString, STRU_DB_SCENE> m_mapScenes;

	std::map<uint64_t, LabelAlarmIcon*> m_mapAlarmLbls;
	//消息队列
	ConsumerMsg m_consumer;
	//消息队列报警消息显示... 在MqAlarmSvr里面处理
	//void ConsumerReceiveMessageCallbackImp(std::string exchange, std::string key, std::string tags, std::string msg);
	MqAlarmSvr* m_svrAlarm;

	//当前场景id
	QString m_szSceneId;

	//alarm
	QList<LabelAlarmIcon *> m_lsLblAlarm;
	int m_lsLblAlarmPos;

	QList<LabelRefIcon *> m_lsLblRef;

	//ui 界面
	UiLocalStatus m_stLocalStatus;
};

Q_DECLARE_METATYPE(cvcam::Frame)
