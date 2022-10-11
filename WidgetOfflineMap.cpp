#include "WidgetOfflineMap.h"
#include <QDir>
#include <QDebug>

WidgetOfflineMap::WidgetOfflineMap(QWidget *parent)
	: QWebEngineView(parent)
{
	ui.setupUi(this);
	m_channel = new QWebChannel(this);
	m_channel->registerObject(QString("showcoord"), this);
	//m_channel->registerObject(QString("returnpoints"), this);

	m_blShowSlider = false;
	m_silder = new WidgetSliderV(this);
	m_silder->hide();

	connect(m_silder, &WidgetSliderV::btnSmallSignal, this, &WidgetOfflineMap::ZoomoutSlot, Qt::UniqueConnection);
	connect(m_silder, &WidgetSliderV::btnBigSignal, this, &WidgetOfflineMap::ZoominSlot, Qt::UniqueConnection);
	connect(m_silder, &WidgetSliderV::ValueChangeSignal, this, &WidgetOfflineMap::ZoomValueSlot, Qt::UniqueConnection);

	ui.label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	//ui.label->setAttribute(Qt::WA_TranslucentBackground, true);
	m_blWait = false;
}

WidgetOfflineMap::~WidgetOfflineMap()
{
	m_channel->deregisterObject(this);
}

void WidgetOfflineMap::ZoominSlot()
{
	this->page()->runJavaScript(QString("JsZoomin()"));
}

void WidgetOfflineMap::ZoomoutSlot()
{
	this->page()->runJavaScript(QString("JsZoomout()"));
}

void WidgetOfflineMap::ZoomValueSlot(float zoom)
{
	this->page()->runJavaScript(QString("JsZoomValue(\"%1\")").arg(zoom));
}

void WidgetOfflineMap::GpsLocation(double longitude, double latitude)
{
	double lng = longitude;
	double lat = latitude;
	//qDebug() << fixed << qSetRealNumberPrecision(10) << longitude << latitude;
	//this->page()->runJavaScript(QString("updateInfo(%1,%2)").arg(lng, 0, 'g', 11).arg(lat, 0, 'g', 10));
	this->page()->runJavaScript(QString("updateInfo(%1,%2)").arg(lng).arg(lat));
}

void WidgetOfflineMap::CenterPoint(float lat, float lng, float zoom)
{
	this->page()->runJavaScript(QString("JsCenterPoint(\"%1\",\"%2\",\"%3\")").arg(lat).arg(lng).arg(zoom));
}

void WidgetOfflineMap::PushLabelGps(QString lblids, QString lbltypes, QString strlats, QString strlngs)
{
	//getNewLabelsPlace
	this->page()->runJavaScript(QString("JsPushLabelGps(\"%1\",\"%2\",\"%3\",\"%4\")").arg(lblids).arg(lbltypes).arg(strlats).arg(strlngs));
}

void WidgetOfflineMap::PushLabelAndUpdateGps(QString lblids, QString lbltypes, QString strlats, QString strlngs)
{
	//getNewLabelsPlace
	this->page()->runJavaScript(QString("JsPushLabelAndUpdateGps(\"%1\",\"%2\",\"%3\",\"%4\")").arg(lblids).arg(lbltypes).arg(strlats).arg(strlngs));
}

void WidgetOfflineMap::CalcLabelPixel(QString lblid, int ntype, QString strlat, QString strlng)
{
	//getLabelJsPixel
	uint64_t tm = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	qDebug() << "qt : CalcLabelPixel :" << QString("JsCalcLabelPixel(\"%1\",\"%2\",\"%3\") %4").arg(lblid).arg(strlat).arg(strlng).arg(tm);
	this->page()->runJavaScript(QString("JsCalcLabelPixel(\"%1\",\"%2\",\"%3\",\"%4\")").arg(lblid).arg(ntype).arg(strlng).arg(strlat));
}

void WidgetOfflineMap::CalcPixels(QString strx, QString stry)
{
	m_strx = strx, m_stry = stry;
	qDebug() << "qt :CalcPixels :" << QString("JsCalcPixels(\"%1\",\"%2\")").arg(strx).arg(stry);

	this->page()->runJavaScript(QString("JsCalcPixels(\"%1\",\"%2\")").arg(strx).arg(stry));
}

