#pragma once

#include "PubUnit.h"
#include <QObject>
#include <experimental/filesystem>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

namespace std {
	namespace fs = std::experimental::filesystem;
}

class Config : public QObject
{
	Q_OBJECT

public:
	Config(QObject *parent = nullptr);
	~Config();
public:
	bool load(QString file);

	struct Stru_Config {
		QString szTitle;

		QString szServerIp;
		QString szUriPath;
		int serverPort;
		QString szUser;
		QString szPwd;
	};

	struct Stru_LocalMap {
		bool dynamicCenterMap;
		QString centerMapPoint;
		float centerMapPointx;
		float centerMapPointy;
		int centerMapZoom;
	};

	struct Stru_GpsMap {
		QString sceneId;
		QString gpsMap;
	};
	struct Stru_Alarm {
		int maxShowCount;
		int maxLiveMillSecond;
		Stru_Alarm() : maxShowCount(8), maxLiveMillSecond(0) {};
	};

	struct RabbitMqInf {
		QString host;
		int port;
		QString username;
		QString password;
		QString path;
		QString queuename;
		QVector<QString> exchangename;
		QString consumertag;
		QVector<QString> routingkey;
		QString missqueuename;
		QVector<QString> missRoutingkey;
		bool publisherconfirms;

		RabbitMqInf() : port(5672), publisherconfirms(true), path("/") {};
	};
	Stru_Config stConfig;

	QList<Stru_GpsMap> lsGpsInf;
	RabbitMqInf consumer;
	Stru_Alarm alarmInf;
	Stru_LocalMap mapInf;
};
