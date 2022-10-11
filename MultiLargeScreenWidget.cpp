#include "MultiLargeScreenWidget.h"
#include <QUrl>
#include <QWebChannel>
#include <QWebEngineView>
#include <QJsonArray>
#include <QDesktopWidget>
#include <QScreen>
#include "GLYuvWidget.h"
#include <QDir>
#include <QScrollBar>
#include <QList>

MultiLargeScreenWidget * g_pMain = nullptr;
extern Log4Qt::Logger * g_logger;

const QString C_WIDGETICONLABEL[5][4] = {
	{"聚福园自助餐厅", ":/UI/icon/restaurant2x.png", "1365", "1567"}, {"奥林匹克公园",":/UI/icon/scenicspot.png", "1574", "1640"}, 
	{"国家体育场", ":/UI/icon/nesticon2x.png", "1573", "1179"}, {"水立方", ":/UI/icon/watercubeicon2x.png", "2201", "1612"},
	{"亚奥人家小区", ":/UI/icon/building2x.png", "2842", "981"}
};
//MATCH_ALARMTYPE

const std::string C_ALARMDESC[10] = {"CL_Person", "CL_Car", "CR_Person", "CR_Car", "CR_Car_Road", "PRS_FireExit", "PRS_Car", "PRS_Bike", "Density_Person", "Density_Car"};
//const QString C_DEVICETYPE[10][2] = { {QString::fromUtf8("全景相机"), "global2x"}, {QString::fromUtf8("高点相机"), "dome2x"}, 
//	{QString::fromUtf8("低点相机"), "fixcam2x"}, { QString::fromUtf8("建筑"), "buildingicon2x"}, {QString::fromUtf8("水立方"), "watercubeicon2x"},
//	{QString::fromUtf8("国家体育场"), "nesticon2x"},{QString::fromUtf8("奥林匹克公园"), "park2x"}, { QString::fromUtf8("餐厅"), "restaurant2x" }, 
//	{ QString::fromUtf8("餐厅"), "restaurant2x" },{ QString::fromUtf8("其他"), "other2x" } };

const QString C_DEVICETYPE[7][2] = { {QString::fromUtf8("全景相机"), "1"}, {QString::fromUtf8("高点相机"), "2"},
	{QString::fromUtf8("低点相机"), "3"}, { QString::fromUtf8("标志建筑"), "4"}, {QString::fromUtf8("餐饮旅游"), "5"},
	{QString::fromUtf8("餐厅"), "6"}, {QString::fromUtf8("居民社区"), "7"} };

#define GET_ARRAY_LEN(defarray) (sizeof(defarray)/sizeof(defarray[0]))
#define MATCH_ARRAYCONTENT(x, y, z)  { \
	for (int i = 0; i < sizeof(y)/sizeof(y[0]); i++) \
	{ if (x == y[i]) { z = i; break; } }  \
	z = -1; }; 

#define MATCH_ALARMTYPE(x, z)  { MATCH_ARRAYCONTENT(x, C_ALARMDESC, z); };

const int C_WIDGETLABELS[7][2] = { {198, 1377}, {1087, 1087}, {1347, 1127},  {1452, 1194}, {1250, 1203},{1904, 1138}, {2441, 1258} };

int Match_AlarmType(QString x)
{
	for (int i = 0; i < sizeof(C_ALARMDESC) / sizeof(C_ALARMDESC[0]); i++)
	{
		if (x.toUtf8().toStdString() == C_ALARMDESC[i])
		{
			return i;
		}
	}

	return -1;
}

int Match_DeviceTypeName(QString x, QString &y)
{
	for (int i = 0; i < sizeof(C_DEVICETYPE) / sizeof(C_DEVICETYPE[0]); i++)
	{
		if (x.toUtf8() == C_DEVICETYPE[i][0])
		{
			y = C_DEVICETYPE[i][1];
			return i;
		}
	}

	return -1;
}

//int Match_DeviceType(QString x)
//{
//	for (int i = 0; i < sizeof(C_DEVICETYPE) / sizeof(C_DEVICETYPE[0]); i++)
//	{
//		if (x.toUtf8() == C_DEVICETYPE[i][1])
//		{
//			return i;
//		}
//	}
//
//	return -1;
//}

int Match_DeviceType(QString x)
{
	for (int i = 0; i < sizeof(C_DEVICETYPE) / sizeof(C_DEVICETYPE[0]); i++)
	{
		if (x.toUtf8() == C_DEVICETYPE[i][0])
		{
			return i;
		}
	}

	return -1;
}

MultiLargeScreenWidget::MultiLargeScreenWidget(QWidget *parent)
	: QWidget(parent)
{
	qRegisterMetaType<cvcam::Frame>("cvcam::Frame");
	qRegisterMetaType<MqAlarmInfo>("MqAlarmInfo");

	m_changeUserWidget = new WidgetChangeUser(this);
	m_loginWidget = new WidgetLogin();
	
	m_nCurrentMaxCam = 4;

	m_blResize = false;

	ui.setupUi(this);
	
	m_strUser = "guest";
	g_pMain = this;

	m_config = new Config();
	m_config->load(QCoreApplication::applicationDirPath() + "/largescreen.json");

	QFile qss(":/UI/qss/treeWidget.qss");
	qss.open(QFile::ReadOnly);
	QByteArray ba = qss.readAll();
	qss.close();
	ui.treeWidgetCam->setStyleSheet(ba.data());
	qss.setFileName(":/UI/qss/verticalScrollBar.qss");
	qss.open(QFile::ReadOnly);
	ba = qss.readAll();
	qss.close();
	ui.treeWidgetCam->verticalScrollBar()->setStyleSheet(ba.data());

	qss.setFileName(":/UI/qss/horizontalScrollBar.qss");
	qss.open(QFile::ReadOnly);
	ba = qss.readAll();
	qss.close();
	ui.treeWidgetCam->horizontalScrollBar()->setStyleSheet(ba.data());

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

	connect(ui.btnOnlySeg, &LabelClick::clicked, this, &MultiLargeScreenWidget::btnOnlySegSlot, Qt::UniqueConnection);
	connect(ui.btnFourSeg, &LabelClick::clicked, this, &MultiLargeScreenWidget::btnFourSegSlot, Qt::UniqueConnection);
	connect(ui.btnNineSeg, &LabelClick::clicked, this, &MultiLargeScreenWidget::btnNineSegSlot, Qt::UniqueConnection);

	//user
	connect(ui.btnGlobal, &LabelClick::clicked, this, &MultiLargeScreenWidget::btnGlobalSlot, Qt::UniqueConnection);
	connect(ui.btnLabels, &LabelClick::clicked, this, &MultiLargeScreenWidget::btnLabelsSlot, Qt::UniqueConnection);
	connect(ui.btnChangeUser, &LabelClick::clicked, this, &MultiLargeScreenWidget::btnChangeUserSlot, Qt::UniqueConnection);
	
	//treeWidget && video
	connect(ui.treeWidgetCam, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeCamItemChangedSlot(QTreeWidgetItem*, int)), Qt::UniqueConnection);
	connect(ui.treeWidgetCam, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(treeCamItemDoubleClickedSlot(QTreeWidgetItem*, int)), Qt::UniqueConnection);

	ui.label_5->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	ui.label_5->setAttribute(Qt::WA_TranslucentBackground, true);

	//1.
	Initialize();
	InitUI();
	
	m_loginWidget->Resizefont(m_fZoom);

	m_blResize = true;
	
	m_blRunning = true;

	//2.
	for (int i = 0; i < m_config->alarmInf.maxShowCount; i++)
	{
		LabelAlarmIcon * lbl = new LabelAlarmIcon(ui.widgetMap);
		connect(m_mapWidget, &WidgetOfflineMap::MapAlarmLabelReturnPixel, lbl, &LabelAlarmIcon::ShowLabelPlaceSlot, Qt::UniqueConnection);
		m_lsLblAlarm.push_back(lbl);
	}
	m_lsLblAlarmPos = 0;

	int is = 0;
	int iss = 0;
	for (auto &sc : m_lsScene)
	{
		if (sc.longtitude >= -180.0 && sc.longtitude <= 180.0 && sc.latitude >= -90.0 && sc.latitude <= 90.0)
		{
			LabelRefIcon * lbl = new LabelRefIcon(ui.widgetMap);
			connect(m_mapWidget, &WidgetOfflineMap::MapRefLabelReturnPixel, lbl, &LabelRefIcon::ShowLabelPlaceSlot, Qt::UniqueConnection);
			//就是场景id
			lbl->Init(0, sc, sc.deviceId);
			m_lsLblRef.push_back(lbl);
		}

#ifdef _DEBUG
		if (sc.name == QString::fromLocal8Bit("双清路") )
		{
			LabelRefIcon * lbl = new LabelRefIcon(ui.widgetMap);
			connect(m_mapWidget, &WidgetOfflineMap::MapRefLabelReturnPixel, lbl, &LabelRefIcon::ShowLabelPlaceSlot, Qt::UniqueConnection);
			//就是场景id
			sc.longtitude = 116.347262;
			sc.latitude = 40.005316;

			lbl->Init(0, sc, sc.deviceId);
			m_lsLblRef.push_back(lbl);
			iss = is;
		}
		if ( sc.name == QString::fromLocal8Bit("朝阳区"))
		{
			LabelRefIcon * lbl = new LabelRefIcon(ui.widgetMap);
			connect(m_mapWidget, &WidgetOfflineMap::MapRefLabelReturnPixel, lbl, &LabelRefIcon::ShowLabelPlaceSlot, Qt::UniqueConnection);
			//就是场景id
			sc.longtitude = 116.345517;
			sc.latitude = 40.004224;
			//116.347262,
			lbl->Init(0, sc, sc.deviceId);
			m_lsLblRef.push_back(lbl);
		}

		is++;
#endif
	}

	//4.
	connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
	timer.setInterval(1000);

	//6. ShowRefLabel
	Resizefont();
	//7. rabbitmq
	ConnectMq();

	ShowRefLabel();
}

void MultiLargeScreenWidget::ShowRefLabel()
{
	//全景标签
	QString lblids, lbltypes, szxs, szys;

	for (auto &it1 : m_lsLblRef)
	{
		QString lblid, lat, lng;
		it1->GetLabelInfo(lblid, lat, lng);

		lblids = lblids + "," + lblid;
		lbltypes = lbltypes + ", 0";
		szxs = szxs + "," + lat;
		szys = szys + "," + lng;
	}

	// 更新html里面的标签位置
	m_mapWidget->PushLabelAndUpdateGps(lblids, lbltypes, szxs, szys);
}

//qt geometry 刷新问题 退而求其次
void MultiLargeScreenWidget::onTimeout()
{
	timer.stop();
	ResizeUi();

	loadUiStatus();

	//优先使用上次退出时的  没有使用最后一个 sceneid 
	bool blFound = false;
	if (m_lsScene.size() > 0)
	{
		for (auto scsc : m_lsScene)
		{
			if (m_stLocalStatus.sceneId == scsc.deviceId)
			{
				blFound = true;
				ChangeSceneSlot(m_stLocalStatus.sceneId);
				break;
			}
		}
		if (!blFound)
			ChangeSceneSlot(m_lsScene[m_lsScene.size() - 1].deviceId);
	}

	if (!m_config->mapInf.dynamicCenterMap)
	{
		m_mapWidget->CenterPoint(m_config->mapInf.centerMapPointy, m_config->mapInf.centerMapPointx, m_config->mapInf.centerMapZoom);
	}
}