//void WidgetOfflineMap::GpsUpdate(double longitude, double latitude,  QString revsecond, QString revthird, double longitude1, double latitude1)
//{
//	//qDebug() << "Hweekhun" <<revsecond << revthird;
//	//emit obstacle_road(revsecond, revthird, longitude1, latitude1);
//	//QString json_str = QString(QJsonDocument(json_arr).toJson()).remove(QRegExp("\\s"));
//	//qDebug() << json_str;
//	//if (loadflag == true) {
//	double lng = longitude;
//	double lat = latitude;
//	//qDebug() << fixed << qSetRealNumberPrecision(10) << longitude << latitude;
//	this->page()->runJavaScript(QString("theLocation_obstacle(%1,%2)").arg(lng, 0, 'g', 11).arg(lat, 0, 'g', 10));
//	//}
//}

bool WidgetOfflineMap::Initialize(QString uri, bool blShowSlider)
{
	page()->setWebChannel(m_channel);
	setContextMenuPolicy(Qt::NoContextMenu);
	//QUrl url("file:html/map.html");

	//QUrl url("file:G:/zhy_work/04.test/40.tools/07.baidumap/02.bmap-offline-for-web-master/index.html");
	//QUrl url("file:G:/maponline.html");
	//QUrl url("G:/zhy_work/02.code/23.MultiLargeScreen/MultiLargeScreen/html/online/maponline.html");
	QUrl url(uri);
	this->page()->load(url);

	qDebug() << url.isRelative();
	qDebug() << QDir::isAbsolutePath(url.path());

	m_blShowSlider = blShowSlider;
	show();
	
	return true;
}

void WidgetOfflineMap::getCoordinates(double lng, double lat)
{
	emit MapReturnGps(lng, lat);
}

void WidgetOfflineMap::getJsPixels(QString strx, QString stry)
{
	qDebug() << " strx:" << strx << " stry" << stry;
	m_blWait = false;
	m_strResX = strx, m_strResY = stry;
	emit MapReturnPixels(m_strResX, m_strResY);
}

void WidgetOfflineMap::getLabelJsPixel(QString lblid, int type, QString strx, QString stry)
{
	qDebug() << "lblid " << lblid << " strx:" << strx << " stry" << stry;
	m_blWait = false;
	
	if (type == 1)
		emit MapAlarmLabelReturnPixel(lblid, strx, stry);
	else
		emit MapRefLabelReturnPixel(lblid, strx, stry);
}

//20220314 js
void WidgetOfflineMap::getNewLabelsPlace(QString lblids, QString lbltypes, QString strxs, QString strys)
{
	QString lblid, strx, stry;
	QStringList llbl = lblids.split(",");
	QStringList llbltype = lbltypes.split(",");
	QStringList lx = strxs.split(",");
	QStringList ly = strys.split(",");

	if (llbl.size() != lx.size() || ly.size() != lx.size())
		return;

	qDebug() << "getNewLabelsPlace length " << llbl.length();
	for (int ii = 0; ii < llbl.size(); ii++)
	{
		bool ok = false;
		int dec = llbltype[ii].toInt(&ok, 10);
		if (ok)
			getLabelJsPixel(llbl[ii], dec, lx[ii], ly[ii]);
	}
}

void WidgetOfflineMap::resizeEvent(QResizeEvent *event)
{
	QRect rt = this->geometry();
	ui.label->setGeometry(rt);
	ui.label->move(0, 0);
	ui.label->raise();

	if (!m_blShowSlider)
		return;
	m_silder->move(rt.width() / 10.0 * 9.5, rt.height() / 5.0 * 3);
	m_silder->raise();
	m_silder->show();

	m_silder->SetValue(100);
}

//void WidgetOfflineMap::startWorkInAThread()
//{
//	workerThread = new WorkerThread();
//	workerThread->setParam(this);
//	connect(workerThread, &WorkerThread::resultReady, this, &WidgetOfflineMap::handleResults);
//	//connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
//	workerThread->start();
//}

void WidgetOfflineMap::handleResults()
{
	qDebug() << " handleResults";
	m_blWait = false;
}

//void WorkerThread::setParam(WidgetOfflineMap* map)
//{
//	m_map = map;
//}
//
//void WorkerThread::run()
//{
//	m_map->m_blWait = true;
//	//this->page()->runJavaScript(QString("getPixelFromGps(%1,%2)").arg(strx).arg(stry));
//
//	//m_map->JsCalcPixels();
//	//QTime tt;
//	//tt.start();
//	//while (true)
//	//{
//	//	if (tt.elapsed() > 1000)
//	//	{
//	//		break;
//	//	}
//	//	QThread::msleep(100);
//
//	//}
//
//	//emit resultReady();
//}
//
//bool WorkerThread::JsCalcPixels(QString strx, QString stry, QString &strresx, QString &strresy)
//{
//	m_map->m_blWait = true;
//	m_map->JsCalcPixels(strx, stry);
//	return false;
//}