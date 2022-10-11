#include "MqAlarmSvr.h"

#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

MqAlarmSvr::MqAlarmSvr(QObject *parent)
{
	m_blRunning = false;
}

MqAlarmSvr::~MqAlarmSvr()
{
	stop();
}

bool MqAlarmSvr::startThread(const Config* params)
{
	m_params = (Config* )params;

	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_blRunning)
		return true;

	m_blRunning = true;

	start();
	return true;
}

bool MqAlarmSvr::stop()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_blRunning)
		return true;

	m_blRunning = false;

	quit();
	return true;
}

void MqAlarmSvr::run()
{
	//ZH_LOG_FMT_LEVEL(SENTRYSVR_NAME, DEBUG, " MqAlarmSvr::StartThread! \n");

	while (m_blRunning)
	{
		ProcessMsg();
		if (!m_blRunning)
			break;
		int i = 1;
		
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		CleanUp();
	}
}

void MqAlarmSvr::ConsumerReceiveMessageCallbackImp(std::string exchange, std::string key, std::string tags, std::string msg)
{
	std::unique_lock<std::mutex> lck(m_mutexVtInfos);
	uint64_t ts = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	m_lastTm = ts;
	DectectMqInf inf;
	inf.ts = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	inf.exchange = exchange.c_str();
	inf.key = key.c_str();

	inf.msg = msg.c_str();// std::move(msg);

	m_vtInfos.push_back(inf);
}

void MqAlarmSvr::ProcessMsg()
{
	std::unique_lock<std::mutex> lck(m_mutexVtInfos);
	uint64_t tn = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	for (auto & it : m_vtInfos)
	{
		if (tn - it.ts > 3000)
		{
			//ZH_LOG_FMT_LEVEL(SENTRYSVR_NAME, WARN, "MqAlarmSvr continue %s", it.msg.substr(300, 100));
			continue;
		}

		//来自消息队列的数据转出信息 带gps
		MqAlarmInfo inf;
		if (!ParseMsg(it.msg, &inf))
		{
		}
		else
		{
			uint64_t tm = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			///todo //20220314 js
			qDebug() << " showAlarm" << inf.msgData.densityInfo.longitude << inf.msgData.densityInfo.latitude;
			if (inf.msgData.rule.algoType == "CrossLine" || inf.msgData.rule.algoType == "CrossRegion" || inf.msgData.rule.algoType == "ParkingRestrictedStop")
				emit showAlarm(inf.msgData.sceneId, tm, inf.msgData.detectInfo.longitude, inf.msgData.detectInfo.latitude, inf);
			else if (inf.msgData.rule.algoType == "Density")
				emit showAlarm(inf.msgData.sceneId, tm, inf.msgData.densityInfo.longitude, inf.msgData.densityInfo.latitude, inf);
			else
			{
				//todo log error
			}
		}
	}

	m_vtInfos.clear();
}

void MqAlarmSvr::CleanUp()
{
	std::unique_lock<std::mutex> lck(m_mutexVtInfos);
	uint64_t tn = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	for (auto it = m_vtInfos.begin(); it < m_vtInfos.end(); )
	{
		auto inf = *it;
		if (tn - inf.ts > 3000)
		{
			it = m_vtInfos.erase(it);
		}
		else
			it++;
	}
}

