#pragma once

#include <QWidget>

//#include "ui_MultiLargeScreenFixedWidget.h"
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

#include "WidgetOfflineMap.h"

#include "SvrResourceMng.h"
#include "HttpClientSvr.h"

#include "streamsdk.h"
#include <thread>
#include <string>

#define M_MAX_SEG (9)

class MultiLargeScreenFixedWidget : public QWidget
{
	Q_OBJECT

public:
	MultiLargeScreenFixedWidget(QWidget *parent = Q_NULLPTR);
	~MultiLargeScreenFixedWidget();

	bool Initialize();

	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
	//void GpsUpdate(double longitude, double latitude, QJsonArray json_arr, QString revsecond, QString revthird, double longitude1, double latitude1);
	//void GpsLocation(double longitude, double latitude);

	void OpenStream(std::vector<std::string> urls);

	void startVideo();
	void stopVideo();

private:
	Ui::MultiLargeScreenWidget ui;
	std::vector<std::string> m_urls;
	volatile bool m_blRunning;

	bool m_blResize;
	float m_fZoom;
	cvcam::Frame m_frame;
	cvcam::StreamManager m_manager;
	std::thread m_thread;

	WidgetCam* m_widgetCams[M_MAX_SEG];

	//资源管理和 其他需要http的共用一个httpclientsvr
	//resource manage
	SvrResourceMng * m_svrResourceMng;
	//httpclient svr
	HttpClientSvr * m_httpClientSvr;

	WidgetDomeVideo *m_widgetAlarm;

	WidgetOfflineMap *m_mapWidget;
	WidgetOfflineMap *m_mapLayoutWidget;
private:
	signals:
	int showYuvSignal1(cvcam::Frame frame);
	int showYuvSignal2(cvcam::Frame frame);
	int showYuvSignal3(cvcam::Frame frame);
	int showYuvSignal4(cvcam::Frame frame);
	int showYuvSignal5(cvcam::Frame frame);

	int showYuvSignal6(cvcam::Frame frame);

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

public slots:
	void btnOnlySegSlot();
	void btnFourSegSlot();
	void btnNineSegSlot();

	void getCoordinates(double lng, double lat);
	void getCoordinateRect(double lngs, double lats, double lnge, double late);
	//void returnWayPoints(const QString &r_waypoints);

private:
	QMap<int, GM_DEVICEGROUP_STRU> m_mapStrus;
	//fake data init
	void MultiLargeScreenFixedWidget::FakeData();
};

Q_DECLARE_METATYPE(cvcam::Frame)