MultiLargeScreenWidget::~MultiLargeScreenWidget()
{
	m_consumer.stop();
	m_blRunning = false;

	//stopVideo();

	//disconnect(this, &MultiLargeScreenWidget::showYuvSignal1, m_widgetCams[0], &GLYuvWidget::slotShowYuvFrame);
	//disconnect(this, &MultiLargeScreenWidget::showYuvSignal2, m_widgetCams[1], &GLYuvWidget::slotShowYuvFrame);
	//disconnect(this, &MultiLargeScreenWidget::showYuvSignal3, m_widgetCams[2], &GLYuvWidget::slotShowYuvFrame);
	//disconnect(this, &MultiLargeScreenWidget::showYuvSignal4, m_widgetCams[3], &GLYuvWidget::slotShowYuvFrame);
	//disconnect(this, &MultiLargeScreenWidget::showYuvSignal5, ui.widgetRef, &GLYuvWidget::slotShowYuvFrame);
	//disconnect(this, &MultiLargeScreenWidget::showYuvSignal6, m_widgetAlarm, &GLYuvWidget::slotShowYuvFrame);

	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

bool MultiLargeScreenWidget::Initialize()
{
	//1. start all processes manager...
	m_httpClientSvr = new HttpClientSvr(this);
	m_svrInterface = new SvrInterface();

	m_svrInterface->SetParam(m_httpClientSvr, m_config, m_config->stConfig.szServerIp.toStdString(), m_config->stConfig.szUriPath.toStdString(),
		m_config->stConfig.serverPort);
	m_svrResourceMng = new SvrResourceMng(this);
	//2. set parameters ...
	m_svrResourceMng->SetParam(m_httpClientSvr);

	//3. get devices info
	m_mapOrgs.clear();
	m_mapDevices.clear();

	QTime tm1;
	tm1.start();
	qDebug() << "start GetAllOrganization ";
	m_svrInterface->GetAllOrganization(m_mapOrgs, m_vCameraGroup, m_mapDevices, m_lsScene, m_mapOrgLinkDevice, m_vLabelGroup, m_mapLabels, m_mapDeviceLinkLabel);
	qDebug() << "end GetAllOrganization " << tm1.elapsed();
	//4. get info... from http
	QString str;
	bool bl = m_svrResourceMng->GetRemoteDbInfo(0, str);
	if (!bl)
	{
		//g_logger->warn("GetRemoteDbInfo failed!");
	}
	else
		;// g_logger->info("GetRemoteDbInfo successed!");

	bl = m_svrResourceMng->GetRemoteIconInfo(str);
	if (!bl)
	{
		//g_logger->warn("GetRemoteIconInfo failed!");
	}
	else
		;// g_logger->info("GetRemoteIconInfo successed!");

	return true;
}

bool MultiLargeScreenWidget::ConnectMq()
{
	m_svrAlarm = new MqAlarmSvr();
	m_svrAlarm->startThread(m_config);

	std::vector<std::string> exchangename;
	std::vector<std::string> routingkey;

	for (auto &it : m_config->consumer.exchangename)
	{
		//qDebug() << "exchangename" << it.toStdString().c_str();
		exchangename.push_back(it.toStdString());
	}
	for (auto &it : m_config->consumer.routingkey)
	{
		//qDebug() << "routingkey" << it.toStdString().c_str();
		routingkey.push_back(it.toStdString());
	}
	std::vector<std::string> scenes;

	for (auto &it : m_lsScene)
	{
		//qDebug() << "m_mapScenes" << it.second.id.toUtf8().toStdString().c_str();
		scenes.push_back(it.deviceId.toUtf8().toStdString());
	}

	//test
#ifdef DEBUG_TEST
	//scenes.push_back("3f50f36d-3299-44f9-ae87-4777766360df");
#endif
	ConsumerMsg::AmqpParam paramConsumer(m_config->consumer.host.toStdString(), m_config->consumer.port, m_config->consumer.username.toStdString(), m_config->consumer.password.toStdString(), 
		m_config->consumer.path.toStdString(), m_config->consumer.queuename.toStdString(), m_config->consumer.consumertag.toStdString(), exchangename, routingkey, scenes);

	qDebug() << m_config->consumer.host.toStdString().c_str() << m_config->consumer.port << m_config->consumer.username.toStdString().c_str() << m_config->consumer.password.toStdString().c_str() <<
		m_config->consumer.path.toStdString().c_str() << m_config->consumer.queuename.toStdString().c_str() << m_config->consumer.consumertag.toStdString().c_str();// << exchangename << routingkey << scenes;
	m_consumer.Connect(paramConsumer);
	//链接到报警处理类
	m_consumer.SetReceiveMessageCallback(std::bind(&MqAlarmSvr::ConsumerReceiveMessageCallbackImp, m_svrAlarm, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
	m_consumer.start();

	connect(m_svrAlarm, &MqAlarmSvr::showAlarm, this, &MultiLargeScreenWidget::AlarmInfSlot, Qt::UniqueConnection);
	return true;
}

void MultiLargeScreenWidget::DisconnectMq()
{
	m_consumer.stop(); 
	m_svrAlarm->stop();
}
#if 0
void MultiLargeScreenWidget::InitUi1()
{
	//wdetail = new WidgetDetailInfo(ui.widgetMap);
	//QRect rt1 = ui.widgetMap->geometry();
	////w1->setParent(ui.widgetMap);
	//wdetail->setGeometry(350, 120, wdetail->size().width(), wdetail->size().height());

	//wdetail->show();
	//walarm = new WidgetAlarmInfo(ui.widgetMap);
	//walarm->setParent(ui.widgetMap);
	//walarm->setGeometry(938, 111, walarm->size().width(), walarm->size().height());
	//walarm->show();

	QRect rt2 = ui.widgetRef->geometry();

	m_widgetGlobal = new WidgetRefList(ui.widgetRef);

	m_widgetGlobal->setGeometry(20, 120, m_widgetGlobal->size().width(), m_widgetGlobal->size().height());
	m_widgetGlobal->raise();
	m_widgetGlobal->hide();

	connect(m_widgetGlobal, &WidgetRefList::ChangeScene, this, &MultiLargeScreenWidget::ChangeSceneSlot, Qt::UniqueConnection);

	for (STRU_DB_DEVICE& dd : m_lsScene)
	{
		m_widgetGlobal->AddDevice(dd);
		m_widgetGlobal->ui.listWidget->addItem(dd.name);
	}


	//m_widgetGlobal->ui.listWidget->addItem(QString::fromUtf8("新世纪大桥"));
	//m_widgetGlobal->ui.listWidget->addItem(QString::fromUtf8("国家会议中心"));
	//m_widgetGlobal->ui.listWidget->addItem(QString::fromUtf8("奥林匹克公园"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("新世纪大桥"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("国家会议中心"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("奥林匹克公园"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("新世纪大桥"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("国家会议中心"));
	//w4->ui.listWidget->addItem(QString::fromUtf8("奥林匹克公园"));

	//m_mapLayoutWidget = new WidgetOfflineMap(ui.widgetMap);
	m_mapWidget = new WidgetOfflineMap(ui.widgetMap);
	//G:\zhy_work\02.code\23.MultiLargeScreen\MultiLargeScreen\html

	QString filePath = qApp->applicationDirPath();
	QString uri = QString::fromLocal8Bit("file:%1/html/index.html").arg(qApp->applicationDirPath());

	qDebug() << uri;
	m_mapWidget->Initialize(uri, true);

	//m_mapWidget->Initialize("file:G:/maponline.html");
	//m_mapLayoutWidget->Initialize("url(:html/map.html)");
	//m_mapLayoutWidget->Initialize("file:G:/maponline.html");
	connect(m_mapWidget, &WidgetOfflineMap::MapReturnGps, this, &MultiLargeScreenWidget::getCoordinates, Qt::UniqueConnection);
	connect(m_mapWidget, &WidgetOfflineMap::MapReturnPixels, this, &MultiLargeScreenWidget::MapReturnPixelsSlot, Qt::UniqueConnection);

	//ui.label_5->raise();
	//m_mapLayoutWidget->raise();
	//ui.widgetMap->show();

	//wdetail->raise();
	//walarm->raise();
#if 0
	//四个报警标签
	for (int i = 0; i < 4; i++)
	{
		QRect rt(700, i * 80 + 200, 60, 70);
		m_AlarmIcon[i] = new LabelAlarmIcon(ui.widgetMap);
		m_AlarmIcon[i]->setGeometry(rt);
		m_AlarmIcon[i]->Init(i);

		//connect(m_AlarmIcon[i], &LabelAlarmIcon::clicked, this, &MultiLargeScreenWidget::ShowAlarmDetailWidgetSlot, Qt::UniqueConnection);
		m_AlarmIcon[i]->show();
	}
#endif
	//5个地点标签
	//for (int i = 0; i < 5; i++)
	//{
	//	m_widgetLabels[i] = new WidgetLabel(ui.widgetRef);
	//	m_widgetLabels[i]->SetInfo(C_WIDGETICONLABEL[i][0], C_WIDGETICONLABEL[i][1]);
	//	m_widgetLabels[i]->move(C_WIDGETICONLABEL[i][2].toInt() - 78, C_WIDGETICONLABEL[i][3].toInt() -984);
	//	m_widgetLabels[i]->show();
	//}

	//7个WidgetIconLabel标签
	//for (int i = 0; i < 7; i++)
	//{
	//	m_widgetIconLabels[i] = new WidgetIconLabel(ui.widgetRef);
	//	m_widgetIconLabels[i]->SetInfo(i);
	//	m_widgetIconLabels[i]->move(C_WIDGETLABELS[i][0] - 78, C_WIDGETLABELS[i][1] - 984);
	//	m_widgetIconLabels[i]->show();
	//}

	ui.listWidget->addItem(QString::fromUtf8("预置位 001"));
	ui.listWidget->addItem(QString::fromUtf8("预置位 002"));
	ui.listWidget->addItem(QString::fromUtf8("预置位 003"));
	ui.listWidget->addItem(QString::fromUtf8("预置位 004"));
	ui.listWidget->addItem(QString::fromUtf8("预置位 005"));

	ui.listWidget->setStyleSheet("QListWidget{background-color:transparent; border:0px; margin:0px 0px 0px 0px;}"
		"QListWidget::Item{height:40px; border:0px; padding-left:14px; color: #FEFEFE;}"
		"QListWidget::Item:hover{color:rgb(0, 252, 252);}"
		"QListWidget::Item:selected{background-color:transparent; color:rgb(0, 252, 252);}");

	ui.listWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{ width:8px; background:#15808C; margin:0px,0px,0px,0px; padding-top:0px; padding-bottom:0px;}"
		"QScrollBar::handle:vertical{ width:8px; background:#00B3BE; border-radius:4px; min-height:20;}"
		"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical { background:#15808C; border-radius:4px;}");

	ui.listWidget->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{ height:8px; background:#15808C; margin:0px,0px,0px,0px; padding-left:0px; padding-right:0px;}"
		"QScrollBar::handle:horizontal{ width:8px; background:#00B3BE; border-radius:4px; min-width:20; }"
		"QScrollBar::add-page : horizontal, QScrollBar::sub-page : horizontal{ background:#15808C; border-radius:4px; }");
	//connect()

	TransDataCameraGroup();

	AddListInfo(NULL, ui.treeWidgetCam, m_mapCameraGroup);

	//QRect rt2 = ui.widgetRef->geometry();
	m_widgetTarget = new WidgetTargetList(ui.widgetRef);
	connect(m_widgetTarget, &WidgetTargetList::showLabelSignal, this, &MultiLargeScreenWidget::ShowLabelSlot, Qt::UniqueConnection);
	connect(m_widgetTarget, &WidgetTargetList::showTargetListSignal, this, &MultiLargeScreenWidget::ShowTargetListSlot, Qt::UniqueConnection);
	connect(m_widgetTarget, &WidgetTargetList::showCameraListSignal, this, &MultiLargeScreenWidget::ShowCameraListSlot, Qt::UniqueConnection);

	m_widgetTarget->setGeometry(300, 120, m_widgetTarget->size().width(), m_widgetTarget->size().height());
	m_widgetTarget->raise();
	m_widgetTarget->hide();

	TransDataTarget();
	TransDataCameras();
	QStringList ls;
	AddListInfo(&ls, m_widgetTarget->ui.treeWidget, m_mapTarget);
	////////////////////

	m_widgetTarget2 = new WidgetTargetList(ui.widgetRef);
	AddListInfo(&ls, m_widgetTarget2->ui.treeWidget, m_mapCameras);
	m_widgetTarget2->setGeometry(340 + m_widgetTarget->size().width(), 120, m_widgetTarget2->size().width(), m_widgetTarget2->size().height() - 40);
	m_widgetTarget2->raise();
	m_widgetTarget2->hide();

	m_widgetVideoSegment = new WidgetVideoSegment(ui.widgetVideos);
	m_widgetVideoSegment->setGeometry(ui.widgetVideos->geometry());
	//m_widgetVideoSegment->OpenStreamSlot(0, "rtsp://192.168.50.101/shuangqinglu/13/stream2");
	//m_widgetVideoSegment->OpenStreamSlot(1, "rtsp://192.168.50.101/shuangqinglu/14/stream2");
	//m_widgetVideoSegment->OpenStreamSlot(2, "rtsp://192.168.50.101/shuangqinglu/23/stream2");
	//m_widgetVideoSegment->OpenStreamSlot(3, "rtsp://192.168.50.101/shuangqinglu/34/stream2");
	connect(m_widgetVideoSegment, &WidgetVideoSegment::IndexChangeSignal, this, &MultiLargeScreenWidget::CtrlPanelEnabledSlot, Qt::UniqueConnection);

	m_widgetVideoSegment->show();
	//ui.widgetRef->OpenStream("rtsp://192.168.50.105:9554/giga/ch2/stream3", ":/UI/back/refcvcam.png");
	//m_widgetCams[0]->OpenStream
	//urls.push_back("rtsp://192.168.8.103:8554/giga/ch2/stream1");
}
#endif

void MultiLargeScreenWidget::InitUI()
{
	ui.lblTitle->setText(m_config->stConfig.szTitle);

	m_widgetSlider = new WidgetSlider(ui.widget_6);
	m_widgetSlider->show();

	//1. 全景 附着物
	m_lblExpandGlobal = new QLabel(ui.widgetRef);
	m_lblExpandGlobal->setBaseSize(28, 16);
	m_lblExpandGlobal->setGeometry(0, 0, 28, 16);
	m_lblExpandGlobal->setPixmap(QPixmap(QString::fromLocal8Bit(":/UI/icon/expand.png")));
	m_lblExpandGlobal->hide();

	m_lblExpandLabels = new QLabel(ui.widgetRef);
	m_lblExpandLabels->setBaseSize(28, 16);
	m_lblExpandLabels->setGeometry(0, 0, 28, 16);
	m_lblExpandLabels->setPixmap(QPixmap(QString::fromLocal8Bit(":/UI/icon/expand.png")));
	m_lblExpandLabels->hide();

	//控制盘操作
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickTop, this, &MultiLargeScreenWidget::CtrlPanelTopSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickTopRight, this, &MultiLargeScreenWidget::CtrlPanelTopRightSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickRight, this, &MultiLargeScreenWidget::CtrlPanelRightSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickBottomRight, this, &MultiLargeScreenWidget::CtrlPanelBottomRightSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickBottom, this, &MultiLargeScreenWidget::CtrlPanelBottomSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickBottomLeft, this, &MultiLargeScreenWidget::CtrlPanelBottomLeftSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickLeft, this, &MultiLargeScreenWidget::CtrlPanelLeftSlot, Qt::QueuedConnection);
	connect(ui.lblControlPanel, &WidgetControlPanel::OnClickTopLeft, this, &MultiLargeScreenWidget::CtrlPanelTopLeftSlot, Qt::QueuedConnection);

	connect(ui.lblControlPanel, &WidgetControlPanel::OnStop, this, &MultiLargeScreenWidget::CtrlPanelStop, Qt::QueuedConnection);

	connect(m_widgetSlider, &WidgetSlider::btnBigSignal, this, &MultiLargeScreenWidget::ZoominSlot, Qt::QueuedConnection);
	connect(m_widgetSlider, &WidgetSlider::btnSmallSignal, this, &MultiLargeScreenWidget::ZoomoutSlot, Qt::QueuedConnection);
	connect(m_widgetSlider, &WidgetSlider::btnStopSignal, this, &MultiLargeScreenWidget::CtrlPanelStop, Qt::QueuedConnection);
	
	connect(ui.listWidget, &QListWidget::itemClicked, this, &MultiLargeScreenWidget::PresetChangeSlot, Qt::AutoConnection);
	//2. 全景
	QRect rt2 = ui.widgetRef->geometry();

	m_widgetGlobal = new WidgetRefList(ui.widgetRef);

	m_widgetGlobal->setGeometry(20, 120, m_widgetGlobal->size().width(), m_widgetGlobal->size().height());
	m_widgetGlobal->raise();
	m_widgetGlobal->hide();

	connect(m_widgetGlobal, &WidgetRefList::ChangeScene, this, &MultiLargeScreenWidget::ChangeSceneSlot, Qt::UniqueConnection);

	for (STRU_DB_DEVICE& dd : m_lsScene)
	{
		m_widgetGlobal->AddDevice(dd);
		m_widgetGlobal->ui.listWidget->addItem(dd.name);
	}

	//m_widgetGlobal->ui.listWidget->addItem(QString::fromUtf8("新世纪大桥"));

	//3 地图
	m_mapWidget = new WidgetOfflineMap(ui.widgetMap);
	//G:\zhy_work\02.code\23.MultiLargeScreen\MultiLargeScreen\html

	QString filePath = qApp->applicationDirPath();
	QString uri = QString::fromLocal8Bit("file:%1/html/index.html").arg(qApp->applicationDirPath());

	qDebug() << uri;
	m_mapWidget->Initialize(uri, true);

	//m_mapWidget->Initialize("file:G:/maponline.html");
	connect(m_mapWidget, &WidgetOfflineMap::MapReturnGps, this, &MultiLargeScreenWidget::getCoordinates, Qt::UniqueConnection);
	connect(m_mapWidget, &WidgetOfflineMap::MapReturnPixels, this, &MultiLargeScreenWidget::MapReturnPixelsSlot, Qt::UniqueConnection);

	//4 预置位
	//ui.listWidget->addItem(QString::fromUtf8("预置位 001"));
	//ui.listWidget->addItem(QString::fromUtf8("预置位 002"));
	//ui.listWidget->addItem(QString::fromUtf8("预置位 003"));
	//ui.listWidget->addItem(QString::fromUtf8("预置位 004"));
	//ui.listWidget->addItem(QString::fromUtf8("预置位 005"));

	ui.listWidget->setStyleSheet("QListWidget{background-color:transparent; border:0px; margin:0px 0px 0px 0px;}"
		"QListWidget::Item{height:40px; border:0px; padding-left:14px; color: #FEFEFE;}"
		"QListWidget::Item:hover{color:rgb(0, 252, 252);}"
		"QListWidget::Item:selected{background-color:transparent; color:rgb(0, 252, 252);}");

	ui.listWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{ width:8px; background:#15808C; margin:0px,0px,0px,0px; padding-top:0px; padding-bottom:0px;}"
		"QScrollBar::handle:vertical{ width:8px; background:#00B3BE; border-radius:4px; min-height:20;}"
		"QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical { background:#15808C; border-radius:4px;}");

	ui.listWidget->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{ height:8px; background:#15808C; margin:0px,0px,0px,0px; padding-left:0px; padding-right:0px;}"
		"QScrollBar::handle:horizontal{ width:8px; background:#00B3BE; border-radius:4px; min-width:20; }"
		"QScrollBar::add-page : horizontal, QScrollBar::sub-page : horizontal{ background:#15808C; border-radius:4px; }");

	//5 左上角 设备树
	TransDataCameraGroup();

	AddListInfo(NULL, ui.treeWidgetCam, m_mapCameraGroup);

	//6. 标签
	m_widgetTarget = new WidgetTargetList(ui.widgetRef);
	connect(m_widgetTarget, &WidgetTargetList::showLabelSignal, this, &MultiLargeScreenWidget::ShowLabelSlot, Qt::UniqueConnection);
	//connect(m_widgetTarget, &WidgetTargetList::showTargetListSignal, this, &MultiLargeScreenWidget::ShowTargetListSlot, Qt::UniqueConnection);
	//connect(m_widgetTarget, &WidgetTargetList::showCameraListSignal, this, &MultiLargeScreenWidget::ShowCameraListSlot, Qt::UniqueConnection);

	m_widgetTarget->setGeometry(300, 120, m_widgetTarget->size().width(), m_widgetTarget->size().height());
	m_widgetTarget->raise();
	m_widgetTarget->hide();
	m_widgetTarget->Resizefont(m_fZoom);
	//TransDataTarget();
	//TransDataTarget();
	
	TransDataCameras();
	QStringList ls;
	//AddListInfo(&ls, m_widgetTarget->ui.treeWidget, m_mapTarget);
	////////////////////

	//m_widgetTarget2 = new WidgetTargetList(ui.widgetRef);
	//AddListInfo(&ls, m_widgetTarget2->ui.treeWidget, m_mapCameras);
	//m_widgetTarget2->setGeometry(340 + m_widgetTarget->size().width(), 120, m_widgetTarget2->size().width(), m_widgetTarget2->size().height() - 40);
	//m_widgetTarget2->raise();
	//m_widgetTarget2->hide();

	//7 . 分屏
	m_widgetVideoSegment = new WidgetVideoSegment(ui.widgetVideos);
	m_widgetVideoSegment->setGeometry(ui.widgetVideos->geometry());

	connect(m_widgetVideoSegment, &WidgetVideoSegment::IndexChangeSignal, this, &MultiLargeScreenWidget::CtrlPanelEnabledSlot, Qt::UniqueConnection);
	connect(m_widgetVideoSegment, &WidgetVideoSegment::ShowFullIndexSignal, this, &MultiLargeScreenWidget::ShowFullDomeSlot, Qt::UniqueConnection);
	m_widgetVideoSegment->show();

	ui.lblControlPanel->setDisabled(true);
	ui.widget_6->setDisabled(true);
	ui.listWidget->clear();
}

//void MultiLargeScreenWidget::GpsLocation(double longitude, double latitude)
//{
//	double lng = longitude;
//	double lat = latitude;
//	//qDebug() << fixed << qSetRealNumberPrecision(10) << longitude << latitude;
//	ui.widgetMap->page()->runJavaScript(QString("theLocation_obstacle(%1,%2)").arg(lng, 0, 'g', 11).arg(lat, 0, 'g', 10));
//}
//
//void MultiLargeScreenWidget::GpsUpdate(double longitude, double latitude, QJsonArray json_arr, QString revsecond, QString revthird, double longitude1, double latitude1)
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

void MultiLargeScreenWidget::Resizefont()
{
	//全部字体
	QList<QLabel *> lsLbl = this->findChildren<QLabel *>();
	QFont ft;
	for (int i = 0; i < lsLbl.size(); i++)
	{
		QLabel * lbl = lsLbl.at(i);

		ft = lbl->font();
		qDebug() << lbl->objectName() << "text:" << lbl->text() << " size:" << ft.pointSize();


		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		if (lbl->objectName() != "lblTitle" && lbl->objectName() != "labelStatus")
		{
			ft.setPointSize(ft.pointSize() / m_fZoom);
			lbl->setFont(ft);
		}
		else if (lbl->objectName() == "lblTitle")
		{
			ft.setPointSize(ft.pointSize() / m_fZoom);
		}

		QPalette pe;
		//QColor cl(RGB());
		pe.setColor(QPalette::WindowText, QColor("#DEDFE1"));
		lbl->setPalette(pe);
	}

	QList<QPushButton *> lsBtn = this->findChildren<QPushButton *>();
	for (int i = 0; i < lsBtn.size(); i++)
	{
		QPushButton * ls = lsBtn.at(i);
		ft = ls->font();

		qDebug() << ls->objectName() << " parent: " << ls->parent()->objectName() << " size:" << ft.pointSize();
		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(15.0 / m_fZoom);
		//ft.setBold(true);
		//ft.setWeight(75);
		ls->setFont(ft);
	}

	QList<QTreeWidget *> lsTree = this->findChildren<QTreeWidget *>();
	for (int i = 0; i < lsTree.size(); i++)
	{
		QTreeWidget * tr = lsTree.at(i);
		ft = tr->font();
		qDebug() << tr->objectName() << " parent: " << tr->parent()->objectName() << " size:" << ft.pointSize();
		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(15.0 / m_fZoom);
		//ft.setBold(true);
		//ft.setWeight(75);
		tr->setFont(ft);
	}

	QList<QListWidget *> lsLs = this->findChildren<QListWidget *>();
	for (int i = 0; i < lsLs.size(); i++)
	{
		QListWidget * ls = lsLs.at(i);
		ft = ls->font();
		
		qDebug() << ls->objectName() << " parent: " << ls->parent()->objectName() << " size:" << ft.pointSize();
		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(15.0 / m_fZoom);
		//ft.setBold(true);
		//ft.setWeight(75);
		ls->setFont(ft);
	}

	QList<QLineEdit *> lsEdt = this->findChildren<QLineEdit *>();

	for (int i = 0; i < lsEdt.size(); i++)
	{
		QLineEdit * edt = lsEdt.at(i);

		qDebug() << edt->objectName();

		ft = edt->font();

		ft.setFamily(QString::fromUtf8("\346\200\235\346\272\220\351\273\221\344\275\223 CN"));
		ft.setPointSize(ft.pointSize() / m_fZoom);
		edt->setFont(ft);
	}
}
//
//void MultiLargeScreenWidget::OpenStream(std::vector<std::string> urls)
//{
//	cvcam::StreamOpenResultList resultList;
//	cvcam::StreamParamsList paramsList;
//	m_urls = urls;
//	int i = 0;
//	for (auto &str : urls)
//	{
//		cvcam::StreamParams params =
//		{
//			cvcam::StreamType::STREAM_TYPE_MAIN,
//			str,
//			cvcam::CameraType::CAMERA_TYPE_DETAIL,
//			i++,
//			std::to_string(i),
//			NULL
//		};
//		paramsList.push_back(params);
//	}
//
//	m_manager = cvcam::StreamManager::create();
//
//	int res = m_manager.open(paramsList, resultList);
//
//	//connect(this, &MultiLargeScreenWidget::showYuvSignal6, m_widgetAlarm, &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
//	
//	connect(this, &MultiLargeScreenWidget::showYuvSignal5, ui.widgetRef, &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
//
//	connect(this, &MultiLargeScreenWidget::showYuvSignal1, m_widgetCams[0], &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
//	connect(this, &MultiLargeScreenWidget::showYuvSignal2, m_widgetCams[1], &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
//	connect(this, &MultiLargeScreenWidget::showYuvSignal3, m_widgetCams[2], &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
//	connect(this, &MultiLargeScreenWidget::showYuvSignal4, m_widgetCams[3], &GLYuvWidget::slotShowYuvFrame, Qt::AutoConnection);
//}
//
//void MultiLargeScreenWidget::stopVideo()
//{
//	m_blRunning = false;
//	std::this_thread::sleep_for(std::chrono::milliseconds(200));
//	m_manager.close();
//	int i = 10;
//	while (!m_thread.joinable() && i > 0)
//	{
//		i--;
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	}
//	m_thread.join();
//}
//
//void MultiLargeScreenWidget::startVideo()
//{
//	m_manager.startDecoding();
//	//cvcam::Frame frameX[2];
//	cvcam::Frame frame2;
//	cvcam::Frame frame;
//	QTime tt;
//	tt.start();
//	while (m_blRunning)
//	{
//		for (int i = 0; i < m_urls.size(); i++)
//		{
//			cvcam::Stream stream = m_manager.at(i);
//			cvcam::FrameQueue frameQueue = stream.frameQueue();
//			
//			cvcam::Frame frame;
//			if (frameQueue.tryPop(frame))
//			{
//				if (!m_blRunning)
//					break;
//				if (frame.isValid())
//				{
//					//frameX[i] = frame;
//					frame2 = frame;
//					//printf("frame: %d id= %d \n", frame.format(), i);
//
//					if (!frame.data(0) || !frame.data(1))
//					{
//						//printf("m_frame == %p %p \n", frame.data(0), frame.data(1));
//						std::this_thread::sleep_for(std::chrono::milliseconds(1));
//						continue;
//					}
//#if 1
//					if (i == 0)
//					{
//						//printf("----m_frame showYuvSignal1 ==%llu %p \n", ::GetTickCount64(), frame.impl());
//						emit showYuvSignal1(frame);
//						emit showYuvSignal6(frame);
//					}
//					else if (i == 1)
//						emit showYuvSignal2(frame);
//					else if (i == 2)
//						emit showYuvSignal3(frame);
//					else if (i == 3)
//						emit showYuvSignal4(frame);
//					else if (i == 4)
//						emit showYuvSignal5(frame);
//#else
//					//if (i == 0)
//					//	emit showYuvSignal1((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
//					//else if (i == 1)
//					//	emit showYuvSignal2((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
//					//else if (i == 2)
//					//	emit showYuvSignal3((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
//					//else if (i == 3)
//					//	emit showYuvSignal4((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
//					//else if (i == 4)
//					//	emit showYuvSignalStitch((uchar*)frame.data(0), (uchar*)frame.data(1), (uchar*)frame.data(2), frame.width(), frame.height());
//					
//					//emit showYuvSignal5(frame);
//#endif
//					//qDebug("tryPop elapsed: %d ms", tt.elapsed());
//					tt.restart();
//					std::this_thread::sleep_for(std::chrono::milliseconds(2));
//
//					frame2 = cvcam::Frame();
//				}
//			}
//			else
//			{
//				//if (i == 0)
//				//	emit showYuvSignal1((uchar*)frameX[i].data(0), (uchar*)frameX[i].data(1), (uchar*)frameX[i].data(2), frameX[i].width(), frameX[i].height());
//				//else
//				//	emit showYuvSignalStitch((uchar*)frameX[i].data(0), (uchar*)frameX[i].data(1), (uchar*)frameX[i].data(2), frameX[i].width(), frameX[i].height());
//
//				std::this_thread::sleep_for(std::chrono::milliseconds(1));
//			}
//		}
//	}
//
//	m_manager.stopDecoding();
//	std::this_thread::sleep_for(std::chrono::milliseconds(50));
//	//m_manager.close();
//}

void MultiLargeScreenWidget::getCoordinates(double lng, double lat)
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

void MultiLargeScreenWidget::MapReturnPixelsSlot(QString strx, QString stry)
{
	m_strScreenPixelX = strx;
	m_strScreenPixelY = stry;

	QStringList ssx = m_strScreenPixelX.split(",");
	QStringList ssy = m_strScreenPixelY.split(",");

	if (ssx.length() != ssy.length() )
	{
		//todo log error!
		return;
	}

	if (m_mapOverlay.size() != ssy.length())
	{
		//todo log error 1!
		g_logger->error("m_mapOverlay size not equal ssy.length()");
		return;
	}

	QRect rtMap = m_mapWidget->geometry();
	QMap<int, void*>::iterator it; 

	int ipos = 0;
	for (it = m_mapOverlay.begin(); it != m_mapOverlay.end(); )
	{
		int x = ssx[ipos].toInt();
		int y = ssy[ipos].toInt();
		switch (it.key())
		{
		case 0:
		{
			WidgetLabel* lbl = static_cast<WidgetLabel*>(it.value());
			if (lbl)
			{
				//rtMap
				
				QRect rt = lbl->geometry();
				if (rt.width() + x <= rtMap.width() && rt.height() + y <= rtMap.height())
				{
					lbl->move(x, y);
					lbl->show();
				}
				else
				{
					lbl->hide();
				}
			}
			else
			{
				//todo log
			}
			break;
		}
		case 1:
		{
			LabelAlarmIcon* alarmLbl = static_cast<LabelAlarmIcon*>(it.value());
			if (alarmLbl)
			{
				QRect rt = alarmLbl->geometry();
				if (rt.width() + x <= rtMap.width() && rt.height() + y <= rtMap.height())
				{
					alarmLbl->move(x, y);
					alarmLbl->show();
				}
				else
				{
					alarmLbl->hide();
				}
			}
			else
			{
				//todo log
			}
			break;
		}
		case 2:
		{
			WidgetIconLabel* iconLbl = static_cast<WidgetIconLabel*>(it.value());
			if (iconLbl)
			{
				QRect rt = iconLbl->geometry();
				if (rt.width() + x <= rtMap.width() && rt.height() + y <= rtMap.height())
				{
					iconLbl->move(x, y);
					iconLbl->show();
				}
				else
				{
					iconLbl->hide();
				}
			}
			else
			{
				//todo log
			}
			break;
		}
		default:
			break;

		}
		ipos++;
	}
}

void MultiLargeScreenWidget::getCoordinateRect(double lngs, double lats, double lnge, double late)
{
	//todo 
	printf("");
}

void MultiLargeScreenWidget::btnOnlySegSlot()
{
	m_nCurrentMaxCam = 1;
	if (m_widgetVideoSegment)
	{
		m_widgetVideoSegment->CloseAllStream();
		m_widgetVideoSegment->SetSegmentCount(1);
	}
}

void MultiLargeScreenWidget::btnFourSegSlot()
{
	m_nCurrentMaxCam = 4;
	if (m_widgetVideoSegment)
	{
		m_widgetVideoSegment->CloseAllStream();
		m_widgetVideoSegment->SetSegmentCount(4);
	}
}

void MultiLargeScreenWidget::btnNineSegSlot()
{
	m_nCurrentMaxCam = 9;
	if (m_widgetVideoSegment)
	{
		m_widgetVideoSegment->CloseAllStream();
		m_widgetVideoSegment->SetSegmentCount(9);
	}
}

void MultiLargeScreenWidget::btnGlobalSlot()
{
	QRect rt = ui.widgetStitchTitle->geometry();//(QWidget*)(ui.btnGlobal->parent())->geometry();
	if (m_lblExpandGlobal->isHidden())
	{
		m_lblExpandGlobal->move((rt.width() - rt.x()) / 4.0 - 14, rt.height() + rt.y() + 15);
		m_lblExpandGlobal->raise();

		m_lblExpandGlobal->show();

		m_widgetGlobal->move(18, rt.height() + rt.y() + 35);
		m_widgetGlobal->show();
	}
	else
	{
		m_lblExpandGlobal->hide();
		m_widgetGlobal->hide();
	}
}

void MultiLargeScreenWidget::btnLabelsSlot()
{
	QRect rt = ui.widgetStitchTitle->geometry();

	if (m_lblExpandLabels->isHidden())
	{
		m_lblExpandLabels->raise();
		m_lblExpandLabels->move((rt.width() - rt.x()) / 4.0 * 3 - 14, rt.height() + rt.y() + 15);
		m_lblExpandLabels->show();

		m_widgetTarget->move((rt.width() - rt.x()) / 2.0, rt.height() + rt.y() + 35);
		m_widgetTarget->show();
	}
	else
	{
		m_lblExpandLabels->hide();
		m_widgetTarget->hide();
	}
}

void MultiLargeScreenWidget::btnChangeUserSlot()
{
	QRect rt = ui.widget_3->geometry();
	QRect rt1 = ui.horizontalSpacer_2->geometry();
	QRect rt2 = ui.verticalSpacer_9->geometry();
	if (m_changeUserWidget->isHidden())
	{
		m_changeUserWidget->setParent(this);
		m_changeUserWidget->move(rt.x() + rt2.x(), rt.height() - rt2.height() + 10);
		m_changeUserWidget->raise();
		m_changeUserWidget->show();
	}
	else
		m_changeUserWidget->hide();
}

void MultiLargeScreenWidget::changeEvent(QEvent* e)
{
	if (e->type() == QEvent::WindowStateChange)
	{
		QWindowStateChangeEvent* ev = static_cast<QWindowStateChangeEvent*>(e);
		if (!(ev->oldState() & Qt::WindowMaximized) && windowState() & Qt::WindowMaximized)
		{
			qDebug() << "MainWindow::changeEvent geometry() = " << geometry();

			timer.start();
		}

		if (!(ev->oldState() & Qt::WindowFullScreen) && windowState() & Qt::WindowFullScreen)
		{
			qDebug() << "MainWindow::changeEvent geometry() = " << geometry();

			timer.start();
		}
	}
	QWidget::changeEvent(e);
}

void MultiLargeScreenWidget::resizeEvent(QResizeEvent *event)
{
	if (!m_blResize)
	{
		return;
	}

	ResizeUi();
}

void MultiLargeScreenWidget::logout()
{
	//
	saveUiStatus();
	m_strUser = "guest";
	return;
}

void MultiLargeScreenWidget::changeUser()
{
	//m_loginWidget->showFullScreen();
	m_loginWidget->show();
	m_loginWidget->raise();
}

void MultiLargeScreenWidget::ResizeUi()
{
	QRect rt = ui.widgetMap->geometry();
	qDebug() << rt;
	m_mapWidget->setGeometry(rt);
	m_mapWidget->move(0, 0);

	//QRect refRect = ui.widgetCam1->geometry();

	//m_widgetCams[0]->setGeometry(refRect.x(), refRect.y(), refRect.width() - 2, refRect.height() - 8);
//	m_widgetCams[0]->raise();

	//m_widgetCams[0]->setGeometry(ui.widgetCam1->geometry());
	//m_widgetCams[1]->setGeometry(ui.widgetCam1->geometry());
	//m_widgetCams[2]->setGeometry(ui.widgetCam1->geometry());
	//m_widgetCams[3]->setGeometry(ui.widgetCam1->geometry());
	//m_widgetCams[0]->show();
	//m_widgetCams[1]->show();
	//m_widgetCams[2]->show();
	//m_widgetCams[3]->show();

	//todo ??? resize late?

	ui.lblControlPanel->ResizeUI();
	
	//m_widgetAlarm->setGeometry(ui.widgetCam4->geometry());
	//m_widgetAlarm->move(0, 0);
	//m_widgetAlarm->show();

	//QRect refRectRef = ui.widgetRef->geometry();
	//m_lblRef->setGeometry(0, 0, refRectRef.width(), refRectRef.height());
	//m_lblRef->raise();
	//m_lblRef->show();

	QRect rtCtl = ui.widget_CtlPnl->geometry();
	ui.lblControlPanel->ResizeUI(QSize(qMin(rtCtl.width(), rtCtl.height()), qMin(rtCtl.width(), rtCtl.height())));
	ui.lblControlPanel->move(10, 0);

	ui.listWidget->setGeometry(qMin(rtCtl.width(), rtCtl.height()) + 10, 0, rtCtl.width() - qMin(rtCtl.width(), rtCtl.height()) - 22, rtCtl.height());

	if (m_widgetVideoSegment)
		m_widgetVideoSegment->ResizeUI(ui.widgetVideos->geometry());
}

#if 0
bool MultiLargeScreenWidget::isTopItem(QTreeWidgetItem* item)
{
	if (!item) 
		return false;
	if (!item->parent())
		return true;
	return false;
}

void MultiLargeScreenWidget::setChildCheckState(QTreeWidgetItem *item, Qt::CheckState cs)
{
	if (!item) 
		return;	
	for (int i = 0; i < item->childCount(); i++)
	{
		QTreeWidgetItem* child = item->child(i);
		if (child->checkState(0) != cs)
		{
			child->setCheckState(0, cs);

			emit checkStateChange()
		}
	}
	setParentCheckState(item->parent());
}

void MultiLargeScreenWidget::setParentCheckState(QTreeWidgetItem *item)
{
	if (!item) return;	
	int selectedCount = 0;	
	int childCount = item->childCount();
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

QTreeWidgetItem* MultiLargeScreenWidget::FindtreeViewItem(QTreeWidgetItem *treeWidgetItem, GM_DEVICEGROUP_STRU* mapStrus)
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

QTreeWidgetItem* MultiLargeScreenWidget::GettreeViewItem(QTreeWidget *treeWidget, QMap<int, PGM_DEVICEGROUP_STRU> mapStrus, int nId)
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

void MultiLargeScreenWidget::AddDevicesTotreeView(QTreeWidget *treeWidget, const QStringList *header, QMap<int, PGM_DEVICEGROUP_STRU> mapStrus)
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

#ifdef __DATE_2020321_NO_ORG__
void MultiLargeScreenWidget::TransDataTarget()
{
	QMap<int, GM_DEVICEGROUP_STRU> mapStrus = m_mapTarget;
	GM_DEVICEGROUP_STRU p1;

	int i = 1;

	mapStrus.clear();
	std::map<QString, std::vector<STRU_DB_LABEL> >::iterator itlbls;
	if (m_mapLabels.size() == 0)
		return;

	itlbls = m_mapLabels.find(m_szSceneId);
	if (itlbls == m_mapLabels.end())
	{
		//
		qDebug() << " not found device=sceneid " << m_szSceneId;
		return;
	}
	std::vector<STRU_DB_LABEL> lbls = itlbls->second;

	//取type
	QMap<QString, QString> mapTypes;
	QMap<QString, int> mapParent;

//#if 0
//	for (STRU_DB_LABEL & it : lbls)
//	{
//		int ii = Match_DeviceType(it.flagType);
//		if (ii > 2)
//			mapTypes[it.flagType] = C_DEVICETYPE[ii][0];
//	}
//#else
//	for (STRU_DB_LABEL & it : lbls)
//	{
//		mapTypes[it.flagType] = it.flagType;
//	}
//#endif
#if 0
	for (STRU_DB_LABEL & it : lbls)
	{
		int ii = Match_DeviceType(it.flagType);
		if (ii >= 0 && ii <= 2)
			mapTypes[it.flagType] = C_DEVICETYPE[ii][0];
	}
#else
	//20220318 修改接口
	QString sz1 = QString::fromUtf8("重要目标");
	for (STRU_DB_LABELTYPE & itgroup : m_vLabelGroup)
	{
		if (itgroup.typeGroupname.toUtf8() == sz1)
			mapTypes[itgroup.flagName] = itgroup.flagName;
	}

#endif

	for (auto it = mapTypes.begin(); it != mapTypes.end(); it++)
	{
		p1.deviceautoId = i;
		p1.cameraid = it.key();
		p1.nodeName = it.value();
		p1.deviceautoParent = -1;
		p1.roleType = 0;
		p1.nodeType = 0;
		p1.deviceautoParent = 0;
		p1.nodeId = QString::number(p1.deviceautoId);
		mapStrus[p1.deviceautoId] = p1;
		mapParent[p1.cameraid] = i;
		i++;
	}

	for (STRU_DB_LABEL & it : lbls)
	{
		p1.deviceautoId = i;
		p1.cameraid = it.labelId;
		p1.nodeName = it.name;
		p1.deviceautoParent = -1;
		p1.roleType = 0;

		int id = -1;
#if 0
		int id = Match_DeviceType(it.flagType);
		if (id != -1)
		{
			p1.nodeType = 1;
			QMap<QString, int>::Iterator itid;
			itid = mapParent.find(it.flagType);
			if (itid != mapParent.end())
			{
				p1.deviceautoParent = itid.value();
			}
		}
#else
		id = Match_DeviceType(it.flagType);
		if (id != -1)
		{
			QMap<QString, int>::Iterator itid;
			itid = mapParent.find(it.flagType);
			if (itid != mapParent.end())
			{
				p1.deviceautoParent = itid.value();
				p1.nodeType = 1;
			}
		}
#endif		
		if (id == -1)
		{
			//p1.nodeType = 1000;
			p1.devicetype = 3;
		}
		else if (id == 0)
		{//全景
			p1.devicetype = 101;
			//p1.devicetype = id;
		}
		else if (id == 1)
		{//球机
			p1.devicetype = 102;
			//p1.devicetype = id;
		}
		else if (id == 3)
		{//枪机
			p1.devicetype = 103;
			//p1.devicetype = id;
		}
		else if (id == 4)
		{//建筑
			p1.devicetype = 201;
			//p1.devicetype = id;
		}
		else if (id >= 5 && id <= 8)
		{//建筑 其他
			p1.devicetype = 202;
			//p1.devicetype = id;
		}
		else
		{  //其他类型
			p1.devicetype = 1000;
			//p1.devicetype = id;
		}

		//没有图标 采用cameraid 作为内部标识
		
		p1.parentId = it.parentId;
		p1.nodeId = QString::number(p1.deviceautoId);
		mapStrus[p1.deviceautoId] = p1;

		//接口提供数据结构时间复杂度为O(n2) 内部简化为O(n) * 2....
		//内部处理 树问题 
		//m_mapParentTarget[p1.cameraid] = i;
		i++;
	}
	m_mapTarget = mapStrus;
	return;

#if 0
	p1.cameraid = "1";
	strcpy(p1.nodeName, "居民社区");
	p1.nodeType = 0;
	p1.parentId = "0";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "2";
	strcpy(p1.nodeName, "亚奥人家小区");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "3";
	strcpy(p1.nodeName, "春秋家园小区");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "4";
	strcpy(p1.nodeName, "西门子集团公寓");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "5";
	strcpy(p1.nodeName, "标注建筑");
	p1.nodeType = 0;
	p1.devicetype = 1000;
	p1.parentId = "0";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = 6;
	strcpy(p1.nodeName, "盘古大观");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = 5;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "7";
	strcpy(p1.nodeName, "国家体育场(鸟巢)");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "5";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "8";
	strcpy(p1.nodeName, "水立方");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "5";
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "9";
	strcpy(p1.nodeName, "餐饮旅游");
	p1.nodeType = 0;
	p1.devicetype = 1000;
	p1.parentId = "0";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "10";
	strcpy(p1.nodeName, "中华第一玉");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "9";
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "11";
	strcpy(p1.nodeName, "北京奥林匹克公园");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "9";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "12";
	strcpy(p1.nodeName, "聚福园自助餐厅");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "9";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;
#endif
	//m_mapTarget = mapStrus;
}
#else
//2022 03 21 组结构没有 先不处理
void MultiLargeScreenWidget::TransDataTargetChild(QString parentid, int &i)
{
	GM_DEVICEGROUP_STRU p1;

	for (auto it = m_mapLabels.begin(); it != m_mapLabels.end(); it++)
	{
		if (it->second.parentId != parentid)
			continue;

		p1.deviceautoId = i;
		p1.cameraid = it->second.labelId;
		p1.nodeName = it->second.name;
		p1.deviceautoParent = -1;
		p1.nodeType = 1;
		p1.roleType = 0;
		if (it->second.name == QString::fromLocal8Bit("重要目标")) //m_szSceneId)
		{
			p1.nodeType = 0;
			p1.deviceautoParent = 0;
		}
		else if (it->second.parentId != "" && it->second.parentName == QString::fromLocal8Bit("重要目标"))
		{
			p1.nodeType = 1;
			QMap<QString, int>::Iterator itid;
			itid = m_mapParentTarget.find(it->second.parentId);
			if (itid != m_mapParentTarget.end())
			{
				p1.deviceautoParent = itid.value();
			}
		}
		if (it->second.flagType == "")
		{
			p1.devicetype = 100;
		}
		else if (it->second.flagType != "")
		{
			p1.devicetype = 2;
		}
		p1.parentId = it->second.parentId;
		p1.nodeId = QString(p1.deviceautoId);

		m_mapTarget[p1.deviceautoId] = p1;

		m_mapParentTarget[p1.cameraid] = i;
		i++;

		TransDataTargetChild(p1.cameraid, i);
	}
}

void MultiLargeScreenWidget::TransDataCameraChild(QString parentid, int &i)
{
	GM_DEVICEGROUP_STRU p1;

	for (auto it = m_mapLabels.begin(); it != m_mapLabels.end(); it++)
	{
		if (it->second.parentId != parentid)
			continue;

		p1.deviceautoId = i;
		p1.cameraid = it->second.labelId;
		p1.nodeName = it->second.name;
		p1.deviceautoParent = -1;
		p1.nodeType = 1;
		p1.roleType = 0;
		if (it->second.name == QString::fromLocal8Bit("监控相机"))//m_szSceneId)
		{
			p1.nodeType = 0;
			p1.deviceautoParent = 0;
		}
		else if (it->second.parentId != "" && it->second.parentName == QString::fromLocal8Bit("监控相机"))
		{
			p1.nodeType = 1;
			QMap<QString, int>::Iterator itid;
			itid = m_mapParentTarget.find(it->second.parentId);
			if (itid != m_mapParentTarget.end())
			{
				p1.deviceautoParent = itid.value();
			}
		}
		if (it->second.flagType == "")
		{
			p1.devicetype = 100;
		}
		else if (it->second.flagType != "")
		{
			p1.devicetype = 2;
		}
		p1.parentId = it->second.parentId;
		p1.nodeId = QString(p1.deviceautoId);

		m_mapCameras[p1.deviceautoId] = p1;

		m_mapParentCamera[p1.cameraid] = i;
		i++;

		TransDataCameraChild(p1.cameraid, i);
	}
}

void MultiLargeScreenWidget::TransDataTarget()
{
	//QMap<int, GM_DEVICEGROUP_STRU> mapStrus = m_mapTarget;
	GM_DEVICEGROUP_STRU p1;
	//std::map<QString, STRU_DB_DEVICE>::iterator it ;
	auto it1 = m_mapLabels.find(m_szSceneId);
	//std::map<QString, STRU_DB_DEVICE>::iterator it = m_mapLabels.find(m_szSceneId);
	if (it1 == m_mapLabels.end())
	{
		qDebug() << "not found sceneid=" << m_szSceneId;
		return;
	}

	STRU_DB_DEVICE* root = (STRU_DB_DEVICE*)(&it1->second);
	int i = 1;

	m_mapTarget.clear();
	m_mapParentTarget.clear();
	TransDataTargetChild(m_szSceneId, i);

	//for (auto it = m_mapLabels.begin(); it != m_mapLabels.end(); it++)
	//{
	//	p1.deviceautoId = i;
	//	p1.cameraid = it->second.labelId;
	//	p1.nodeName = it->second.name;
	//	p1.deviceautoParent = -1;
	//	p1.nodeType = 1;
	//	p1.roleType = 1;
	//	if (it->second.parentId == "")
	//	{
	//		p1.nodeType = 0;
	//		p1.deviceautoParent = 0;
	//	}
	//	else if (it->second.parentId != "")
	//	{
	//		p1.nodeType = 1;
	//		QMap<QString, int>::Iterator itid;
	//		itid = m_mapParentTarget.find(it->second.parentId);
	//		if (itid != m_mapParentTarget.end())
	//		{
	//			p1.deviceautoParent = itid.value();
	//		}
	//	}
	//	if (it->second.flagType == "")
	//	{
	//		p1.devicetype = 100;
	//	}
	//	else if (it->second.flagType != "")
	//	{
	//		p1.devicetype = 2;
	//	}
	//	p1.parentId = it->second.parentId;
	//	p1.nodeId = QString(p1.deviceautoId);

	//	m_mapTarget[p1.deviceautoId] = p1;

	//		//接口提供数据结构时间复杂度为O(n2) 内部简化为O(n) * 2....
	//		//内部处理 树问题 
	//	m_mapParentTarget[p1.cameraid] = i;
	//	i++;
	//}
	//m_mapTarget = mapStrus;
	return;

//	QMap<int, GM_DEVICEGROUP_STRU> mapStrus = m_mapTarget;
//	GM_DEVICEGROUP_STRU p1;
//
//	int i = 1;
//
//	mapStrus.clear();
//	std::map<QString, std::vector<STRU_DB_LABEL> >::iterator itlbls;
//	if (m_mapLabels.size() == 0)
//		return;
//
//	itlbls = m_mapLabels.find(m_szSceneId);
//	if (itlbls == m_mapLabels.end())
//	{
//		//
//		qDebug() << " not found device=sceneid " << m_szSceneId;
//		return;
//	}
//	std::vector<STRU_DB_LABEL> lbls = itlbls->second;
//
//	//取type
//	QMap<QString, QString> mapTypes;
//	QMap<QString, int> mapParent;
//
//	//#if 0
//	//	for (STRU_DB_LABEL & it : lbls)
//	//	{
//	//		int ii = Match_DeviceType(it.flagType);
//	//		if (ii > 2)
//	//			mapTypes[it.flagType] = C_DEVICETYPE[ii][0];
//	//	}
//	//#else
//	//	for (STRU_DB_LABEL & it : lbls)
//	//	{
//	//		mapTypes[it.flagType] = it.flagType;
//	//	}
//	//#endif
//#if 0
//	for (STRU_DB_LABEL & it : lbls)
//	{
//		int ii = Match_DeviceType(it.flagType);
//		if (ii >= 0 && ii <= 2)
//			mapTypes[it.flagType] = C_DEVICETYPE[ii][0];
//	}
//#else
//	//20220318 修改接口
//	QString sz1 = QString::fromUtf8("重要目标");
//	for (STRU_DB_LABELTYPE & itgroup : m_vLabelGroup)
//	{
//		if (itgroup.typeGroupname.toUtf8() == sz1)
//			mapTypes[itgroup.flagName] = itgroup.flagName;
//	}
//
//#endif
//
//	for (auto it = mapTypes.begin(); it != mapTypes.end(); it++)
//	{
//		p1.deviceautoId = i;
//		p1.cameraid = it.key();
//		p1.nodeName = it.value();
//		p1.deviceautoParent = -1;
//		p1.roleType = 0;
//		p1.nodeType = 0;
//		p1.deviceautoParent = 0;
//		p1.nodeId = QString::number(p1.deviceautoId);
//		mapStrus[p1.deviceautoId] = p1;
//		mapParent[p1.cameraid] = i;
//		i++;
//	}
//
//	for (STRU_DB_LABEL & it : lbls)
//	{
//		p1.deviceautoId = i;
//		p1.cameraid = it.labelId;
//		p1.nodeName = it.name;
//		p1.deviceautoParent = -1;
//		p1.roleType = 0;
//
//		int id = -1;
//#if 0
//		int id = Match_DeviceType(it.flagType);
//		if (id != -1)
//		{
//			p1.nodeType = 1;
//			QMap<QString, int>::Iterator itid;
//			itid = mapParent.find(it.flagType);
//			if (itid != mapParent.end())
//			{
//				p1.deviceautoParent = itid.value();
//			}
//		}
//#else
//		id = Match_DeviceType(it.flagType);
//		if (id != -1)
//		{
//			QMap<QString, int>::Iterator itid;
//			itid = mapParent.find(it.flagType);
//			if (itid != mapParent.end())
//			{
//				p1.deviceautoParent = itid.value();
//				p1.nodeType = 1;
//			}
//		}
//#endif		
//		if (id == -1)
//		{
//			//p1.nodeType = 1000;
//			p1.devicetype = 3;
//		}
//		else if (id == 0)
//		{//全景
//			p1.devicetype = 101;
//			//p1.devicetype = id;
//		}
//		else if (id == 1)
//		{//球机
//			p1.devicetype = 102;
//			//p1.devicetype = id;
//		}
//		else if (id == 3)
//		{//枪机
//			p1.devicetype = 103;
//			//p1.devicetype = id;
//		}
//		else if (id == 4)
//		{//建筑
//			p1.devicetype = 201;
//			//p1.devicetype = id;
//		}
//		else if (id >= 5 && id <= 8)
//		{//建筑 其他
//			p1.devicetype = 202;
//			//p1.devicetype = id;
//		}
//		else
//		{  //其他类型
//			p1.devicetype = 1000;
//			//p1.devicetype = id;
//		}
//
//		//没有图标 采用cameraid 作为内部标识
//
//		p1.parentId = it.parentId;
//		p1.nodeId = QString::number(p1.deviceautoId);
//		mapStrus[p1.deviceautoId] = p1;
//
//		//接口提供数据结构时间复杂度为O(n2) 内部简化为O(n) * 2....
//		//内部处理 树问题 
//		//m_mapParentTarget[p1.cameraid] = i;
//		i++;
//	}
//	m_mapTarget = mapStrus;
//	return;

}
#endif
#if 0
void MultiLargeScreenWidget::TransDataCameras()
{
	//设备
	QMap<int, GM_DEVICEGROUP_STRU> mapStrus = m_mapCameraGroup;
	GM_DEVICEGROUP_STRU p1;

	int i = 1000;

	std::map<QString, STRU_DB_DEVICE>::iterator it;// m_mapDevices
	for (it = m_mapDevices.begin(); it != m_mapDevices.end(); it++)
	//for (STRU_DB_ORGANIZATION & it : m_vCameraGroup)
	{
		//PSTRU_DB_DEVICE node = &it.value();
		p1.deviceautoId = i;
		p1.cameraid = it->second.deviceId;
		p1.nodeName = it->second.name;
		p1.deviceautoParent = -1;
		p1.roleType = 1;
		if (it->second.orgId == "")
		{
			p1.nodeType = 0;
			p1.deviceautoParent = 0;
		}
		else if (it->second.orgId != "")
		{
			p1.nodeType = 1;
			QMap<QString, int>::Iterator itid;
			itid = m_mapParent.find(it->second.orgId);
			if (itid != m_mapParent.end())
			{
				p1.deviceautoParent = itid.value();
			}
		}

		QString sztype;
		int id = Match_DeviceTypeName(it->second.type, sztype);

		if (id == -1)
		{
			//p1.nodeType = 1000;
			p1.devicetype = 3;
		}
		else if (id == 0)
		{//全景
			p1.devicetype = 101;
			//p1.devicetype = id;
		}
		else if (id == 1)
		{//球机
			p1.devicetype = 102;
			//p1.devicetype = id;
		}
		else if (id == 3)
		{//枪机
			p1.devicetype = 103;
			//p1.devicetype = id;
		}
		else if (id == 4)
		{//建筑
			p1.devicetype = 201;
			//p1.devicetype = id;
		}
		else if (id >= 5 && id <= 8)
		{//建筑 其他
			p1.devicetype = 202;
			//p1.devicetype = id;
		}
		else
		{  //其他类型
			p1.devicetype = 1000;
			//p1.devicetype = id;
		}
		
		p1.parentId = it->second.orgId;
		p1.nodeId = QString::number(p1.deviceautoId);

		p1.url = it->second.rtspUrl;
		p1.urlSub = it->second.rtspSubUrl;

		mapStrus[p1.deviceautoId] = p1;

		m_mapParent[p1.cameraid] = i;
		i++;
	}

	m_mapCameraGroup = mapStrus;

#if 0
	p1.cameraid = "1";
	strcpy(p1.nodeName, "高点相机");
	p1.nodeType = 0;
	p1.parentId = "0";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "2";
	strcpy(p1.nodeName, "唐人街购物中心");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "3";
	strcpy(p1.nodeName, "国家体育场南路");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "4";
	strcpy(p1.nodeName, "北四环中路辅路");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "41";
	strcpy(p1.nodeName, "北四环中路");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "5";
	strcpy(p1.nodeName, "低点相机");
	p1.nodeType = 0;
	p1.devicetype = 1000;
	p1.parentId = "0";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "6";
	strcpy(p1.nodeName, "天辰西路");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "5";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "7";
	strcpy(p1.nodeName, "春笋智能洗车");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "5";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "8";
	strcpy(p1.nodeName, "售票处");
	p1.nodeType = 1;
	p1.devicetype = 1000;
	p1.parentId = "5";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;
#endif
	//m_mapCameras = mapStrus;
}

void MultiLargeScreenWidget::TransDataCameras()
{
	QMap<int, GM_DEVICEGROUP_STRU> mapStrus = m_mapCameras;
	GM_DEVICEGROUP_STRU p1;

	int i = 1;
	mapStrus.clear();
	std::map<QString, STRU_DB_LABEL >::iterator itlbls;
	if (m_mapLabels.size() == 0)
		return;

	//itlbls = m_mapLabels.find(m_szSceneId);
	//if (itlbls == m_mapLabels.end())
	//{
	//	//
	//	qDebug() << " not found device=sceneid " << m_szSceneId;
	//	return;
	//}
	STRU_DB_LABEL& lbls = itlbls->second;

	//取type
	//QList<QMap<QString, QString>  > lsmapTypes;
	QMap<QString, QString> mapTypes;
	QMap<QString, int> mapParent;
#if 0
	for (STRU_DB_LABEL & it : lbls)
	{
		int ii = Match_DeviceType(it.flagType);
		if (ii >= 0 && ii <= 2)
			mapTypes[it.flagType] = C_DEVICETYPE[ii][0];
	}
#else
	//20220318 修改接口
	//QString sz = QStringLiteral("监控相机");
	QString sz1 = QString::fromUtf8("监控相机");
	for (STRU_DB_LABELTYPE & itgroup : m_vLabelGroup)
	{
		if (itgroup.typeGroupname.toUtf8() == sz1)
			mapTypes[itgroup.flagName] = itgroup.flagName;
	}

#endif

	//for (auto it = mapTypes.begin(); it != mapTypes.end(); it++)
	//{
	//	p1.deviceautoId = i;
	//	p1.cameraid = it.key();
	//	p1.nodeName = it.value();
	//	p1.deviceautoParent = -1;
	//	p1.roleType = 0;
	//	p1.nodeType = 0;
	//	p1.deviceautoParent = 0;
	//	p1.nodeId = QString::number(p1.deviceautoId);
	//	mapStrus[p1.deviceautoId] = p1;
	//	mapParent[p1.cameraid] = i;
	//	i++;
	//}

	//for (STRU_DB_LABEL & it : lbls)
	//{
	//	p1.deviceautoId = i;
	//	p1.cameraid = it.labelId;
	//	p1.nodeName = it.name;
	//	p1.deviceautoParent = -1;
	//	p1.roleType = 0;

	//	int id = Match_DeviceType(it.flagType);
	//	if (id != -1)
	//	{
	//		p1.nodeType = 1;
	//		QMap<QString, int>::Iterator itid;
	//		itid = mapParent.find(it.flagType);
	//		if (itid != mapParent.end())
	//		{
	//			p1.deviceautoParent = itid.value();
	//		}
	//	}

	//	if (id == -1)
	//	{
	//		//p1.nodeType = 1000;
	//		p1.devicetype = 3;
	//	}
	//	else if (id == 0)
	//	{//全景
	//		p1.devicetype = 101;
	//		//p1.devicetype = id;
	//	}
	//	else if (id == 1)
	//	{//球机
	//		p1.devicetype = 102;
	//		//p1.devicetype = id;
	//	}
	//	else if (id == 3)
	//	{//枪机
	//		p1.devicetype = 103;
	//		//p1.devicetype = id;
	//	}
	//	else if (id == 4)
	//	{//建筑
	//		p1.devicetype = 201;
	//		//p1.devicetype = id;
	//	}
	//	else if (id >= 5 && id <= 8)
	//	{//建筑 其他
	//		p1.devicetype = 202;
	//		//p1.devicetype = id;
	//	}
	//	else
	//	{  //其他类型
	//		p1.devicetype = 1000;
	//		//p1.devicetype = id;
	//	}

	//	//没有图标 采用cameraid 作为内部标识

	//	p1.parentId = it.parentId;
	//	p1.nodeId = QString::number(p1.deviceautoId);
	//	mapStrus[p1.deviceautoId] = p1;

	//	//接口提供数据结构时间复杂度为O(n2) 内部简化为O(n) * 2....
	//	//内部处理 树问题 
	//	//m_mapParentTarget[p1.cameraid] = i;
	//	i++;
	//}

	//m_mapCameras = mapStrus;
	return;
}
#else
void MultiLargeScreenWidget::TransDataCameras()
{
	QMap<int, GM_DEVICEGROUP_STRU> mapStrus = m_mapCameras;
	GM_DEVICEGROUP_STRU p1;
	//std::map<QString, STRU_DB_DEVICE>::iterator it ;
	auto it1 = m_mapLabels.find(m_szSceneId);
	//std::map<QString, STRU_DB_DEVICE>::iterator it = m_mapLabels.find(m_szSceneId);
	if (it1 == m_mapLabels.end())
	{
		qDebug() << "not found sceneid=" << m_szSceneId;
		return;
	}

	STRU_DB_DEVICE* root = (STRU_DB_DEVICE*)(&it1->second);
	int i = 1;

	m_mapCameras.clear();
	m_mapParentCamera.clear();
	TransDataCameraChild(m_szSceneId, i);
}
#endif

void MultiLargeScreenWidget::TransDataCameraGroup()
{
	QMap<int, GM_DEVICEGROUP_STRU> mapStrus = m_mapCameraGroup;
	GM_DEVICEGROUP_STRU p1;
	
	int i = 1;
	for (STRU_DB_ORGANIZATION & it : m_vCameraGroup)
	{
		p1.deviceautoId = i;
		p1.cameraid = it.uuid;
		p1.nodeName = it.name;
		p1.deviceautoParent = -1;
		p1.nodeType = 1;
		p1.roleType = 1;
		if (it.parentId == "")
		{
			p1.nodeType = 0;
			p1.deviceautoParent = 0;
		}
		else if (it.parentId != "")
		{
			p1.nodeType = 1;
			QMap<QString, int>::Iterator itid;
			itid = m_mapParent.find(it.parentId);
			if (itid != m_mapParent.end())
			{
				p1.deviceautoParent = itid.value();
			}
		}
		if (it.type == "")
		{
			p1.devicetype = 100;
		}
		else if (it.type != "")
		{
			p1.devicetype = 2;
		}
		p1.parentId = it.parentId;
		p1.nodeId = QString(p1.deviceautoId);
		mapStrus[p1.deviceautoId] = p1;

		//接口提供数据结构时间复杂度为O(n2) 内部简化为O(n) * 2....
		//内部处理 树问题 
		m_mapParent[p1.cameraid] = i;
		i++;
	}
	m_mapCameraGroup = mapStrus;
	return;

#if  0

	p1.cameraid = "2";
	strcpy(p1.nodeName, "新华大厦1区");
	p1.nodeType = 9;
	p1.devicetype = 3;
	p1.parentId = "1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "3";
	strcpy(p1.nodeName, "新华大厦1区102");
	p1.nodeType = 9;
	p1.devicetype = 1;
	p1.parentId = "2";
	p1.nodeId = p1.cameraid;
	p1.url = "rtsp://192.168.8.101/drone/12/stream1";
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "4";
	strcpy(p1.nodeName, "新华大厦1区104");
	p1.nodeType = 9;
	p1.devicetype = 1;
	p1.parentId = "2";
	p1.url = "rtsp://192.168.8.101/drone/14/stream1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "5";
	strcpy(p1.nodeName, "新华大厦3区");
	p1.nodeType = 9;
	p1.devicetype = 1;
	p1.parentId = "1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "6";
	strcpy(p1.nodeName, "新华大厦3区301");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = "5";
	p1.url = "rtsp://192.168.8.101/drone/31/stream1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "7";
	strcpy(p1.nodeName, "新华大厦3区302");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = "5";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "8";
	strcpy(p1.nodeName, "新华大厦3区309");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = "5";
	p1.url = "rtsp://192.168.8.101/drone/32/stream1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "9";
	strcpy(p1.nodeName, "新华大厦3区309-1");
	p1.nodeType = 9;
	p1.devicetype = 2;
	p1.parentId = "8";
	p1.url = "rtsp://192.168.8.101/drone/33/stream1";
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "10";
	strcpy(p1.nodeName, "新华大厦3区312");
	p1.nodeType = 9;
	p1.devicetype = 3;
	p1.parentId = "5";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;

	p1.cameraid = "11";
	strcpy(p1.nodeName, "新华大厦3区315A 3区315A 3区315A");
	p1.nodeType = 9;
	p1.devicetype = 4;
	p1.parentId = "5";
	p1.url = "rtsp://192.168.8.101/drone/35/stream1";
	p1.nodeId = p1.cameraid;
	mapStrus[p1.cameraid.c_str()] = p1;
#endif
}

void MultiLargeScreenWidget::treeCamItemDoubleClickedSlot(QTreeWidgetItem* item, int column)
{
	m_nNowCam++;
	if (m_nNowCam > m_nCurrentMaxCam)
		m_nNowCam = 0;

	int nid1 = item->data(0, Qt::UserRole).toInt();
	
	QMap<int, GM_DEVICEGROUP_STRU>::Iterator it;
	it = m_mapCameraGroup.find(nid1);
	
	if (it != m_mapCameraGroup.end())
	{
#ifndef __DEFINE_CYCLE__
		STRU_DB_STREAMURL pInfo;
		pInfo.deviceId = it->cameraid;
		bool bl = m_svrInterface->GetDeviceStreamUrl(it->cameraid, "1", pInfo);
		if (bl)
			m_widgetVideoSegment->OpenStreamSlot(pInfo.deviceId, m_widgetVideoSegment->GetCurrentPos(), pInfo.rtsp.toStdString(), it->nodeName, true);
		else
			qDebug() << " GetDeviceStreamUrl error " << it->cameraid;
#else
		if (it->url != "")
			m_widgetVideoSegment->OpenStreamSlot(m_widgetVideoSegment->GetCurrentPos(), it->url.toUtf8().toStdString(), true);
#endif
	}
	else
	{
		//todo log
	}
}

void MultiLargeScreenWidget::treeCamItemChangedSlot(QTreeWidgetItem* item, int column)
{
	//disconnect(ui.treeWidgetCam, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeCamItemChangedSlot(QTreeWidgetItem*, int)));
	//setChildCheckState(item, item->checkState(0));
	//setParentCheckState(item);
	//connect(ui.treeWidgetCam, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(treeCamItemChangedSlot(QTreeWidgetItem*, int)), Qt::UniqueConnection);
}

//void MultiLargeScreenWidget::SaveLocalUIStatus(QString szScene)
//{
//	//QJsonParseError jsonError;
//	//QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);
//
//	//QJsonValue value;
//	//if (jsonError.error != QJsonParseError::NoError)
//	//{
//	//	return false;
//	//}
//
//	//if (!json.isObject())
//	//{
//	//	return false;
//	//}
//	//QJsonObject rootObj = json.object();
//
//}
//
//void MultiLargeScreenWidget::LoadLocalUIStatus(QString szScene)
//{
//}

void MultiLargeScreenWidget::closeEvent(QCloseEvent *event)
{
	saveUiStatus();
}

void MultiLargeScreenWidget::ChangeSceneSlot(QString sceneId)
{
	//1 保存配置
	saveUiStatus();

	//保留当前的scene
	m_stLocalStatus.sceneId = sceneId;

	STRU_DB_STREAMURL pInfo;
	if (!m_svrInterface->GetDeviceStreamUrl(sceneId, "1", pInfo))
	{
		qDebug() << " get device steamurl error" << sceneId;
		//return;
	}

	m_szSceneId = sceneId;
	QString szname = "";
	for (auto & it : m_lsScene)
	{
		if (m_szSceneId == it.deviceId)
		{
			if (!m_config->mapInf.dynamicCenterMap)
			{
				//切换地图
				if (it.longtitude >= -180.0 && it.longtitude <= 180.0 && it.latitude >= -90.0 && it.latitude <= 90.0)
					m_mapWidget->CenterPoint(it.latitude, it.longtitude, 19);
				else
				{
					qDebug() << " not found  gps info " << sceneId;
					//return;
				}
			}
			TransDataTarget();
			TransDataCameras();
			QStringList ls;
			szname = it.name;

			m_widgetTarget->AddList(&ls, m_widgetTarget->m_treeWidgetTarget, m_mapTarget);
			m_widgetTarget->AddList(&ls, m_widgetTarget->m_treeWidgetCamera, m_mapCameras);

			m_widgetTarget->showTargetListSlot();
		}
	}

	for (int j = 0; j < m_widgetGlobal->ui.listWidget->count(); j++)
	{
		QString itemPath = m_widgetGlobal->ui.listWidget->item(j)->text();
		if (itemPath == szname)
		{
			m_widgetGlobal->ui.listWidget->item(j)->setSelected(true);
			break;
		}
	}
	
	QList<LabelAlarmIcon *> lsAlarmLbl = this->findChildren<LabelAlarmIcon *>();
	for (int i = 0; i < lsAlarmLbl.size(); i++)
	{
		LabelAlarmIcon * lbl = lsAlarmLbl.at(i);
		lbl->hide();
	}

	QList<WidgetDetailInfo *> lsDetail = this->findChildren<WidgetDetailInfo *>();
	for (int i = 0; i < lsDetail.size(); i++)
	{
		WidgetDetailInfo * lbl = lsDetail.at(i);
		lbl->hide();
	}

	QList<WidgetDetailInfoAlarm *> lsDetailAlarm = this->findChildren<WidgetDetailInfoAlarm *>();
	for (int i = 0; i < lsDetailAlarm.size(); i++)
	{
		WidgetDetailInfoAlarm * lbl = lsDetailAlarm.at(i);
		lbl->hide();
	}

	QList<WidgetIconLabel *> lsIconLbl = this->findChildren<WidgetIconLabel *>();
	for (int i = 0; i < lsIconLbl.size(); i++)
	{
		WidgetIconLabel * lbl = lsIconLbl.at(i);
		lbl->close();
	}

	QList<WidgetLabel *> lsl = this->findChildren<WidgetLabel *>();
	for (int i = 0; i < lsl.size(); i++)
	{
		WidgetLabel * lbl = lsl.at(i);
		lbl->close();
	}

	QList<WidgetCameraList *> lslCamera = this->findChildren<WidgetCameraList *>();
	for (int i = 0; i < lslCamera.size(); i++)
	{
		WidgetCameraList * lbl = lslCamera.at(i);
		lbl->close();
	}

	ui.widgetRef->CloseStream();
	if (pInfo.rtsp != "")
		ui.widgetRef->OpenStream(pInfo.rtsp.toStdString());

	loadUiStatus();
}

void MultiLargeScreenWidget::ShowTargetListSlot()
{
	//QStringList ls;
	//AddListInfo(&ls, m_widgetTarget->ui.treeWidget, m_mapTarget);
}

void MultiLargeScreenWidget::ShowCameraListSlot()
{
	//QStringList ls;
	//AddListInfo(&ls, m_widgetTarget->ui.treeWidget, m_mapCameras);
}

void MultiLargeScreenWidget::ShowLabelSlot(bool blShow, QString lblId)
{
	std::map<QString, STRU_DB_LABEL >::iterator it;
	it = m_mapLabels.find(lblId);
	if (it == m_mapLabels.end())
	{
		//todo log error
		return;
	}
	QRect rtp = ui.widgetRef->geometry();
	STRU_DB_LABEL & lblData = it->second;
	//for (STRU_DB_LABEL& lblData : vlblData)
	{
		//if (lblData.labelId != lblId)
		//{
		//	continue;
		//}
		//new 
		QString y = "";
		int type1 = Match_DeviceTypeName(lblData.flagType, y);
		int type = -1;
		if (type1 >= 0 && type1 <= 2)
			type = 1;
		else if (type1 >= 3 && type1 <= 6)
			type = 0;
		if (lblData.userData == nullptr)
		{
			//?? 类型问题
			switch (type)
			{
			case 0:
			{
				WidgetLabel* lbl = new WidgetLabel(ui.widgetRef);
				QList<QString> lsDeviceid;
				for (auto &it : lblData.deviceLinks)
				{
					lsDeviceid.push_back(it.id);
				}
				lbl->SetInfoWithIcon(lblData.name, lblData.icon, lblId, lsDeviceid);
				lblData.userData = lbl;
				break;
			}
			case 1:
			{
				WidgetIconLabel* lbl = new WidgetIconLabel(ui.widgetRef);
				QList<QString> lsDeviceid;
				for (auto &it : lblData.deviceLinks)
				{
					lsDeviceid.push_back(it.id);
				}
				lbl->SetInfo(m_szSceneId, lblId, type1, lsDeviceid);
				lblData.userData = lbl;

				break;
			}
			default:
				break;
			}
		}

		//show hide
		switch (type)//lblData.type)
		{
		case 0:
		{
			WidgetLabel* lbl = static_cast<WidgetLabel*>(lblData.userData);
			if (!lbl)
			{
				//todo log error
				return;
			}

			lbl->move(lblData.xPos * rtp.width(), lblData.yPos * rtp.height());

			if (blShow)
				lbl->show();
			else
			{
				lbl->hide();
			}
			break;
		}
		case 1:
		{
			WidgetIconLabel* lbl = static_cast<WidgetIconLabel*>(lblData.userData);
			if (!lbl)
			{
				//todo log error
				return;
			}
			lbl->move(lblData.xPos * rtp.width(), lblData.yPos * rtp.height());
			if (blShow)
				lbl->show();
			else
			{
				lbl->hide();
			}
			break;
		}
		case 2:
		{
			//todo err!!
			//LabelAlarmIcon* lbl = static_cast<LabelAlarmIcon*>(lblData->userData);
			//if (!lbl)
			//{
			//	//todo log error
			//	return;
			//}
			//lbl->move(lblData.xPos * rtp.width(), lblData.yPos * rtp.height());
			//if (blShow)
			//	lbl->show();
			//else
			//	lbl->hide();
			break;
		}
		default:
			//todo log error

			break;
		}
	}
}

void MultiLargeScreenWidget::CheckandCleanAlarmLabelCount()
{
	uint64_t tm = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	QString lblid, szx, szy;
	//QList<LabelAlarmIcon *> lsLbl = this->findChildren<LabelAlarmIcon *>();
	//超时的清理
	QMap<uint64_t, LabelAlarmIcon*> lblSort;
	for (int i = 0; i < m_lsLblAlarm.size(); i++)
	{
		LabelAlarmIcon * lbl = m_lsLblAlarm.at(i);
		lbl->GetLabelInfo(lblid, szx, szy);
		uint64_t ul = lbl->GetLiveMillSecond();
		if (ul > m_config->alarmInf.maxLiveMillSecond && m_config->alarmInf.maxLiveMillSecond > 0)
		{
			//disconnect(m_mapWidget, &WidgetOfflineMap::MapLabelReturnPixel, lbl, &LabelAlarmIcon::ShowLabelPlaceSlot);

			qDebug() << "Label close tm :" << lblid << " tm " << tm << " this " << lbl;

			lbl->hide();
		}
		else
		{
			lblSort[ul] = lbl;
		}
	}

	QMap<uint64_t, LabelAlarmIcon*>::Iterator it;
	int i = 0;

	QString lblids, lbltypes, szxs, szys;
	
	//只保留最后面的 其他都清理掉
	for (it = lblSort.begin(); it!= lblSort.end(); it++, i++)
	{
		LabelAlarmIcon * lbl = it.value();

		lbl->GetLabelInfo(lblid, szx, szy);
		if (i < m_config->alarmInf.maxShowCount)
		{
			
			lblids = lblids + "," + lblid;
			lbltypes = lbltypes + ", 1";
			szxs = szxs + "," + szx;
			szys = szys + "," + szy;
			continue;
		}
		
		//disconnect(m_mapWidget, &WidgetOfflineMap::MapLabelReturnPixel, lbl, &LabelAlarmIcon::ShowLabelPlaceSlot);

		qDebug() << "Label close max :" << lblid << " tm " << tm << " this " << lbl;
		lbl->hide();
	}

	qDebug() << "Label after closed size= " << lblSort.size();

	for (auto &it1 : m_lsLblRef)
	{
		QString lblid, lat, lng;
		it1->GetLabelInfo(lblid, lat, lng);

		lblids = lblids + "," + lblid;
		lbltypes = lbltypes + ", 0";
		szxs = szxs + "," + lat;
		szys = szys + "," + lng;
	}


	////只保留最前面的 其他都清理掉
	//for (it = --lblSort.end(); it != --lblSort.begin(); i++, it--)
	//{
	//	if (i < m_config->alarmInf.maxShowCount)
	//		continue;
	//	LabelAlarmIcon * lbl = it.value();
	//	disconnect(m_mapWidget, &WidgetOfflineMap::MapLabelReturnPixel, lbl, &LabelAlarmIcon::ShowLabelPlaceSlot);
	//	qDebug() << "Label close max :" << lbl->m_szLabelId << " tm " << tm << " this " << lbl;
	//	lbl->close();
	//}

	// 更新html里面的标签位置
	m_mapWidget->PushLabelGps(lblids, lbltypes, szxs, szys);
}

void MultiLargeScreenWidget::AlarmInfoStatusSlot(QString szAlarmDesc)
{
	ui.lblStatus->setText(QString::fromUtf8("<html><head/><body><p><span style=\" color:#ff0000; \">%1</span></p></body></html>").arg(szAlarmDesc));
}

void MultiLargeScreenWidget::AlarmInfSlot(QString sceneId, qint64 timestamp, qreal lng, qreal lat, MqAlarmInfo inf)
{
	//场景相关问题被屏蔽了
	uint64_t tm = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	//LabelAlarmIcon * lbl = new LabelAlarmIcon(ui.widgetMap);

	int ia = -1;
	ia = Match_AlarmType(inf.msgData.rule.ruleType);
	if (m_lsLblAlarmPos < 0 || m_lsLblAlarmPos >= m_config->alarmInf.maxShowCount)
		m_lsLblAlarmPos = 0;
	LabelAlarmIcon * lbl = m_lsLblAlarm[m_lsLblAlarmPos];
	lbl->Init(ia, inf.msgId, &inf, false);

	////m_mapAlarmLbls[timestamp] = lbl;
	////lbl->move(lblData.xPos, lblData.yPos);
	//调用地图js 异步返回 先连接槽 然后调用

	qDebug() << "AlarmInfSlot connect:" << inf.msgId << " tm " << tm << " lbl:";// << lbl;

	//connect(m_mapWidget, &WidgetOfflineMap::MapLabelReturnPixel, lbl, &LabelAlarmIcon::ShowLabelPlaceSlot, Qt::UniqueConnection);
	//CalcLabelPixel ->JsCalcLabelPixel -> getLabelJsPixel ->MapLabelReturnPixel -> ShowLabelPlaceSlot
	m_mapWidget->CalcLabelPixel(inf.msgId, 1, QString::number(lat), QString::number(lng));

	m_lsLblAlarmPos++;
	//检查报警信息个数
	CheckandCleanAlarmLabelCount();
}

void MultiLargeScreenWidget::PresetChangeSlot(QListWidgetItem *item)
{
	if (!item)
		return;

	QString preid = item->data(Qt::UserRole).toString();
	if (m_svrInterface->GotoPreset(m_widgetVideoSegment->GetCurrentDeviceId(), preid))
	{

	}
	else
		qDebug() << "goto preset error";
}

void MultiLargeScreenWidget::CtrlPanelStop()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 0, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::ZoominSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 1, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::ZoomoutSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 2, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::CtrlPanelTopSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 21, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::CtrlPanelTopRightSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 26, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::CtrlPanelRightSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 24, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::CtrlPanelBottomRightSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 28, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::CtrlPanelBottomSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 22, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::CtrlPanelBottomLeftSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 27, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::CtrlPanelLeftSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 23, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::CtrlPanelTopLeftSlot()
{
	CtrlPanelToSvr(m_widgetVideoSegment->GetCurrentDeviceId(), 25, m_widgetSlider->GetValue());
}

void MultiLargeScreenWidget::ShowFullDomeSlot(QString deviceid)
{
	STRU_DB_DEVICE* pp;

	if (!FindDeviceById(deviceid, pp))
	{
		return;
	}

	QRect rt = this->geometry();
	WidgetDomeVideo* domeVideo = new WidgetDomeVideo(this);
	
	domeVideo->setWindowFlags(Qt::FramelessWindowHint);// Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	domeVideo->setWindowModality(Qt::ApplicationModal);
	domeVideo->setGeometry(rt.x() + rt.width() / 5, rt.y() + rt.height() / 5, rt.width() / 5.0 * 3, rt.height() / 5.0 * 3);

	domeVideo->ui.lblControlPanel->setDisabled(!pp->hasPTZ);
	domeVideo->ui.widget_6->setDisabled(!pp->hasPTZ);
	std::vector<STRU_DB_PRESET> presets;
	if (m_svrInterface->GetPreset(deviceid, "0", presets))
	{
		for (STRU_DB_PRESET& it : presets)
		{
			QListWidgetItem *item = new QListWidgetItem(it.name);
			item->setData(Qt::UserRole, it.id);
			domeVideo->ui.listWidget->addItem(item);
		}
	}

	//不知道流地址是否会变  每次都需要重新申请
	STRU_DB_STREAMURL pInfo;
	bool bl = m_svrInterface->GetDeviceStreamUrl(deviceid, "1", pInfo);
	if (bl)
	{
		domeVideo->OpenStream(pInfo.rtsp.toStdString());
	}
	else
		qDebug() << " GetDeviceStreamUrl error " << deviceid;

	domeVideo->SetInfo(deviceid, pp->hasPTZ);
	if (pp->hasPTZ)
	{
		//connect(domeVideo->ui.lblControlPanel, &WidgetControlPanel::OnClickTop, this, &MultiLargeScreenWidget::CtrlPanelTopSlot, Qt::QueuedConnection);
		//connect(domeVideo->ui.lblControlPanel, &WidgetControlPanel::OnClickTopRight, this, &MultiLargeScreenWidget::CtrlPanelTopRightSlot, Qt::QueuedConnection);
		//connect(domeVideo->ui.lblControlPanel, &WidgetControlPanel::OnClickRight, this, &MultiLargeScreenWidget::CtrlPanelRightSlot, Qt::QueuedConnection);
		//connect(domeVideo->ui.lblControlPanel, &WidgetControlPanel::OnClickBottomRight, this, &MultiLargeScreenWidget::CtrlPanelBottomRightSlot, Qt::QueuedConnection);
		//connect(domeVideo->ui.lblControlPanel, &WidgetControlPanel::OnClickBottom, this, &MultiLargeScreenWidget::CtrlPanelBottomSlot, Qt::QueuedConnection);
		//connect(domeVideo->ui.lblControlPanel, &WidgetControlPanel::OnClickBottomLeft, this, &MultiLargeScreenWidget::CtrlPanelBottomLeftSlot, Qt::QueuedConnection);
		//connect(domeVideo->ui.lblControlPanel, &WidgetControlPanel::OnClickLeft, this, &MultiLargeScreenWidget::CtrlPanelLeftSlot, Qt::QueuedConnection);
		//connect(domeVideo->ui.lblControlPanel, &WidgetControlPanel::OnClickTopLeft, this, &MultiLargeScreenWidget::CtrlPanelTopLeftSlot, Qt::QueuedConnection);

		//connect(domeVideo->ui.lblControlPanel, &WidgetControlPanel::OnStop, this, &MultiLargeScreenWidget::CtrlPanelStop, Qt::QueuedConnection);

		//connect(domeVideo->m_widgetSlider, &WidgetSlider::btnBigSignal, this, &MultiLargeScreenWidget::ZoominSlot, Qt::QueuedConnection);
		//connect(domeVideo->m_widgetSlider, &WidgetSlider::btnSmallSignal, this, &MultiLargeScreenWidget::ZoomoutSlot, Qt::QueuedConnection);
		//connect(domeVideo->m_widgetSlider, &WidgetSlider::btnStopSignal, this, &MultiLargeScreenWidget::CtrlPanelStop, Qt::QueuedConnection);
	}
	else
		domeVideo->ui.widget->hide();
	//connect(domeVideo->ui.lblControlPanel, &WidgetDomeVideo::IndexChangeSignal, this, &MultiLargeScreenWidget::CtrlPanelEnabledSlot, Qt::UniqueConnection);

	domeVideo->raise();
	domeVideo->show();
}

void MultiLargeScreenWidget::CtrlPanelEnabledSlot(int npos, QString deviceid)
{
	QString sz;
	STRU_DB_DEVICE* pp;

	ui.lblControlPanel->setDisabled(true);
	ui.widget_6->setDisabled(true);

	ui.listWidget->clear();
	if (!FindDeviceById(deviceid, pp))
	{
		return;
	}
	else
	{
		ui.lblControlPanel->setDisabled(!pp->hasPTZ);
		ui.widget_6->setDisabled(!pp->hasPTZ);
		std::vector<STRU_DB_PRESET> presets;
		if (m_svrInterface->GetPreset(deviceid, "0", presets)) 
		{
			for (STRU_DB_PRESET& it : presets)
			{
				QListWidgetItem *item = new QListWidgetItem(it.name);
				item->setData(Qt::UserRole, it.id);
				ui.listWidget->addItem(item);
			}
		}
	}
}

void MultiLargeScreenWidget::CtrlPanelToSvr(QString deviceId, int flag, float fspeed)
{
	if (m_svrInterface)
		m_svrInterface->DomeCtrl(deviceId, flag, fspeed);
}

void MultiLargeScreenWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	//case Qt::Key_Escape:
	//	if (Qt::WindowFullScreen == (this->windowState() & Qt::WindowFullScreen))
	//		this->showMaximized();
	//	else
	//		this->showFullScreen();
	//	break;
	case Qt::Key_F1:

		m_config->mapInf.dynamicCenterMap != m_config->mapInf.dynamicCenterMap;
		//for (int i = 0; i < 4; i++)
		//{
		//	m_AlarmIcon[i]->hide();
		//}
		//for (int i = 0; i < 7; i++)
		//{
		//	m_widgetIconLabels[i]->hide();
		//}
		//for (int i = 0; i < 5; i++)
		//{
		//	m_widgetLabels[i]->hide();
		//}
		break;

	case Qt::Key_F2:
		//for (int i = 0; i < 4; i++)
		//{
		//	m_AlarmIcon[i]->show();
		//}
		//for (int i = 0; i < 7; i++)
		//{
		//	m_widgetIconLabels[i]->show();
		//}
		//for (int i = 0; i < 5; i++)
		//{
		//	m_widgetLabels[i]->show();
		//}

		break;
	case Qt::Key_F9:
	{
		//QString resx, resy;
		////m_mapWidget->CalcPixels("116.350", "39.9705");
		//qDebug() << "JsCalcLabelPixel ";
		//m_mapWidget->CalcLabelPixel("lbl111", 1, "116.350", "39.9705");
		break;
	}
	default:
		QWidget::keyPressEvent(event);
	}
}

