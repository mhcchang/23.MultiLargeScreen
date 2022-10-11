#include "Config.h"
#include <QDebug>

Config::Config(QObject *parent)
	: QObject(parent)
{
}

Config::~Config()
{
}

bool Config::load(QString str)
{
	QFile file(str);
	QByteArray data;
	if (!file.open(QIODevice::ReadOnly | QIODevice::ExistingOnly))
	{
		return false;
	}
	else
	{
		data = file.readAll();
		file.close();
	}
	
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(data, &jsonError);

	qDebug() << data.data();
	QJsonValue value;
	if (jsonError.error == QJsonParseError::NoError)
	{
		if (json.isObject())
		{
			QJsonObject rootObj = json.object();
			QString szTitle;
			int cameraid;
			int serverID;
			int port;
			int nChannelIndex;
			int nFromDevice;
			bool bUVConvert;
			int type;
			QString url;

			//是否含有key  rootpath
			if (rootObj.contains("title"))
			{
				//<html><head/><body><p align=\"center\"><span style=\"color:#17cffd;\">大屏展示软件</span></p></body></html>
				//取出key为rootpath的值
				QJsonValue value = rootObj.value("title");
				if (value.isString())
				{
					stConfig.szTitle = QString::fromLocal8Bit("<html><head/><body><p align=\"center\"><span style=\"color:#17cffd;\">%1</span></p></body></html>").arg(value.toString());
				}
				else
					stConfig.szTitle = QString::fromLocal8Bit("<html><head/><body><p align=\"center\"><span style=\"color:#17cffd;\">大屏展示软件</span></p></body></html>");
			}
			if (rootObj.contains("serverIp"))
			{
				value = rootObj.value("serverIp");
				if (value.isString())
				{
					stConfig.szServerIp = value.toString();
				}
				else
					return false;
			}
			if (rootObj.contains("uri"))
			{
				value = rootObj.value("uri");
				if (value.isString())
				{
					stConfig.szUriPath = value.toString();
				}
				else
					return false;
			}			
			if (rootObj.contains("serverPort"))
			{
				value = rootObj.value("serverPort");
				if (value.isDouble())
				{
					stConfig.serverPort = value.toInt();
				}
			}

			if (rootObj.contains("userName"))
			{
				value = rootObj.value("userName");
				if (value.isString())
				{
					stConfig.szUser = value.toString();
				}
			}

			if (rootObj.contains("password"))
			{
				value = rootObj.value("password");
				if (value.isString())
				{
					stConfig.szPwd = value.toString();
				}
			}

			if (!rootObj.contains("localMap") || !rootObj["localMap"].isObject())
			{
				//LOG_FATAL_P1_NOFOUND("consumerMq");
				return false;
			}
			QJsonObject vlocalMap = rootObj["localMap"].toObject();
			if (!vlocalMap.contains("dynamicCenterMap") || !vlocalMap["dynamicCenterMap"].isBool())
			{
				mapInf.dynamicCenterMap = true;
			}
			else
				mapInf.dynamicCenterMap = vlocalMap["dynamicCenterMap"].toBool();
			if (!vlocalMap.contains("dynamicCenterMap") || !vlocalMap["dynamicCenterMap"].isBool())
			{
				mapInf.dynamicCenterMap = true;
			}
			else
				mapInf.dynamicCenterMap = vlocalMap["centerMapPoint"].toBool();
			if (!vlocalMap.contains("centerMapPoint") || !vlocalMap["centerMapPoint"].isString())
			{
				mapInf.centerMapPoint = "";
				mapInf.centerMapPointx = 360.0;
				mapInf.centerMapPointy = 180.0;
			}
			else
			{
				mapInf.centerMapPoint = vlocalMap["centerMapPoint"].toString();

				QStringList ls = vlocalMap["centerMapPoint"].toString().split(',');
				if (ls.size() == 2)
				{
					mapInf.centerMapPointx = ls[0].toDouble();
					mapInf.centerMapPointy = ls[1].toDouble();
				}
			}

			int z = 19;
			if (!vlocalMap.contains("centerMapZoom") || !vlocalMap["centerMapZoom"].isDouble())
			{
				mapInf.centerMapZoom = 19;
			}
			else
				z = vlocalMap["centerMapZoom"].toInt();
			if (z < 19 && z >= 4)
				mapInf.centerMapZoom = z;

			if (!rootObj.contains("consumerMq") || !rootObj["consumerMq"].isObject())
			{
				//LOG_FATAL_P1_NOFOUND("consumerMq");
				return false;
			}

			QJsonObject vConsumerMq = rootObj["consumerMq"].toObject();
			if (!vConsumerMq.contains("host"))
			{
				//LOG_FATAL_P2_NOOBJECT("consumerMq", "host");
				return false;
			}
			else
				consumer.host = vConsumerMq["host"].toString();

			if (vConsumerMq.contains("port"))
				consumer.port = vConsumerMq["port"].toInt();
			else
				consumer.port = 5672;
			if (!vConsumerMq.contains("username"))
			{
				//LOG_FATAL_P2_NOOBJECT("consumerMq", "username");
				return false;
			}
			else
				consumer.username = vConsumerMq["username"].toString();
			if (vConsumerMq.contains("password"))
				consumer.password = vConsumerMq["password"].toString();
			if (vConsumerMq.contains("queuename"))
				consumer.queuename = vConsumerMq["queuename"].toString();
			if (vConsumerMq.contains("missqueuename"))
				consumer.missqueuename = vConsumerMq["missqueuename"].toString();

			if (!vConsumerMq.contains("exchangename"))
			{
				//LOG_FATAL_P2_NOOBJECT("consumerMq", "exchangename");
				return false;
			}
			else
			{
				if (vConsumerMq["exchangename"].isString())
					consumer.exchangename.push_back(vConsumerMq["exchangename"].toString());
				else if (vConsumerMq["exchangename"].isArray())
				{
					QJsonArray arrx1 = vConsumerMq["exchangename"].toArray();
					int ll = arrx1.size();
					for (int k = 0; k < (int)arrx1.size(); k++)
					{
						if (arrx1[k].isString())
						{
							consumer.exchangename.push_back(arrx1[k].toString());
						}
					}
				}
			}

			if (!vConsumerMq.contains("routingkey"))
			{
				//LOG_FATAL_P2_NOOBJECT("consumerMq", "routingkey");
				return false;
			}
			else
			{
				if (vConsumerMq["routingkey"].isString())
					consumer.routingkey.push_back(vConsumerMq["routingkey"].toString());
				else if (vConsumerMq["routingkey"].isArray())
				{
					QJsonArray arrx1 = vConsumerMq["routingkey"].toArray();
					int ll = arrx1.size();
					for (int k = 0; k < (int)arrx1.size(); k++)
					{
						if (arrx1[k].isString())
						{
							consumer.routingkey.push_back(arrx1[k].toString());
						}
					}
				}
			}
			if (consumer.routingkey.size() == 0)
				consumer.routingkey.push_back("#");

			//PSourcePara spp = new SourcePara();
			//PSourcePara spp = sp;
			//spp->cameraid = cameraid;
			//spp->serverID = serverID;
			//spp->port = port;
			//spp->nChannelIndex = nChannelIndex;
			//spp->nFromDevice = nFromDevice;
			//spp->bUVConvert = bUVConvert;
			//spp->type = (SourceType)type;
			//memcpy(spp->url, url.toLocal8Bit().data(), __min(strlen(url.toLocal8Bit().data()), sizeof(spp->url)));

			if (!rootObj.contains("alarm") || !rootObj["alarm"].isObject())
			{
				return false;
			}

			QJsonObject alarmObj = rootObj["alarm"].toObject();
			if (!alarmObj.contains("maxShowCount") || !alarmObj["maxShowCount"].isDouble())
			{
				return false;
			}
			else
				alarmInf.maxShowCount = alarmObj["maxShowCount"].toInt();


			if (!rootObj.contains("gpsmap") || !rootObj["gpsmap"].isArray())
			{
				return false;
			}
			QJsonArray gpsmapArr = rootObj["gpsmap"].toArray();

			for (int ii = 0; ii < gpsmapArr.size(); ii++)
			{
				if (gpsmapArr[ii].isObject())
				{
					Stru_GpsMap gps;
					QJsonObject gpsObj = gpsmapArr[ii].toObject();
					if (!gpsObj.contains("sceneId") || !gpsObj["sceneId"].isString())
					{
						continue;
					}
					else
						gps.sceneId = gpsObj["sceneId"].toString();

					if (!gpsObj.contains("sceneId") || !gpsObj["sceneId"].isString())
					{
						continue;
					}
					else
						gps.sceneId = gpsObj["sceneId"].toString();
					lsGpsInf.push_back(gps);
				}
			}

			return true;
		}
	}
	return false;
}
