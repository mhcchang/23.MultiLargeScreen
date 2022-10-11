#include "SvrWeather.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SvrWeather::SvrWeather(QObject *parent)
	: QThread(parent)
{
	m_blRunning = false;
	m_blUpdate = false;
}

SvrWeather::~SvrWeather()
{
}

void SvrWeather::run()
{
	while (m_blRunning)
	{
		//正常1小时更新一次
		if (!GetInfoFromWeb())
		{
			QThread::sleep(60);
		}
		else
			QThread::sleep(60 * 60);
	}
}

void SvrWeather::Init(const HttpClientSvr * httpClientSvr, const Config& params, bool blUpdate)
{
	m_httpClientSvr = (HttpClientSvr *)httpClientSvr;
	m_params = (Config*)&params;
	m_blUpdate = blUpdate;
}

bool SvrWeather::startThread()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_blRunning)
		return true;

	m_blRunning = true;

	start();
	return true;
}

bool SvrWeather::stop()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_blRunning)
		return true;

	m_blRunning = false;

	quit();
	return true;
}

/*
{
	"errNo":"0",
	"data":{
		"weather":{
			"setting":{
				"city":"北京"
			},
			"content":{
				"week":"周三 02月23日",
				"city":"北京",
				"today":{
					"time":"周三 02月23日 (实时: 7℃)",
					"date":"02月23日",
					"img":[
						"",
						""
					],
					"condition":"晴",
					"wind":"西南风<3级",
					"temp":"7℃",
					"link":"http:\/\/www.weather.com.cn\/weather\/101010100.shtml#7d",
					"imgs":{
						"1":"a0",
						"0":"a0"
					},
					"pm25":"23",
					"pollution":"0",
					"pm25url":"\/\/www.baidu.com\/s?wd=%E5%8C%97%E4%BA%AC%E7%A9%BA%E6%B0%94%E8%B4%A8%E9%87%8F%E6%8C%87%E6%95%B0&tn=baidutop10&rsv_idx=2"
				},
*/

bool SvrWeather::GetInfoFromWeb()
{
	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	QByteArray ba;
	QString sz = m_httpClientSvr->GetTxtBody("https://www.baidu.com/home/other/data/weatherInfo?city=北京", ba, mapParam, mapHeader);

	return ParseMsg(sz);
}

bool SvrWeather::ParseMsg(QString sz)
{
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

	if (rootObj.contains("eCode"))
	{
		QJsonValue value = rootObj.value("eCode");
		if (!value.isString() || value.toString() != "0")
		{
			return false;
		}
	}

	if (!rootObj.contains("data") || !rootObj.value("data").isObject())
	{
		return false;
	}
	QJsonObject objData = rootObj["data"].toObject();
	if (!objData.contains("weather") || !objData.value("weather").isObject())
	{
		return false;
	}
	QJsonObject objWeather = objData["weather"].toObject();

	if (!objWeather.contains("content") || !objWeather["content"].isObject())
	{
		return false;
	}
	QJsonObject objContent = objWeather["content"].toObject();

	if (!objContent.contains("city") || !objContent.contains("week") || !objContent.contains("today"))
	{
		return false;
	}
	else
	{
		//todo 
	}

	if (!objContent["today"].isObject())
	{
		return false;
	}

	QJsonObject objtoday = objContent["today"].toObject();

	if (objtoday.contains("time") && objtoday["time"].isString())
	{
		
	}
	if (objtoday.contains("date") && objtoday["date"].isString())
	{
		m_szDay = objtoday["date"].toString();
	}
	if (objtoday.contains("condition") && objtoday["condition"].isString())
	{
		m_szCondition = objtoday["condition"].toString();
	}
	if (objtoday.contains("wind") && objtoday["wind"].isString())
	{
		m_szWind = objtoday["wind"].toString();
	}
	if (objtoday.contains("temp") && objtoday["temp"].isString())
	{
		m_szTemp = objtoday["temp"].toString();
	}	
	if (objtoday.contains("pm25") && objtoday["pm25"].isString())
	{
		m_szPm25 = objtoday["pm25"].toString();
	}
	if (objtoday.contains("pollution") && objtoday["pollution"].isString())
	{
		m_szPollution = objtoday["pollution"].toString();
	}

	return true;
}

void SvrWeather::GetInfo(QString &szDay, QString &szCondition, QString &szWind,
	QString &szTemp, QString &szPm25, QString &szPollution)
{
	m_szDay = szDay;
	m_szCondition = szCondition;
	m_szWind = szWind;
	m_szTemp = szTemp;
	m_szPm25 = szPm25;
	m_szPollution = szPollution;
}