bool MqAlarmSvr::ParseMsg(QString msg, MqAlarmInfo* inf)
{
	if (inf == nullptr)
		return false;

	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(msg.toUtf8(), &jsonError);
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
		QJsonValue value = rootObj.value("msgType");
		if (!value.isString() || value.toString() != "alertInfo")
		{
			return false;
		}
	}
	if (!rootObj.contains("msgId") || !rootObj.value("msgId").isString())
	{
		return false;
	}
	else
		inf->msgId = rootObj["msgId"].toString();


	if (!rootObj.contains("msgData") || !rootObj.value("msgData").isObject())
	{
		return false;
	}
	QJsonObject msgObj = rootObj.value("msgData").toObject();

	if (!msgObj.contains("sceneId") || !msgObj.contains("deviceIp") || !msgObj.contains("alertDateTime") 
		|| !msgObj.contains("rule") || !msgObj["rule"].isObject() || !msgObj.contains("targetPoint") || !msgObj.contains("targetPoint_16x9"))
	{
		return false;
	}
	inf->msgData.deviceIp = msgObj["deviceIp"].toString();
	inf->msgData.sceneId = msgObj["sceneId"].toString();
	inf->msgData.alertDateTime = msgObj["alertDateTime"].toString();

	QJsonObject vrule = msgObj["rule"].toObject();
	if (!vrule.contains("ruleId") || !vrule["ruleId"].isDouble())// 
	{
		return false;
	}
	else
		inf->msgData.rule.ruleId = vrule["ruleId"].toInt();

	if (!vrule.contains("algoType") || !vrule.contains("ruleType") || !vrule.contains("ruleName"))
		return false;
	inf->msgData.rule.algoType = vrule["algoType"].toString();
	inf->msgData.rule.ruleType = vrule["ruleType"].toString();
	inf->msgData.rule.ruleName = vrule["ruleName"].toString();
	if (vrule.contains("ruleDescription"))
		inf->msgData.rule.ruleDescription = vrule["ruleDescription"].toString();

	if (!vrule.contains("direction") || !vrule["direction"].isDouble())
		;// return false;
	else
		inf->msgData.rule.direction = vrule["direction"].toInt();
	if (!vrule.contains("place") || !vrule["place"].isDouble())
		;// return false;
	else
		inf->msgData.rule.place = vrule["place"].toInt();

	if (!vrule.contains("detectLinePoint") || !vrule["detectLinePoint"].isDouble())
		return false;
	else
		inf->msgData.rule.detectLinePoint = vrule["detectLinePoint"].toInt();

	if (!vrule.contains("detectLine") || !vrule["detectLine"].isArray())
	{
		QJsonArray jsonArray = vrule["detectLine"].toArray();
		for (int i = 0; i < jsonArray.count(); i++)
		{
			if (jsonArray[i].isArray())
			{
				QJsonArray childValue = jsonArray[i].toArray();
				if (childValue.size() == 2)
				{
					if (childValue[0].isDouble() && childValue[1].isDouble())
					{
						MqAlarmInfo::MsgData::Point pt;
						pt.x = childValue[0].toDouble();
						pt.y = childValue[1].toDouble();
						inf->msgData.rule.detectLine.push_back(pt);
					}
				}
			}
		}
	}

	if (!vrule.contains("detectLine_16x9") || !vrule["detectLine_16x9"].isArray())
	{
		QJsonArray jsonArray = vrule["detectLine_16x9"].toArray();
		for (int i = 0; i < jsonArray.count(); i++)
		{
			if (jsonArray[i].isArray())
			{
				QJsonArray childValue = jsonArray[i].toArray();
				if (childValue.size() == 2)
				{
					if (childValue[0].isDouble() && childValue[1].isDouble())
					{
						MqAlarmInfo::MsgData::Point pt;
						pt.x = childValue[0].toDouble();
						pt.y = childValue[1].toDouble();
						inf->msgData.rule.detectLine_16x9.push_back(pt);
					}
				}
			}
		}
	}

	//
	if (msgObj.contains("targetPoint") && msgObj["targetPoint"].isObject())
	{
		QJsonObject obj = msgObj["targetPoint"].toObject();
		if (obj.contains("x") && obj.contains("y") && obj["x"].isDouble() && obj["y"].isDouble())
		{
			inf->msgData.targetPoint.x = obj["x"].toDouble();
			inf->msgData.targetPoint.y = obj["y"].toDouble();
		}
	}
	if (msgObj.contains("targetPoint_16x9") && msgObj["targetPoint_16x9"].isObject())
	{
		QJsonObject obj = msgObj["targetPoint_16x9"].toObject();
		if (obj.contains("x") && obj.contains("y") && obj["x"].isDouble() && obj["y"].isDouble())
		{
			inf->msgData.targetPoint_16x9.x = obj["x"].toDouble();
			inf->msgData.targetPoint_16x9.y = obj["y"].toDouble();
		}
	}

	if (inf->msgData.rule.algoType == "Density")
	{
		if (!msgObj.contains("densityInfo") || !msgObj["densityInfo"].isObject())
			return false;
		QJsonObject obj = msgObj["densityInfo"].toObject();

		if (!obj.contains("timestamp") || !obj["timestamp"].isDouble())
			return false;
		else
			inf->msgData.densityInfo.timestamp = (uint64_t)obj["timestamp"].toDouble();

		if (!obj.contains("image") || !obj.contains("videoUrl") || !obj.contains("objectCategory"))
		{
			return false;
		}
		inf->msgData.densityInfo.videoUrl = obj["videoUrl"].toString();
		inf->msgData.densityInfo.objectCategory = obj["objectCategory"].toString();
		inf->msgData.densityInfo.image = obj["image"].toString();

		if (!obj.contains("sensitivity") || !obj["sensitivity"].isDouble())
			return false;
		else
			inf->msgData.densityInfo.sensitivity = (uint64_t)obj["timestamp"].toDouble();

		if (!obj.contains("objectCount") || !obj["objectCount"].isDouble())
			return false;
		else
			inf->msgData.densityInfo.objectCount = obj["objectCount"].toInt();
		if (!obj.contains("azimuth") || !obj["azimuth"].isDouble())
			return false;
		else
			inf->msgData.densityInfo.azimuth = obj["azimuth"].toDouble();

		if (!obj.contains("elevation") || !obj["elevation"].isDouble())
			return false;
		else
			inf->msgData.densityInfo.elevation = obj["elevation"].toDouble();
		if (!obj.contains("longitude") || !obj["longitude"].isDouble())
			return false;
		else
			inf->msgData.densityInfo.longitude = obj["longitude"].toDouble();
		if (!obj.contains("latitude") || !obj["latitude"].isDouble())
			return false;
		else
			inf->msgData.densityInfo.latitude = obj["latitude"].toDouble();

		if (!obj.contains("coord") || !obj["coord"].isObject())
			return false;
		else
		{
			QJsonObject objcoord = obj["coord"].toObject();
			if (!objcoord.contains("x") || !objcoord["x"].isDouble())
				return false;
			if (!objcoord.contains("y") || !objcoord["y"].isDouble())
				return false;
			if (!objcoord.contains("w") || !objcoord["w"].isDouble())
				return false;
			if (!objcoord.contains("h") || !objcoord["h"].isDouble())
				return false;
			if (!objcoord.contains("pan") || !objcoord["pan"].isDouble())
				return false;
			if (!objcoord.contains("tilt") || !objcoord["tilt"].isDouble())
				return false;
			if (!objcoord.contains("zoom") || !objcoord["zoom"].isDouble())
				return false;

			inf->msgData.densityInfo.coord.x = objcoord["x"].toDouble();
			inf->msgData.densityInfo.coord.y = objcoord["y"].toDouble();
			inf->msgData.densityInfo.coord.w = objcoord["w"].toDouble();
			inf->msgData.densityInfo.coord.h = objcoord["h"].toDouble();
			inf->msgData.densityInfo.coord.pan = objcoord["pan"].toDouble();
			inf->msgData.densityInfo.coord.tilt = objcoord["tilt"].toDouble();
			inf->msgData.densityInfo.coord.zoom = objcoord["zoom"].toDouble();
		}

		if (!obj.contains("coord_16x9") || !obj["coord_16x9"].isObject())
			return false;
		else
		{
			QJsonObject objcoord = obj["coord_16x9"].toObject();
			if (!objcoord.contains("x") || !objcoord["x"].isDouble())
				return false;
			if (!objcoord.contains("y") || !objcoord["y"].isDouble())
				return false;
			if (!objcoord.contains("w") || !objcoord["w"].isDouble())
				return false;
			if (!objcoord.contains("h") || !objcoord["h"].isDouble())
				return false;
			if (!objcoord.contains("pan") || !objcoord["pan"].isDouble())
				return false;
			if (!objcoord.contains("tilt") || !objcoord["tilt"].isDouble())
				return false;
			if (!objcoord.contains("zoom") || !objcoord["zoom"].isDouble())
				return false;

			inf->msgData.densityInfo.coord_16x9.x = objcoord["x"].toDouble();
			inf->msgData.densityInfo.coord_16x9.y = objcoord["y"].toDouble();
			inf->msgData.densityInfo.coord_16x9.w = objcoord["w"].toDouble();
			inf->msgData.densityInfo.coord_16x9.h = objcoord["h"].toDouble();
			inf->msgData.densityInfo.coord_16x9.pan = objcoord["pan"].toDouble();
			inf->msgData.densityInfo.coord_16x9.tilt = objcoord["tilt"].toDouble();
			inf->msgData.densityInfo.coord_16x9.zoom = objcoord["zoom"].toDouble();
		}
	}

	else if (inf->msgData.rule.algoType == "CrossLine" || "CrossRegion" == inf->msgData.rule.algoType || "ParkingRestrictedStop" == inf->msgData.rule.algoType)
	{
		if (!msgObj.contains("detectInfo") || !msgObj["detectInfo"].isObject())
			return false;
		QJsonObject obj = msgObj["detectInfo"].toObject();

		if (!obj.contains("timestamp") || !obj["timestamp"].isDouble())
			return false;
		else
			inf->msgData.detectInfo.timestamp = (uint64_t)obj["timestamp"].toDouble();

		if (!obj.contains("detectId") || !obj.contains("trackId") || !obj.contains("objectClass") || !obj.contains("objectCategory") || !obj.contains("detectImage"))
		{
			return false;
		}
		inf->msgData.detectInfo.detectId = obj["detectId"].toString();
		inf->msgData.detectInfo.trackId = obj["trackId"].toString();
		inf->msgData.detectInfo.objectClass = obj["objectClass"].toString();
		inf->msgData.detectInfo.objectCategory = obj["objectCategory"].toString();
		inf->msgData.detectInfo.detectImage = obj["detectImage"].toString();
		
		if (!obj.contains("azimuth") || !obj["azimuth"].isDouble())
			return false;
		else
			inf->msgData.detectInfo.azimuth = obj["azimuth"].toDouble();

		if (!obj.contains("elevation") || !obj["elevation"].isDouble())
			return false;
		else
			inf->msgData.detectInfo.elevation = obj["elevation"].toDouble();
		if (!obj.contains("longitude") || !obj["longitude"].isDouble())
			return false;
		else
			inf->msgData.detectInfo.longitude = obj["longitude"].toDouble();
		if (!obj.contains("latitude") || !obj["latitude"].isDouble())
			return false;
		else
			inf->msgData.detectInfo.latitude = obj["latitude"].toDouble();

		if (!obj.contains("coord") || !obj["coord"].isObject())
			return false;
		else
		{
			QJsonObject objcoord = obj["coord"].toObject();
			if (!objcoord.contains("x") || !objcoord["x"].isDouble())
				return false;
			if (!objcoord.contains("y") || !objcoord["y"].isDouble())
				return false;
			if (!objcoord.contains("w") || !objcoord["w"].isDouble())
				return false;
			if (!objcoord.contains("h") || !objcoord["h"].isDouble())
				return false;
			if (!objcoord.contains("pan") || !objcoord["pan"].isDouble())
				return false;
			if (!objcoord.contains("tilt") || !objcoord["tilt"].isDouble())
				return false;
			if (!objcoord.contains("zoom") || !objcoord["zoom"].isDouble())
				return false;

			inf->msgData.detectInfo.coord.x = objcoord["x"].toDouble();
			inf->msgData.detectInfo.coord.y = objcoord["y"].toDouble();
			inf->msgData.detectInfo.coord.w = objcoord["w"].toDouble();
			inf->msgData.detectInfo.coord.h = objcoord["h"].toDouble();
			inf->msgData.detectInfo.coord.pan = objcoord["pan"].toDouble();
			inf->msgData.detectInfo.coord.tilt = objcoord["tilt"].toDouble();
			inf->msgData.detectInfo.coord.zoom = objcoord["zoom"].toDouble();
		}

		if (!obj.contains("coord_16x9") || !obj["coord_16x9"].isObject())
			return false;
		else
		{
			QJsonObject objcoord = obj["coord_16x9"].toObject();
			if (!objcoord.contains("x") || !objcoord["x"].isDouble())
				return false;
			if (!objcoord.contains("y") || !objcoord["y"].isDouble())
				return false;
			if (!objcoord.contains("w") || !objcoord["w"].isDouble())
				return false;
			if (!objcoord.contains("h") || !objcoord["h"].isDouble())
				return false;
			if (!objcoord.contains("pan") || !objcoord["pan"].isDouble())
				return false;
			if (!objcoord.contains("tilt") || !objcoord["tilt"].isDouble())
				return false;
			if (!objcoord.contains("zoom") || !objcoord["zoom"].isDouble())
				return false;

			inf->msgData.detectInfo.coord_16x9.x = objcoord["x"].toDouble();
			inf->msgData.detectInfo.coord_16x9.y = objcoord["y"].toDouble();
			inf->msgData.detectInfo.coord_16x9.w = objcoord["w"].toDouble();
			inf->msgData.detectInfo.coord_16x9.h = objcoord["h"].toDouble();
			inf->msgData.detectInfo.coord_16x9.pan = objcoord["pan"].toDouble();
			inf->msgData.detectInfo.coord_16x9.tilt = objcoord["tilt"].toDouble();
			inf->msgData.detectInfo.coord_16x9.zoom = objcoord["zoom"].toDouble();
		}
	}

	return true;
}