bool MultiLargeScreenWidget::saveUiStatus()
{
	int iii = 0;
	bool bl = false;
	for (auto & scinf : m_stLocalStatus.scenes)
	{
		if (scinf.sceneId == m_szSceneId)
		{
			m_stLocalStatus.scenes[iii].targets.clear();
			m_stLocalStatus.scenes[iii].cameras.clear();
			m_stLocalStatus.scenes[iii].cameraIds.clear();
			m_stLocalStatus.scenes[iii].count = m_nCurrentMaxCam;

			QList<WidgetIconLabel *> lsIconLbl = this->findChildren<WidgetIconLabel *>();
			for (int i = 0; i < lsIconLbl.size(); i++)
			{
				WidgetIconLabel * lbl = lsIconLbl.at(i);
				if (!lbl->isHidden())
				{
					scinf.cameras.append(lbl->GetId());
					lbl->hide();
				}
			}

			QList<WidgetLabel *> lsLbl = this->findChildren<WidgetLabel *>();
			for (int i = 0; i < lsLbl.size(); i++)
			{
				WidgetLabel * lbl = lsLbl.at(i);
				if (!lbl->isHidden())
				{
					scinf.targets.append(lbl->GetId());
					lbl->hide();
				}
			}

			QStringList lsd = m_widgetVideoSegment->GetCurrentDeviceIds();
			for (int i = 0; i < lsd.size(); i++)
			{
				scinf.cameraIds.append(lsd[i]);
			}

			bl = true;
			break;
		}
		iii++;
	}

	if (!bl)
	{
		UiLocalStatus::SceneInfo scinf;
		scinf.count = m_nCurrentMaxCam;

		QList<WidgetIconLabel *> lsIconLbl = this->findChildren<WidgetIconLabel *>();
		for (int i = 0; i < lsIconLbl.size(); i++)
		{
			WidgetIconLabel * lbl = lsIconLbl.at(i);
			if (!lbl->isHidden())
			{
				scinf.cameras.append(lbl->GetId());
				lbl->hide();
			}
		}

		QList<WidgetLabel *> lsLbl = this->findChildren<WidgetLabel *>();
		for (int i = 0; i < lsLbl.size(); i++)
		{
			WidgetLabel * lbl = lsLbl.at(i);
			if (!lbl->isHidden())
			{
				scinf.targets.append(lbl->GetId());
				lbl->hide();
			}
		}

		QStringList lsd = m_widgetVideoSegment->GetCurrentDeviceIds();
		for (int i = 0; i < lsd.size(); i++)
		{
			scinf.cameraIds.append(lsd[i]);
		}
		scinf.sceneId = m_szSceneId;
		m_stLocalStatus.scenes.push_back(scinf);
	}

	m_stLocalStatus.sceneId = m_szSceneId;

	//1. 保存到本地
	QJsonObject jsonRoot;
	QJsonDocument jsonDoc;
	jsonRoot["user"] = m_stLocalStatus.user;

	//QJsonObject segVideo;

	jsonRoot["sceneId"] = m_stLocalStatus.sceneId;

	QJsonArray scenes;

	for (int i = 0; i < m_stLocalStatus.scenes.size(); i++)
	{
		QJsonObject scif;
		scif["sceneId"] = m_stLocalStatus.scenes[i].sceneId;
		
		scif["count"] = m_stLocalStatus.scenes[i].count;

		QJsonArray cameraIds;
		for (auto& a1 : m_stLocalStatus.scenes[i].cameraIds)
			cameraIds.append(a1);
		scif["cameraIds"] = cameraIds;

		QJsonArray targets;
		for (auto& a1 : m_stLocalStatus.scenes[i].targets)
			targets.append(a1);

		//scif.insert("targets", targets);
		scif["targets"] = targets;

		QJsonArray cameras;
		for (auto& a1 : m_stLocalStatus.scenes[i].cameras)
			cameras.append(a1);
		scif["cameras"] = cameras;
		scenes.append(scif);
	}

	//SceneInfo scif;
	jsonRoot["sceneId"] = m_stLocalStatus.sceneId;
	jsonRoot["scenes"] = scenes;

	jsonDoc.setObject(jsonRoot);

	QByteArray postData = jsonDoc.toJson(QJsonDocument::Indented);

	QFile fl(qApp->applicationDirPath() + "/local_scene.json");
	if (!fl.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	int ii = fl.write(postData);

	fl.close();
	
	//2. 上传

	return true;
}

bool MultiLargeScreenWidget::loadUiStatus()
{
	//1. 从本地找

	QString path = qApp->applicationDirPath();
	QFile fl(qApp->applicationDirPath() + "/local_scene.json");
	if (!fl.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	QString sz = fl.readAll();
	fl.close();

	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(sz.toUtf8(), &jsonError);

	QJsonValue value;
	if (jsonError.error != QJsonParseError::NoError)
	{
		return false;
	}

	if (!json.isObject())
	{
		return false;
	}
	QJsonObject rootObj = json.object();

	if (!rootObj.contains("user") && !rootObj["user"].isString())
	{
		return false;
	}
	m_stLocalStatus.user = rootObj["user"].toString();

	if (!rootObj.contains("sceneId") && !rootObj["sceneId"].isString())
	{
		return false;
	}
	else
		m_stLocalStatus.sceneId = rootObj["sceneId"].toString();

	if (!rootObj.contains("scenes") && !rootObj["scenes"].isArray())
	{
		return false;
	}
	QJsonArray sceneArr = rootObj["scenes"].toArray();

	m_stLocalStatus.scenes.clear();

	for (int ii = 0; ii < sceneArr.size(); ii++)
	{
		if (sceneArr[ii].isObject())
		{
			QJsonObject sc = sceneArr[ii].toObject();
			if (!sc.contains("sceneId") || !sc["sceneId"].isString() ||
				!sc.contains("count") || !sc["count"].isDouble() ||
				!sc.contains("targets") || !sc["targets"].isArray() ||
				!sc.contains("cameras") || !sc["cameras"].isArray())
			{
				continue;
			}

			QJsonArray targetArr = sc["targets"].toArray();
			QJsonArray cameraArr = sc["cameras"].toArray();
			QJsonArray cameraIdsArr = sc["cameraIds"].toArray();

			UiLocalStatus::SceneInfo scif;
			scif.count = sc["count"].toInt();
			for (int ll = 0; ll < cameraIdsArr.size(); ll++)
				scif.cameraIds.append(cameraIdsArr[ll].toString());

			scif.sceneId = sc["sceneId"].toString();
			for (int ll = 0; ll < targetArr.size(); ll++)
				scif.targets.append(targetArr[ll].toString());

			for (int ll = 0; ll < cameraArr.size(); ll++)
				scif.cameras.append(cameraArr[ll].toString());

			m_stLocalStatus.scenes.push_back(scif);
		}
	}

	//2. m_stLocalStatus 
	for (int ii = 0; ii < m_stLocalStatus.scenes.size(); ii++)
	{
		if (m_stLocalStatus.scenes[ii].sceneId == m_szSceneId)
		{
			if (m_widgetVideoSegment)
			{
				//1 分屏逻辑重置
				m_nCurrentMaxCam = m_stLocalStatus.scenes[ii].count;
				if (m_nCurrentMaxCam > 0 && m_nCurrentMaxCam < 9)
				{
					m_widgetVideoSegment->CloseAllStream();
					m_widgetVideoSegment->SetSegmentCount(m_nCurrentMaxCam);

					int ipos = 0;
					for (auto & cam : m_stLocalStatus.scenes[ii].cameraIds)
					{
						STRU_DB_STREAMURL pInfo;
						bool bl = m_svrInterface->GetDeviceStreamUrl(cam, "1", pInfo);
						std::map<QString, STRU_DB_DEVICE>::iterator it = m_mapDevices.find(cam);
						if (bl && it != m_mapDevices.end() && cam != "")
						{
							QString szinf = it->second.name;
							m_widgetVideoSegment->OpenStreamSlot(cam, ipos, pInfo.rtsp.toStdString(), it->second.name, true);
						}
						ipos++;
						if (ipos > m_stLocalStatus.scenes[ii].count)
							break;
					}
				}
			}

			if (m_widgetTarget)
			{
				//2. 标签逻辑
				for (auto &kk : m_stLocalStatus.scenes[ii].targets)
				{
					QTreeWidgetItemIterator itt(m_widgetTarget->m_treeWidgetTarget);
					while (*itt)
					{
						QTreeWidgetItem *item = (*itt);
						if ((item->data(0, Qt::UserRole).toString()) == kk)
						{
							item->setSelected(true);
							item->setCheckState(0, Qt::Checked);

							if (item->checkState(0) == Qt::CheckState::Checked)
							{
								ShowLabelSlot(true, item->data(0, Qt::UserRole).toString());
							}
							else if (item->checkState(0) == Qt::CheckState::Unchecked)
							{
								ShowLabelSlot(false, item->data(0, Qt::UserRole).toString());
							}

							break;
						}

						++itt;
					}
				}
			}

			if (m_widgetTarget)
			{
				for (auto &cam : m_stLocalStatus.scenes[ii].cameras)
				{
					QTreeWidgetItemIterator itt(m_widgetTarget->m_treeWidgetCamera);
					while (*itt)
					{
						QTreeWidgetItem *item = (*itt);
						if ((item->data(0, Qt::UserRole).toString()) == cam)
						{
							item->setSelected(true);
							item->setCheckState(0, Qt::Checked);

							if (item->checkState(0) == Qt::CheckState::Checked)
							{
								ShowLabelSlot(true, item->data(0, Qt::UserRole).toString());
							}
							else if (item->checkState(0) == Qt::CheckState::Unchecked)
							{
								ShowLabelSlot(false, item->data(0, Qt::UserRole).toString());
							}
							break;
						}

						++itt;
					}
				}
			}
		}
	}
	
	//2. 没有的去远端查询

	return true;
}

//void MultiLargeScreenWidget::ConsumerReceiveMessageCallbackImp(std::string exchange, std::string key, std::string tags, std::string msg)
//{
//	//if (m_pAlarmDetectMq)
//	//	m_pAlarmDetectMq->ConsumerReceiveMessageCallbackImp(exchange, key, tags, msg);
//}

bool MultiLargeScreenWidget::FindDeviceById(QString deviceId, STRU_DB_DEVICE* &pp)
{
	std::map<QString, STRU_DB_DEVICE>::iterator it = m_mapDevices.find(deviceId);
	if (it != m_mapDevices.end())
	{
		pp = &it->second;
		return true;
	}
	else
		return false;
}

bool MultiLargeScreenWidget::FindOrganizationById(QString orgId, STRU_DB_ORGANIZATION* &pp)
{
	//std::map<std::string, STRU_DB_ORGANIZATION> m_mapOrgs;

	std::map<QString, STRU_DB_ORGANIZATION>::iterator it = m_mapOrgs.find(orgId);
	if (it != m_mapOrgs.end())
	{
		pp = &it->second;
		return true;
	}
	else
		return false;
}

bool MultiLargeScreenWidget::FindLabelById(QString sceneId, QString lblId, STRU_DB_LABEL* &pp)
{
	//std::map<std::string, std::vector<STRU_DB_LABEL> > m_mapLabels;
	std::map<QString, STRU_DB_LABEL> ::iterator it = m_mapLabels.find(lblId);
	if (it != m_mapLabels.end())
	{
		//for (auto & itn : it->second)
		{
			if (it->second.labelId == lblId)
			{
				pp = &it->second;
				return true;
			}
		}
	}

	return false;
}

QString MultiLargeScreenWidget::GetStreamUrl(QString deviceId, QString channel)
{
	if (!m_svrInterface)
		return "";

	STRU_DB_STREAMURL pInfo;
	if (!m_svrInterface->GetStreamUrl(deviceId, channel, pInfo))
	{
		return "";
	}

	return pInfo.rtsp;
}

void MultiLargeScreenWidget::slot_keyPressed(QKeyEvent * keyEvent)
{
	switch (keyEvent->key())
	{
	case Qt::Key_Escape:
		if (Qt::WindowFullScreen == (this->windowState() & Qt::WindowFullScreen))
			this->showMaximized();
		else
			this->showFullScreen();
		break;
	}
}
