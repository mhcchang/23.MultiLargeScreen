#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_WidgetOfflineMap.h"
#include <QThread>
#include <QWebEngineView>
#include <QWebChannel>
#include "WidgetSliderV.h"
#include <QMouseEvent>


class WidgetOfflineMap : public QWebEngineView
{
	Q_OBJECT

public:
	WidgetOfflineMap(QWidget *parent = Q_NULLPTR);
	~WidgetOfflineMap();

	//friend class WorkerThread;
public:
	bool Initialize(QString uri, bool blShowSlider = false);

	//��һ��ǩ ���lblid  lat lng  
	void CalcLabelPixel(QString lblid, int ntype, QString strlat, QString strlng);
	//����lng,lat ����  ��� ��Ļ����x,y����
	void CalcPixels(QString strx, QString stry);

	void PushLabelGps(QString lblids, QString lbltypes, QString strlats, QString strlngs);
	void PushLabelAndUpdateGps(QString lblids, QString lbltypes, QString strlats, QString strlngs);
	
	//void GpsUpdate(double longitude, double latitude, QString revsecond, QString revthird, double longitude1, double latitude1);
	void GpsLocation(double longitude, double latitude);

	void CenterPoint(float lat, float lng, float zoom);

	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	//void wheelEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	
public slots:
	void getJsPixels(QString strx, QString stry);
	void getLabelJsPixel(QString lblid, int type, QString strx, QString stry);
	//id
	void getNewLabelsPlace(QString lblids, QString lbltypes, QString strxs, QString strys);
	void handleResults();

	void ZoominSlot();
	void ZoomoutSlot();
	void ZoomValueSlot(float zoom);

public:
signals:
	//�����ߵ�ͼjs���ȡ��gps����  �ⲿ��ͨ�����Ӳۺ��� ��ȡMapReturnGps�źż���
	void MapReturnGps(float lng, float lat);
	//JsCalcPixels �������ص�ֵ
	void MapReturnPixels(QString strx, QString stry);
	//JsCalcPixel �������ص�ֵ ��һ��ǩ
	void MapAlarmLabelReturnPixel(QString lblid, QString strx, QString stry);
	//����ȫ����ǩ
	void MapRefLabelReturnPixel(QString lblid, QString strx, QString stry);

private:
	Ui::WidgetOfflineMap ui;
	QWebChannel * m_channel;

	bool m_blShowSlider;
	WidgetSliderV* m_silder;

	volatile bool m_blWait;
	void getCoordinates(double lng, double lat);

	QString m_strx, m_stry;
	QString m_strResX, m_strResY;
};
