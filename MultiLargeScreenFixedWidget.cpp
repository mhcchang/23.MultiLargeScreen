#include "MultiLargeScreenFixedWidget.h"
#include <QUrl>
#include <QWebChannel>
#include <QWebEngineView>
#include <QJsonArray>
#include <QDesktopWidget>
#include <QScreen>
#include "GLYuvWidget.h"
#include <QDir>

#include <QList>

extern Log4Qt::Logger * g_logger;

MultiLargeScreenFixedWidget::MultiLargeScreenFixedWidget(QWidget *parent)
	: QWidget(parent)
{
	m_widgetAlarm = new WidgetDomeVideo();

	m_blResize = false;

	ui.setupUi(this);
	qRegisterMetaType<cvcam::Frame>("cvcam::Frame");

	QScreen *screen1 = qApp->primaryScreen();
	qreal dpiVal = screen1->logicalDotsPerInch();
	int currentScreenWidth = QApplication::desktop()->width();
	int currentScreenHeight = QApplication::desktop()->height();

	qreal dpiVal2 = screen1->physicalDotsPerInch();
	qreal dpiVal3 = screen1->devicePixelRatio();

	m_fZoom = 1.0f;
	switch (int(dpiVal))
	{
	case 96:
	default:
		break;
	case 120:
		m_fZoom = 1.25f;
		break;
	case 144:
		m_fZoom = 1.5f;
		break;
	case 168:
		m_fZoom = 1.75f;
		break;
	case 192:
		m_fZoom = 2.0f;
		break;
	case 216:
		m_fZoom = 2.25f;
		break;
	case 240:
		m_fZoom = 2.5f;
		break;
	case 288:
		m_fZoom = 3.0f;
		break;
	case 336:
		m_fZoom = 3.5f;
		break;
	}

	connect(ui.btnOnlySeg, &QPushButton::clicked, this, &MultiLargeScreenFixedWidget::btnOnlySegSlot, Qt::UniqueConnection);
	connect(ui.btnFourSeg, &QPushButton::clicked, this, &MultiLargeScreenFixedWidget::btnFourSegSlot, Qt::UniqueConnection);
	connect(ui.btnNineSeg, &QPushButton::clicked, this, &MultiLargeScreenFixedWidget::btnNineSegSlot, Qt::UniqueConnection);

	WidgetDetailInfo* w1 = new WidgetDetailInfo(ui.widget_map);
	QRect rt1 = ui.widget_map->geometry();
	//w1->setParent(ui.widgetMap);
	w1->setGeometry(40, 40, w1->size().width(), w1->size().height());
	
	w1->show();

	WidgetAlarmInfo* w2 = new WidgetAlarmInfo(ui.widget_map);
	w2->setParent(ui.widget_map);
	w2->setGeometry( 420,  60, w2->size().width(), w2->size().height());
	w2->show();

	QRect rt2 = ui.widgetRef->geometry();
	WidgetTargetList* wt1 = new WidgetTargetList(ui.widgetRef);
	
	wt1->setGeometry( 300, 120, wt1->size().width(), wt1->size().height());
	wt1->raise();
	wt1->show();

	////////////////////
	FakeData();
	wt1->AddListInfo(nullptr, m_mapStrus);

	WidgetTargetList* wt2 = new WidgetTargetList(ui.widgetRef);

	wt2->setGeometry(340 + wt1->size().width(), 120, wt2->size().width(), wt2->size().height() - 40);
	wt2->raise();
	wt2->show();

	WidgetRefList* w4 = new WidgetRefList(ui.widgetRef);
	
	w4->setGeometry(40, 120, w4->size().width(), w4->size().height());
	w4->raise();
	w4->show();

	w4->ui.listWidget->addItem(QString::fromUtf8("国家体育场 (鸟巢)"));
	w4->ui.listWidget->addItem(QString::fromUtf8("新世纪大桥"));
	w4->ui.listWidget->addItem(QString::fromUtf8("国家会议中心"));
	w4->ui.listWidget->addItem(QString::fromUtf8("奥林匹克公园"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("新世纪大桥"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("国家会议中心"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("奥林匹克公园"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("新世纪大桥"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("国家会议中心"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("奥林匹克公园"));

	m_mapLayoutWidget = new WidgetOfflineMap(ui.widget_map);
	m_mapWidget = new WidgetOfflineMap(ui.widget_map);
	QString uri = "file:G:/zhy_work/04.test/40.tools/07.baidumap/02.bmap-offline-for-web-master/index.html";
	m_mapWidget->Initialize(uri);
	
	m_mapLayoutWidget->Initialize("file:html/map.html");
	connect(m_mapWidget, &WidgetOfflineMap::MapReturnGps, this, &MultiLargeScreenFixedWidget::getCoordinates, Qt::UniqueConnection);
	ui.label_5->raise();
	m_mapLayoutWidget->raise();
	//ui.widgetMap->show();
	
	w2->raise();
	w1->raise();

	ui.label_5->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	ui.label_5->setAttribute(Qt::WA_TranslucentBackground, true);

	ui.label_ref->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	ui.label_ref->setAttribute(Qt::WA_TranslucentBackground, true);

	std::vector<std::string> urls;
	//urls.push_back("rtsp://admin:zhkj2501@192.168.0.92");
	//urls.push_back("rtsp://admin:zhkj2501@192.168.0.92");

	urls.push_back("rtsp://172.16.0.107/drone/11/stream1"); 
	urls.push_back("rtsp://172.16.0.107/drone/14/stream1");
	urls.push_back("rtsp://172.16.0.107/drone/23/stream1");
	urls.push_back("rtsp://172.16.0.107/drone/32/stream1");
	urls.push_back("rtsp://172.16.0.107:8554/giga/ch2/stream1");
	OpenStream(urls);
	m_blRunning = true;
	m_thread = std::thread(&MultiLargeScreenFixedWidget::startVideo, this);

	Initialize();
	InitUI();
	//this->showFullScreen();

	m_blResize = true;

}


MultiLargeScreenFixedWidget::~MultiLargeScreenFixedWidget()
{
	m_blRunning = false;

	stopVideo();

	disconnect(this, &MultiLargeScreenFixedWidget::showYuvSignal1, ui.widgetCam1, &GLYuvWidget::slotShowYuvFrame);
	disconnect(this, &MultiLargeScreenFixedWidget::showYuvSignal2, ui.widgetCam2, &GLYuvWidget::slotShowYuvFrame);
	disconnect(this, &MultiLargeScreenFixedWidget::showYuvSignal3, ui.widgetCam3, &GLYuvWidget::slotShowYuvFrame);
	disconnect(this, &MultiLargeScreenFixedWidget::showYuvSignal4, ui.widgetCam4, &GLYuvWidget::slotShowYuvFrame);
	disconnect(this, &MultiLargeScreenFixedWidget::showYuvSignal5, ui.widgetRef, &GLYuvWidget::slotShowYuvFrame);
	disconnect(this, &MultiLargeScreenFixedWidget::showYuvSignal6, m_widgetAlarm, &GLYuvWidget::slotShowYuvFrame);

	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

bool MultiLargeScreenFixedWidget::Initialize()
{
	//return false;
	//1. start all processes manager...
	m_httpClientSvr = new HttpClientSvr(this);
	m_svrResourceMng = new SvrResourceMng(this);
	//2. set parameters ...
	m_svrResourceMng->SetParam(m_httpClientSvr);

	m_widgetAlarm->show();

	//todo ..............
	return false;
	
	//3. get info...
	QString str;
	bool bl = m_svrResourceMng->GetRemoteDbInfo(0, str);
	if (!bl)
	{
		g_logger->warn("GetRemoteDbInfo failed!");
	}
	else
		g_logger->info("GetRemoteDbInfo successed!");

	bl = m_svrResourceMng->GetRemoteIconInfo(str);
	if (!bl)
	{
		g_logger->warn("GetRemoteIconInfo failed!");
	}
	else
		g_logger->info("GetRemoteIconInfo successed!");

	return true;
}

void MultiLargeScreenFixedWidget::InitUI()
{
	setStyleSheet("#formFixedFull { background-color: rgb(8, 4, 27);}");
	for (int i = 0; i < M_MAX_SEG; i++)
	{
		m_widgetCams[i] = new WidgetCam();
		//m_widgetCams[i]
	}

	//全部字体
	QList<QLabel *> lsLbl = this->findChildren<QLabel *>();
	QFont ft;
	for (int i = 0; i < lsLbl.size(); i++)
	{
		QLabel * lbl = lsLbl.at(i);
		
		qDebug() << lbl->objectName();

		ft = ui.label->font();

		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		if (lbl->objectName() != "labelTitle" && lbl->objectName() != "labelStatus")
		{
			ft.setPointSize(9 * m_fZoom);
			ft.setBold(true);
			ft.setWeight(75);

			lbl->setFont(ft);
		}

		QPalette pe;
		//QColor cl(RGB());
		pe.setColor(QPalette::WindowText, QColor("#DEDFE1") );
		lbl->setPalette(pe);

	}

	//	//todo  1.计算视频标签放置点位
	//QPoint pt;
	//QRect rt = geometry();
	//2. 视频标签
	//pt.x = rt
	//todo
	//connect()
}

//void MultiLargeScreenFixedWidget::GpsLocation(double longitude, double latitude)
//{
//	double lng = longitude;
//	double lat = latitude;
//	//qDebug() << fixed << qSetRealNumberPrecision(10) << longitude << latitude;
//	ui.widgetMap->page()->runJavaScript(QString("theLocation_obstacle(%1,%2)").arg(lng, 0, 'g', 11).arg(lat, 0, 'g', 10));
//}
//
//void MultiLargeScreenFixedWidget::GpsUpdate(double longitude, double latitude, QJsonArray json_arr, QString revsecond, QString revthird, double longitude1, double latitude1)
//{
//	//qDebug() << "Hweekhun" <<revsecond << revthird;
//	//emit obstacle_road(revsecond, revthird, longitude1, latitude1);
//	//QString json_str = QString(QJsonDocument(json_arr).toJson()).remove(QRegExp("\\s"));
//	//qDebug() << json_str;
//	//if (loadflag == true) {
//	double lng = longitude;
//	double lat = latitude;
//	//qDebug() << fixed << qSetRealNumberPrecision(10) << longitude << latitude;
//	ui.widgetMap->page()->runJavaScript(QString("theLocation_obstacle(%1,%2)").arg(lng, 0, 'g', 11).arg(lat, 0, 'g', 10));
//	//}
//}

void MultiLargeScreenFixedWidget::OpenStream(std::vector<std::string> urls)
{
	cvcam::StreamOpenResultList resultList;
	cvcam::StreamParamsList paramsList;
	m_urls = urls;
	int i = 0;
	for (auto &str : urls)
	{
		cvcam::StreamParams params =
		{
			cvcam::StreamType::STREAM_TYPE_MAIN,
			str,
			cvcam::CameraType::CAMERA_TYPE_DETAIL,
			i++,
			std::to_string(i),
			NULL
		};
		paramsList.push_back(params);
	}

	m_manager = cvcam::StreamManager::create();

	int res = m_manager.open(paramsList, resultList);

	connect(this, &MultiLargeScreenFixedWidget::showYuvSignal6, m_widgetAlarm, &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);

	connect(this, &MultiLargeScreenFixedWidget::showYuvSignal5, ui.widgetRef, &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);

	connect(this, &MultiLargeScreenFixedWidget::showYuvSignal1, ui.widgetCam1, &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
	connect(this, &MultiLargeScreenFixedWidget::showYuvSignal2, ui.widgetCam2, &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
	connect(this, &MultiLargeScreenFixedWidget::showYuvSignal3, ui.widgetCam3, &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
	connect(this, &MultiLargeScreenFixedWidget::showYuvSignal4, ui.widgetCam4, &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
}

void MultiLargeScreenFixedWidget::stopVideo()
{
	m_blRunning = false;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	m_manager.close();
	int i = 10;
	while (!m_thread.joinable() && i > 0)
	{
		i--;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	m_thread.join();
}

void MultiLargeScreenFixedWidget::startVideo()
{
	m_manager.startDecoding();
	//cvcam::Frame frameX[2];
	cvcam::Frame frame2;
	cvcam::Frame frame;
	QTime tt;
	tt.start();
	while (m_blRunning)
	{
		for (int i = 0; i < m_urls.size(); i++)
		{
			cvcam::Stream stream = m_manager.at(i);
			cvcam::FrameQueue frameQueue = stream.frameQueue();
			
			cvcam::Frame frame;
			if (frameQueue.tryPop(frame))
			{
				if (!m_blRunning)
					break;
				if (frame.isValid())
				{
					//frameX[i] = frame;
					frame2 = frame;
					printf("frame: %d id= %d \n", frame.format(), i);

					if (!frame.data(0) || !frame.data(1))
					{
						printf("m_frame == %p %p \n", frame.data(0), frame.data(1));
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
						continue;
					}
#if 1
					if (i == 0)
					{
						emit showYuvSignal1(frame);
						emit showYuvSignal6(frame);
					}
					else if (i == 1)
						emit showYuvSignal2(frame);
					else if (i == 2)
						emit showYuvSignal3(frame);
					else if (i == 3)
						emit showYuvSignal4(frame);
					else if (i == 4)
						emit showYuvSignal5(frame);
#else
					//if (i == 0)
					//	emit showYuvSignal1((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
					//else if (i == 1)
					//	emit showYuvSignal2((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
					//else if (i == 2)
					//	emit showYuvSignal3((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
					//else if (i == 3)
					//	emit showYuvSignal4((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
					//else if (i == 4)
					//	emit showYuvSignalStitch((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
					
					//emit showYuvSignal5(frame);
#endif
					qDebug("tryPop elapsed: %d ms", tt.elapsed());
					tt.restart();
					std::this_thread::sleep_for(std::chrono::milliseconds(2));

					frame2 = cvcam::Frame();
				}
			}
			else
			{
				//if (i == 0)
				//	emit showYuvSignal1((uchar*)frameX[i].data(0), (uchar*)frameX[i].data(1), (uchar*)frameX[i].data(2), frameX[i].width(), frameX[i].height());
				//else
				//	emit showYuvSignalStitch((uchar*)frameX[i].data(0), (uchar*)frameX[i].data(1), (uchar*)frameX[i].data(2), frameX[i].width(), frameX[i].height());

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}

	m_manager.stopDecoding();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	//m_manager.close();
}

void MultiLargeScreenFixedWidget::getCoordinates(double lng, double lat)
{
	double wgs_longitude;
	double wgs_latitude;
	//loc bdcoord, gcjcoord, wgscoord;
	//bdcoord.lon = lon;
	//bdcoord.lat = lat;
	//gcjcoord = MyGps->bd2gcj(bdcoord);
	//wgscoord = MyGps->gcj2wgs(gcjcoord);
	wgs_longitude = lng;
	wgs_latitude = lat;

	QString gps = "Longitude, Latitude: [" + QString::number(wgs_longitude, 'g', 12) + "°, " + QString::number(wgs_latitude, 'g', 11) + "° ]";
	QPalette pa;
	pa.setColor(QPalette::WindowText, Qt::white);
	//ui.label_10->setPalette(pa);
	//ui.label_10->setText(gps);
}

void MultiLargeScreenFixedWidget::getCoordinateRect(double lngs, double lats, double lnge, double late)
{
	//todo 
	printf("");
}

void MultiLargeScreenFixedWidget::btnOnlySegSlot()
{

}

void MultiLargeScreenFixedWidget::btnFourSegSlot()
{

}

void MultiLargeScreenFixedWidget::btnNineSegSlot()
{

}

void MultiLargeScreenFixedWidget::resizeEvent(QResizeEvent *event)
{
	if (m_blResize)
	{
		QRect rt = ui.widget_map->geometry();
		qDebug() << rt;
		m_mapWidget->setGeometry(rt);
		m_mapWidget->move(0, 0);
	}
}


#if 0
bool MultiLargeScreenFixedWidget::isTopItem(QTreeWidgetItem* item)
{
	if (!item) return false;
	if (!item->parent())
		return true;
	return false;
}

void MultiLargeScreenFixedWidget::setChildCheckState(QTreeWidgetItem *item, Qt::CheckState cs)
{
	if (!item) return;	for (int i = 0; i < item->childCount(); i++)
	{
		QTreeWidgetItem* child = item->child(i);
		if (child->checkState(0) != cs)
		{
			child->setCheckState(0, cs);
		}
	}
	setParentCheckState(item->parent());
}

void MultiLargeScreenFixedWidget::setParentCheckState(QTreeWidgetItem *item)
{
	if (!item) return;	int selectedCount = 0;	int childCount = item->childCount();
	for (int i = 0; i < childCount; i++)
	{
		QTreeWidgetItem* child = item->child(i);
		if (child->checkState(0) == Qt::Checked)
		{
			selectedCount++;
		}
	}
	if (selectedCount == 0)
	{
		item->setCheckState(0, Qt::Unchecked);
	}
	else if (selectedCount == childCount)
	{
		item->setCheckState(0, Qt::Checked);
	}
	else
	{
		item->setCheckState(0, Qt::PartiallyChecked);
	}
}

QTreeWidgetItem* MultiLargeScreenFixedWidget::FindtreeViewItem(QTreeWidgetItem *treeWidgetItem, GM_DEVICEGROUP_STRU* mapStrus)
{
	QTreeWidgetItemIterator it(treeWidgetItem);

	while (*it)
	{
		QTreeWidgetItem *pItem = (*it);
		int qv = -1;
		if (pItem != NULL)
		{
			qv = pItem->data(0, Qt::UserRole).toInt();
		}

		if (qv == mapStrus->deviceautoId)
		{
			return pItem;
		}
		it++;
	}
	return NULL;
}

QTreeWidgetItem* MultiLargeScreenFixedWidget::GettreeViewItem(QTreeWidget *treeWidget, QMap<int, PGM_DEVICEGROUP_STRU> mapStrus, int nId)
{
	GM_DEVICEGROUP_STRU* tmp = NULL;
	for (int iii = 0; iii < mapStrus.size(); iii++)
	{
		if (mapStrus[iii]->deviceautoId == nId)
		{
			tmp = mapStrus[iii];
			break;
		}
	}

	if (tmp == NULL)
		return NULL;
	QTreeWidgetItem * pItem = FindtreeViewItem(treeWidget->invisibleRootItem(), tmp);
	return pItem;
}

void MultiLargeScreenFixedWidget::AddDevicesTotreeView(QTreeWidget *treeWidget, const QStringList *header, QMap<int, PGM_DEVICEGROUP_STRU> mapStrus)
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
	for (int iii = 0; iii < mapStrus.size(); iii++)
	{
		if (mapStrus[iii]->nodeType == 0)
		{
			QString strInfo = QString::fromLocal8Bit(mapStrus[iii]->nodeName);
			QTreeWidgetItem *imageItem1 = new QTreeWidgetItem(treeWidget, QStringList(strInfo));
			imageItem1->setIcon(0, QIcon(":/UI/icon/camgroup.png"));
			imageItemParent = imageItem1;
			imageItem1->setData(0, Qt::UserRole, mapStrus[iii]->deviceautoId);
		}
		else if (mapStrus[iii]->nodeType == 1)
		{
			if (mapStrus[iii]->parentId != 0)
			{
				//获取父节点
				imageItemParent = GettreeViewItem(treeWidget, mapStrus, mapStrus[iii]->deviceParentAutoId);
				if (imageItemParent == NULL)
					continue;

				QString strInfo = QString::fromLocal8Bit(mapStrus[iii]->nodeName);
				QTreeWidgetItem *imageItem1_1 = new QTreeWidgetItem(imageItemParent, QStringList(strInfo));

				imageItem1_1->setData(0, Qt::UserRole, mapStrus[iii]->deviceautoId);
				imageItemParent->addChild(imageItem1_1);
			}
		}
		else if (mapStrus[iii]->nodeType == 9 || mapStrus[iii]->nodeType == 1000)
		{
			if (mapStrus[iii]->parentId != 0)
			{
				//获取父节点
				imageItemParent = GettreeViewItem(treeWidget, mapStrus, mapStrus[iii]->deviceParentAutoId);
				if (imageItemParent == NULL)
					continue;

				QString strInfo = QString::fromLocal8Bit(mapStrus[iii]->nodeName);
				QTreeWidgetItem *imageItem1_2 = new QTreeWidgetItem(imageItemParent, QStringList(strInfo));

				imageItem1_2->setIcon(0, QIcon(":/UI/icon/pantarget.png"));
				imageItem1_2->setCheckState(0, Qt::Unchecked);
				imageItem1_2->setData(0, Qt::UserRole, mapStrus[iii]->deviceautoId);

				if (mapStrus[iii]->devicetype == 1)
					//1 球机 2 云台枪 3 枪机 4 鱼眼 5超广角 6... 0未知
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/camtarget.png"));
				else if (mapStrus[iii]->devicetype == 2)
					imageItem1_2->setIcon(0, QIcon(":/UI/icon/cam.png"));
				else if (mapStrus[iii]->devicetype == 3)
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
#endif


void MultiLargeScreenFixedWidget::FakeData()
{
	QMap<int, GM_DEVICEGROUP_STRU> mapStrus = m_mapStrus;;
	GM_DEVICEGROUP_STRU p1;
	p1.cameraid = 1;
	strcpy(p1.nodeName, "摄像机组");
	p1.nodeType = 0;
	p1.parentId = 0;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 2;
	strcpy(p1.nodeName, "新华大厦1区");
	p1.nodeType = 1;
	p1.devicetype = 0;
	p1.parentId = 1;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 3;
	strcpy(p1.nodeName, "新华大厦1区102");
	p1.nodeType = 9;
	p1.devicetype = 1;
	p1.parentId = 2;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 4;
	strcpy(p1.nodeName, "新华大厦1区104");
	p1.nodeType = 9;
	p1.devicetype = 1;
	p1.parentId = 2;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 5;
	strcpy(p1.nodeName, "新华大厦3区");
	p1.nodeType = 1;
	p1.devicetype = 0;
	p1.parentId = 1;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 6;
	strcpy(p1.nodeName, "新华大厦3区301");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = 5;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 7;
	strcpy(p1.nodeName, "新华大厦3区302");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = 5;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 8;
	strcpy(p1.nodeName, "新华大厦3区309");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = 5;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 9;
	strcpy(p1.nodeName, "新华大厦3区309-1");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = 8;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 10;
	strcpy(p1.nodeName, "新华大厦3区312");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = 5;
	mapStrus[p1.cameraid] = p1;

	p1.cameraid = 11;
	strcpy(p1.nodeName, "新华大厦3区315AAAAAA");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = 5;
	mapStrus[p1.cameraid] = p1;
}
