#pragma once
#pragma execution_character_set("utf-8")
#include <QThread>
#include "Config.h"
#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include <map>
#include "HttpClientSvr.h"
//获取天气预报信息
class SvrWeather : public QThread
{
	Q_OBJECT

public:
	SvrWeather(QObject *parent);
	~SvrWeather();

	void Init(const HttpClientSvr * httpClientSvr, const Config& params, bool blUpdate);
public:
	bool startThread();
	// 停止客户端
	bool stop();
	void run() Q_DECL_OVERRIDE;

	void GetInfo(QString &szDay, QString &szCondition, QString &szWind,
		QString &szTemp, QString &szPm25,	QString &szPollution);

public:
signals:
	void UpdateInfo(QString szDay, QString szCondition, QString szWind,
		QString szTemp, QString szPm25, QString szPollution);
private:
	std::mutex m_mutex;
	volatile bool m_blRunning;
	Config* m_params;
	HttpClientSvr * m_httpClientSvr;

	bool GetInfoFromWeb();
	bool ParseMsg(QString sz);

	QString m_szDay;
	QString m_szCondition;
	QString m_szWind;
	QString m_szTemp;
	QString m_szPm25;
	QString m_szPollution;

	uint64_t m_uLast;

	bool m_blUpdate;
};
