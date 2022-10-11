#include "SvrInterface.h"

const QVariant C_STR_LABEL_TYPE[10][3] = {
	{"buildingicon2x", 0}, {"dome2x", 1}, {"fixcam2x", 2},
};

SvrInterface::SvrInterface()
{
	m_httpClientSvr = nullptr;
	m_blUsessl = false;
	m_szToken = "";
	m_tmToken = 0;
}

SvrInterface::~SvrInterface()
{
	//m_httpClientSvr = nullptr;
}

void SvrInterface::SetParam(HttpClientSvr* httpClientSvr, Config* config, std::string ip, std::string szUriPath, int port, bool blUsessl)
{
	m_httpClientSvr = httpClientSvr;
	m_szIp = ip.c_str(); 
	m_nPort = port;
	m_szUriPath = szUriPath.c_str();
	m_blUsessl = blUsessl;
	m_config = config;
}
#if 0
bool SvrInterface::ParseLogInfo(QString szRespond, STRU_DB_USERINFO & res)
{
	STRU_DB_USERINFO res1;
	QJsonParseError jsonError;

	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

	QJsonValue value;
	if (jsonError.error == QJsonParseError::NoError)
	{
		if (json.isObject())
		{
			QJsonObject rootObj = json.object();

			if (rootObj.contains("eCode"))
			{
				QJsonValue value = rootObj.value("eCode");
				if (!value.isDouble() || value.toInt() != 0)
				{
					return false;
				}
			}
			if (!rootObj.contains("msgData") || !rootObj.value("msgData").isObject())
			{
				return false;
			}

			QJsonObject resObj = rootObj.value("msgData").toObject();
			//是否含有key  rootpath
			if (resObj.contains("id"))
			{
				QJsonValue value = rootObj.value("id");
				if (value.isDouble())
				{
					res1.id = value.toInt();
				}
				else
					return false;
			}
			if (resObj.contains("username"))
			{
				value = resObj.value("username");
				if (value.isString())
				{
					res1.username = value.toString();
				}
				else
					return false;
			}

			if (resObj.contains("phoneNo"))
			{
				value = resObj.value("phoneNo");
				if (value.isString())
				{
					res1.phoneNo = value.toString();
				}
				else
					return false;
			}
			if (resObj.contains("fullname"))
			{
				value = resObj.value("fullname");
				if (value.isString())
				{
					res1.fullname = value.toString();
				}
				else
					return false;
			}

			if (resObj.contains("actived"))
			{
				QJsonValue value = resObj.value("actived");
				if (value.isDouble())
				{
					res1.id = value.toInt();
				}
				else
					return false;
			}

			if (resObj.contains("roles"))
			{
				QJsonValue valueArray = resObj.value("roles");
				if (valueArray.isArray())
				{
					QJsonArray jsonArray = valueArray.toArray();
					for (int i = 0; i < jsonArray.count(); i++)
					{
						QJsonValue childValue = jsonArray[i];

						if (childValue.isObject())
						{
							STRU_DB_ROLE role;
							QJsonObject childObject = childValue.toObject();
							if (childObject.contains("roleId"))
							{
								QJsonValue valueJson = childObject.value("roleId");
								if (valueJson.isDouble())
								{
									role.roleId = valueJson.toDouble();
								}
							}
							if (childObject.contains("roleName"))
							{
								QJsonValue valueJson = childObject.value("roleName");
								if (valueJson.isString())
								{
									role.roleName = valueJson.toString();
								}
							}
							if (childObject.contains("roleDesc"))
							{
								QJsonValue valueJson = childObject.value("roleDesc");
								if (valueJson.isString())
								{
									role.roleDesc = valueJson.toString();
								}
							}

							res1.roles.push_back(role);
						}
					}
				}
			}		
			if (rootObj.contains("description"))
			{
				QJsonValue value = rootObj.value("description");
				if (value.isString())
				{
					res1.description = value.toString();
				}
			}
		}
	}

	res = res1;
	return true;
}
#else
bool SvrInterface::ParseLogInfo(QString szRespond, STRU_DB_USERINFO & res)
{
	STRU_DB_USERINFO res1;
	QJsonParseError jsonError;

	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

	QJsonValue value;
	if (jsonError.error == QJsonParseError::NoError)
	{
		if (json.isObject())
		{
			QJsonObject rootObj = json.object();

			if (rootObj.contains("code"))
			{
				QJsonValue value = rootObj.value("code");
				if (!value.isDouble() || value.toInt() != 200)
				{
					return false;
				}
			}
			if (!rootObj.contains("result") || !rootObj.value("result").isObject())
			{
				return false;
			}


			QJsonObject resObj = rootObj.value("result").toObject();
			//是否含有key  rootpath
			if (resObj.contains("role"))
			{
				QJsonValue value = resObj.value("role");
				if (value.isString())
				{
					res1.role = value.toString();
				}
				else
					return false;
			}
			if (resObj.contains("name"))
			{
				value = resObj.value("name");
				if (value.isString())
				{
					res1.username = value.toString();
				}
				else
					return false;
			}

			if (resObj.contains("token"))
			{
				value = resObj.value("token");
				if (value.isString())
				{
					res1.token = value.toString();
				}
				else
					return false;
			}

			if (resObj.contains("menu"))
			{
				value = resObj.value("menu");
				if (value.isArray())
				{
					
				}
				else
					return false;
			}
		
		}
	}

	res = res1;
	return true;
}
#endif
QString MakeLoginJson(QString username, QString password)
{
	QJsonObject jsonRoot;
	QJsonDocument jsonDoc;
	jsonRoot.insert("username", username);
	jsonRoot.insert("password", password);

	jsonDoc.setObject(jsonRoot);

	QByteArray postData = jsonDoc.toJson(QJsonDocument::Compact);
	return postData;
}

QByteArray MakeUrlJson(QString deviceid, int subchannel)
{
	QJsonObject jsonRoot;
	QJsonDocument jsonDoc;
	jsonRoot.insert("deviceId", deviceid);
	jsonRoot.insert("channel", subchannel);
	jsonRoot.insert("streamType", 0);
	jsonRoot.insert("protocol", 2);

	jsonDoc.setObject(jsonRoot);

	QByteArray postData = jsonDoc.toJson(QJsonDocument::Compact);
	return postData;
}

bool SvrInterface::GetLogInfo(QString username, QString password, STRU_DB_USERINFO &res)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/user/login", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());
	else
		sprintf(szPath, "https://%s:%d/%s/user/login", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	
	QString sz = MakeLoginJson(username, password);
	//mapHeader["password"] = GenMd5(password).c_str();
	//mapHeader["username"] = username.c_str();
	QString szRespond = m_httpClientSvr->Post(szPath, sz.toUtf8(), mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	return ParseLogInfo(szRespond, res);
}

bool SvrInterface::ParseResourceInfo(QString szRespond, std::vector<STRU_DB_RESOURCE> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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
		if (!value.isDouble() || value.toInt() != 0)
		{
			return false;
		}
	}
	if (!rootObj.contains("msgData") || !rootObj.value("msgData").isObject())
	{
		return false;
	}
	QJsonObject msgObj = rootObj.value("msgData").toObject();

	if (!msgObj.contains("list") || !msgObj.value("list").isArray())
	{
		return false;
	}

	QJsonArray jsonArray = msgObj.value("list").toArray();
	for (int i = 0; i < jsonArray.count(); i++)
	{
		QJsonValue childValue = jsonArray[i];

		if (childValue.isObject())
		{
			STRU_DB_RESOURCE resource;
			QJsonObject childObject = childValue.toObject();
			if (childObject.contains("id"))
			{
				QJsonValue valueJson = childObject.value("id");
				if (valueJson.isDouble())
				{
					resource.id = valueJson.toDouble();
				}
			}
			if (childObject.contains("name"))
			{
				QJsonValue valueJson = childObject.value("name");
				if (valueJson.isString())
				{
					valueJson.toString().toUtf8().data();
					resource.name = valueJson.toString();
				}
			}
			if (childObject.contains("description"))
			{
				QJsonValue valueJson = childObject.value("description");
				if (valueJson.isString())
				{
					resource.description = valueJson.toString();
				}
			}

			res.push_back(resource);
		}
	}

	return true;
}

bool SvrInterface::GetResource(std::vector<STRU_DB_RESOURCE> &res)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/api/resource", m_szIp.toStdString().c_str(), m_nPort);
	else
		sprintf(szPath, "https://%s:%d/api/resource", m_szIp.toStdString().c_str(), m_nPort);

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;
	QByteArray ba;
	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	return ParseResourceInfo(szRespond, res);
}

#if 0
bool SvrInterface::ParseOrganizationInfo(QString szRespond, std::vector<STRU_DB_ORGANIZATION> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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
		if (!value.isDouble() || value.toInt() != 0)
		{
			return false;
		}
	}
	if (!rootObj.contains("msgData") || !rootObj.value("msgData").isObject())
	{
		return false;
	}
	QJsonObject msgObj = rootObj.value("msgData").toObject();
	
	if (!msgObj.contains("list") || !msgObj.contains("size") || !msgObj.contains("total"))
	{
		return false;
	}
	QJsonValue valueArray = msgObj.value("list");
	if (!valueArray.isArray())
	{
		return false;
	}

	QJsonArray jsonArray = valueArray.toArray();
	for (int i = 0; i < jsonArray.count(); i++)
	{
		QJsonValue childValue = jsonArray[i];

		if (childValue.isObject())
		{
			STRU_DB_ORGANIZATION organization;
			QJsonObject childObject = childValue.toObject();
			if (childObject.contains("id"))
			{
				QJsonValue valueJson = childObject.value("id");
				if (valueJson.isDouble())
				{
					organization.id = valueJson.toDouble();
				}
			}
			if (childObject.contains("uuid"))
			{
				QJsonValue valueJson = childObject.value("uuid");
				if (valueJson.isString())
				{
					organization.uuid = valueJson.toString();
				}
			}
			if (childObject.contains("name"))
			{
				QJsonValue valueJson = childObject.value("name");
				if (valueJson.isString())
				{
					organization.name = valueJson.toString();
				}
			}
			if (childObject.contains("type"))
			{
				QJsonValue valueJson = childObject.value("type");
				if (valueJson.isString())
				{
					organization.type = valueJson.toString();
				}
			}
			if (childObject.contains("actived"))
			{
				QJsonValue valueJson = childObject.value("actived");
				if (valueJson.isDouble())
				{
					organization.actived = valueJson.toDouble();
				}
			}
			if (childObject.contains("description"))
			{
				QJsonValue valueJson = childObject.value("description");
				if (valueJson.isString())
				{
					organization.description = valueJson.toString();
				}
			}

			res.push_back(organization);
		}
	}

	return true;
}

#else
bool SvrInterface::ParseRecursiveOrganization(QJsonArray childValue, std::vector<STRU_DB_ORGANIZATION> &res)
{
	for (int i = 0; i < childValue.count(); i++)
	{
		STRU_DB_ORGANIZATION organization;
		//organization
		QJsonObject childObject = childValue[i].toObject();

		if (childObject.contains("uuid"))
		{
			QJsonValue valueJson = childObject.value("uuid");
			if (valueJson.isString())
			{
				organization.uuid = valueJson.toString();
			}
		}
		if (childObject.contains("name"))
		{
			QJsonValue value = childObject.value("name");
			if (value.isString())
			{
				//char* ba = value.toString().toUtf8().data();  //ok
				//char* ba1 = value.toString().toLatin1().data();
				//char* ba2 = value.toString().toLocal8Bit().data();
				//char* ba3 = (char*)value.toString().data();
				//QString sz1 = value.toString();
				//QString szgbk = QString::fromUtf8(value.toString().toStdString().c_str()).toLocal8Bit();
				organization.name = value.toString();
				//std::wstring ws = value.toString().toStdWString();
			}
		}
		if (childObject.contains("type"))
		{
			QJsonValue valueJson = childObject.value("type");
			if (valueJson.isString())
			{
				organization.type = valueJson.toString();
			}
		}
		if (childObject.contains("parentId"))
		{
			QJsonValue valueJson = childObject.value("parentId");
			if (valueJson.isString())
			{
				organization.parentId = valueJson.toString();
			}
		}
		if (childObject.contains("contactList") && childObject["contactList"].isArray())
		{
			QJsonArray valueArray = childObject.value("contactList").toArray();
			for (int ii = 0; ii < valueArray.size(); ii++)
			{

				if (valueArray[ii].isObject())
				{
					STRU_DB_CONTACT contact;
					QJsonObject v = valueArray[ii].toObject();
					if (v.contains("id"))
					{
						contact.id = v["id"].toString();
					}

					if (v.contains("conactsName"))
					{
						contact.contactsName = v["conactsName"].toString();
					}
					if (v.contains("phoneNo"))
					{
						contact.phoneNo = v["phoneNo"].toString();
					}

					if (v.contains("phoneNo"))
					{
						contact.idNum = v["phoneNo"].toString();
					}
					organization.contacts.push_back(contact);
				}
			}
		}

		res.push_back(organization);
		//ParseRecursiveOrganization(QJsonValue childValue, std::vector<STRU_DB_ORGANIZATION> &res)
		if (childObject.contains("child") && childObject["child"].isArray())
		{
			QJsonArray childArray1 = childObject["child"].toArray();
			ParseRecursiveOrganization(childArray1, res);
		}
	}

	return true;
}

bool SvrInterface::ParseOrganizationInfo(QString szRespond, std::vector<STRU_DB_ORGANIZATION> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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

	if (rootObj.contains("code"))
	{
		QJsonValue value = rootObj.value("code");
		if (!value.isDouble() || value.toInt() != 200)
		{
			return false;
		}
	}
	if (!rootObj.contains("result") || !rootObj.value("result").isObject())
	{
		return false;
	}
	QJsonObject msgObj = rootObj.value("result").toObject();

	if (!msgObj.contains("list"))
	{
		return false;
	}
	
	QJsonValue valueArray = msgObj.value("list");

	if (!valueArray.isArray())
	{
		return false;
	}
	
	QJsonArray jsonArray = valueArray.toArray();

	return ParseRecursiveOrganization(jsonArray, res);
}
#endif

//组织结构
bool SvrInterface::GetOrganization(std::vector<STRU_DB_ORGANIZATION> &res)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/address", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());
	else
		sprintf(szPath, "https://%s:%d/%s/address", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;

	QByteArray ba;
	//test
	mapHeader["authorization"] = m_szToken;
	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}
	QString szRespond1 = QString::fromUtf8(szRespond.toLocal8Bit());
	return ParseOrganizationInfo(szRespond1, res);
}
#if 0
bool SvrInterface::ParseDevicesInfo(QString szRespond, std::vector<STRU_DB_DEVICE> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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
		if (!value.isDouble() || value.toInt() != 0)
		{
			return false;
		}
	}
	if (!rootObj.contains("msgData") || !rootObj.value("msgData").isObject())
	{
		return false;
	}
	QJsonObject msgObj = rootObj.value("msgData").toObject();

	if (!msgObj.contains("list") || !msgObj.contains("size") || !msgObj.contains("total"))
	{
		return false;
	}
	QJsonValue valueArray = msgObj.value("list");
	if (!valueArray.isArray())
	{
		return false;
	}

	QJsonArray jsonArray = valueArray.toArray();
	for (int i = 0; i < jsonArray.count(); i++)
	{
		QJsonValue childValue = jsonArray[i];

		if (childValue.isObject())
		{
			STRU_DB_DEVICE device;
			QJsonObject childObject = childValue.toObject();
			if (childObject.contains("id"))
			{
				QJsonValue valueJson = childObject.value("id");
				if (valueJson.isDouble())
				{
					device.id = valueJson.toDouble();
				}
			}
			if (childObject.contains("uuid"))
			{
				QJsonValue valueJson = childObject.value("uuid");
				if (valueJson.isString())
				{
					device.uuid = valueJson.toString();
				}
			}
			if (childObject.contains("orgUUID"))
			{
				QJsonValue valueJson = childObject.value("orgUUID");
				if (valueJson.isString())
				{
					device.orgUUID = valueJson.toString();
				}
			}
			if (childObject.contains("name"))
			{
				QJsonValue valueJson = childObject.value("name");
				if (valueJson.isString())
				{
					device.name = valueJson.toString();
				}
			}
			if (childObject.contains("type"))
			{
				QJsonValue valueJson = childObject.value("type");
				if (valueJson.isString())
				{
					device.type = valueJson.toString();
				}
			}
			if (childObject.contains("subtype"))
			{
				QJsonValue valueJson = childObject.value("subtype");
				if (valueJson.isString())
				{
					device.subtype = valueJson.toString();
				}
			}
			if (childObject.contains("actived"))
			{
				QJsonValue valueJson = childObject.value("actived");
				if (valueJson.isDouble())
				{
					device.actived = valueJson.toDouble();
				}
			}
			if (childObject.contains("status"))
			{
				QJsonValue valueJson = childObject.value("status");
				if (valueJson.isDouble())
				{
					device.status = valueJson.toDouble();
				}
			}
			if (childObject.contains("channelCount"))
			{
				QJsonValue valueJson = childObject.value("channelCount");
				if (valueJson.isDouble())
				{
					device.channelCount = valueJson.toDouble();
				}
			}
			if (childObject.contains("serial"))
			{
				QJsonValue valueJson = childObject.value("serial");
				if (valueJson.isString())
				{
					device.serial = valueJson.toString();
				}
			}
			if (childObject.contains("manufacturer"))
			{
				QJsonValue valueJson = childObject.value("manufacturer");
				if (valueJson.isString())
				{
					device.manufacturer = valueJson.toString();
				}
			}
			if (childObject.contains("ip"))
			{
				QJsonValue valueJson = childObject.value("ip");
				if (valueJson.isString())
				{
					device.ip = valueJson.toString();
				}
			}
			if (childObject.contains("port"))
			{
				QJsonValue valueJson = childObject.value("port");
				if (valueJson.isString())
				{
					device.ip = valueJson.toString();
				}
			}
			if (childObject.contains("username"))
			{
				QJsonValue valueJson = childObject.value("username");
				if (valueJson.isString())
				{
					device.username = valueJson.toString();
				}
			}
			if (childObject.contains("password"))
			{
				QJsonValue valueJson = childObject.value("password");
				if (valueJson.isString())
				{
					device.password = valueJson.toString();
				}
			}
			if (childObject.contains("protocol"))
			{
				QJsonValue valueJson = childObject.value("protocol");
				if (valueJson.isString())
				{
					device.protocol = valueJson.toString();
				}
			}
			if (childObject.contains("latitude"))
			{
				QJsonValue valueJson = childObject.value("latitude");
				if (valueJson.isDouble())
				{
					device.latitude = valueJson.toDouble();
				}
			}
			if (childObject.contains("longtitude"))
			{
				QJsonValue valueJson = childObject.value("longtitude");
				if (valueJson.isDouble())
				{
					device.longtitude = valueJson.toDouble();
				}
			}
			if (childObject.contains("elevation"))
			{
				QJsonValue valueJson = childObject.value("elevation");
				if (valueJson.isDouble())
				{
					device.elevation = valueJson.toDouble();
				}
			}			
			if (childObject.contains("rolationAngle"))
			{
				QJsonValue valueJson = childObject.value("rolationAngle");
				if (valueJson.isDouble())
				{
					device.rolationAngle = valueJson.toDouble();
				}
			}
			if (childObject.contains("pitchAngle"))
			{
				QJsonValue valueJson = childObject.value("pitchAngle");
				if (valueJson.isDouble())
				{
					device.pitchAngle = valueJson.toDouble();
				}
			}

			if (childObject.contains("hasPTZ"))
			{
				QJsonValue valueJson = childObject.value("hasPTZ");
				if (valueJson.isBool())
				{
					device.hasPTZ = valueJson.toBool();
				}
			}

			if (childObject.contains("ptzType"))
			{
				QJsonValue valueJson = childObject.value("ptzType");
				if (valueJson.isString())
				{
					device.ptzType = valueJson.toString();
				}
			}
			res.push_back(device);
		}
	}

	return true;
}
#else
bool SvrInterface::ParseDevicesInfo(QString szRespond, std::vector<STRU_DB_DEVICE> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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

	if (rootObj.contains("code"))
	{
		QJsonValue value = rootObj.value("code");
		if (!value.isDouble() || value.toInt() != 200)
		{
			return false;
		}
	}

	if (rootObj.contains("message"))
	{
		QJsonValue value = rootObj.value("message");
		if (!value.isString() || value.toString() != "OK")
		{
			return false;
		}
	}

	if (!rootObj.contains("result") || !rootObj.value("result").isObject())
	{
		return false;
	}
	QJsonObject msgObj = rootObj.value("result").toObject();

	if (!msgObj.contains("list") && !msgObj["list"].isArray())
	{
		return false;
	}

	QJsonArray jsonArray = msgObj["list"].toArray();
	for (int i = 0; i < jsonArray.count(); i++)
	{
		QJsonValue childValue = jsonArray[i];

		if (childValue.isObject())
		{
			STRU_DB_DEVICE device;
			QJsonObject childObject = childValue.toObject();
			if (childObject.contains("deviceId"))
			{
				QJsonValue valueJson = childObject.value("deviceId");
				if (valueJson.isString())
				{
					device.deviceId = valueJson.toString();
				}
			}
			if (childObject.contains("name"))
			{
				QJsonValue valueJson = childObject.value("name");
				if (valueJson.isString())
				{
					device.name = valueJson.toString();
				}
			}
			if (childObject.contains("type"))
			{
				QJsonValue valueJson = childObject.value("type");
				if (valueJson.isString())
				{
					device.type = valueJson.toString();
				}
			}
			if (childObject.contains("channelCount"))
			{
				QJsonValue valueJson = childObject.value("channelCount");
				if (valueJson.isDouble())
				{
					device.channelCount = valueJson.toInt();
				}
			}
			if (childObject.contains("status"))
			{
				QJsonValue valueJson = childObject.value("status");
				if (valueJson.isDouble())
				{
					device.status = valueJson.toInt();
				}
			}
			if (childObject.contains("ip"))
			{
				QJsonValue valueJson = childObject.value("ip");
				if (valueJson.isString())
				{
					device.ip = valueJson.toString();
				}
			}
			if (childObject.contains("port"))
			{
				QJsonValue valueJson = childObject.value("port");
				if (valueJson.isDouble())
				{
					device.port = valueJson.toDouble();
				}
			}

			if (childObject.contains("username"))
			{
				QJsonValue valueJson = childObject.value("username");
				if (valueJson.isString())
				{
					device.username = valueJson.toString();
				}
			}
			if (childObject.contains("password"))
			{
				QJsonValue valueJson = childObject.value("password");
				if (valueJson.isString())
				{
					device.password = valueJson.toString();
				}
			}
			if (childObject.contains("protocol"))
			{
				QJsonValue valueJson = childObject.value("protocol");
				if (valueJson.isString())
				{
					device.protocol = valueJson.toString();
				}
			}
			if (childObject.contains("serial"))
			{
				QJsonValue valueJson = childObject.value("serial");
				if (valueJson.isString())
				{
					device.serial = valueJson.toString();
				}
			}

			if (childObject.contains("manufacturer"))
			{
				QJsonValue valueJson = childObject.value("manufacturer");
				if (valueJson.isString())
				{
					device.manufacturer = valueJson.toString();
				}
			}

			if (childObject.contains("orgId"))
			{
				QJsonValue valueJson = childObject.value("orgId");
				if (valueJson.isString())
				{
					device.orgId = valueJson.toString();
				}
			}
			if (childObject.contains("latitude"))
			{
				QJsonValue valueJson = childObject.value("latitude");
				if (valueJson.isDouble())
				{
					device.latitude = valueJson.toDouble();
				}
			}
			if (childObject.contains("longtitude"))
			{
				QJsonValue valueJson = childObject.value("longtitude");
				if (valueJson.isDouble())
				{
					device.longtitude = valueJson.toDouble();
				}
			}
			if (childObject.contains("elevation"))
			{
				QJsonValue valueJson = childObject.value("elevation");
				if (valueJson.isDouble())
				{
					device.elevation = valueJson.toDouble();
				}
			}
			if (childObject.contains("rolationAngle"))
			{
				QJsonValue valueJson = childObject.value("rolationAngle");
				if (valueJson.isDouble())
				{
					device.rolationAngle = valueJson.toDouble();
				}
			}
			if (childObject.contains("pitchAngle"))
			{
				QJsonValue valueJson = childObject.value("pitchAngle");
				if (valueJson.isDouble())
				{
					device.pitchAngle = valueJson.toDouble();
				}
			}
			if (childObject.contains("location"))
			{
				QJsonValue valueJson = childObject.value("location");
				if (valueJson.isString())
				{
					device.location = valueJson.toString();
				}
			}

			if (childObject.contains("accessGateway"))
			{
				QJsonValue valueJson = childObject.value("accessGateway");
				if (valueJson.isString())
				{
					device.accessGateway = valueJson.toString();
				}
			}
			if (childObject.contains("sceneId"))
			{
				QJsonValue valueJson = childObject.value("sceneId");
				if (valueJson.isString())
				{
					device.sceneId = valueJson.toString();
				}
			}
			if (childObject.contains("thumb"))
			{
				QJsonValue valueJson = childObject.value("thumb");
				if (valueJson.isString())
				{
					device.thumb = valueJson.toString();
				}
			}
			if (childObject.contains("hasPTZ"))
			{
				QJsonValue valueJson = childObject.value("hasPTZ");
				if (valueJson.isBool())
				{
					device.hasPTZ = valueJson.toBool();
				}
			}

			if (childObject.contains("ptzType"))
			{
				QJsonValue valueJson = childObject.value("ptzType");
				if (valueJson.isString())
				{
					device.ptzType = valueJson.toString();
				}
			}
			if (childObject.contains("contactsName"))
			{
				QJsonValue valueJson = childObject.value("contactsName");
				if (valueJson.isString())
				{
					device.contactsName = valueJson.toString();
				}
			}
			if (childObject.contains("phoneNo"))
			{
				QJsonValue valueJson = childObject.value("phoneNo");
				if (valueJson.isString())
				{
					device.phoneNo = valueJson.toString();
				}
			}
			if (childObject.contains("idNum"))
			{
				QJsonValue valueJson = childObject.value("idNum");
				if (valueJson.isString())
				{
					device.idNum = valueJson.toString();
				}
			}
			res.push_back(device);
		}
	}

	return true;
}
#endif
//设备
bool SvrInterface::GetDevices(QString orgId, std::vector<STRU_DB_DEVICE> &res)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/device/%s", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str(), orgId.toStdString().data());
	else
		sprintf(szPath, "https://%s:%d/%s/device/%s", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str(), orgId.toStdString().data());

	QMap<QString, QString> mapParam;
	mapParam["name"] = "";
	mapParam["state"] = "";
	mapParam["pageNum"] = "0";
	mapParam["pageSize"] = "10000";
	
	//name=&state=&pageNum=0&pageSize=10000
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;

	QByteArray ba;
	//mapParam["orgId"] = orgId;// QString::number(orgId);
	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	return ParseDevicesInfo(szRespond, res);
}
#if 0
bool SvrInterface::ParseDeviceStreamUrlInfo(QString szRespond, STRU_DB_STREAMURL &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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
		if (!value.isDouble() || value.toInt() != 0)
		{
			return false;
		}
	}
	if (!rootObj.contains("msgData") || !rootObj.value("msgData").isObject())
	{
		return false;
	}
	QJsonObject msgObj = rootObj.value("msgData").toObject();

	if (msgObj.contains("app"))
	{
		QJsonValue valueJson = msgObj.value("app");
		if (valueJson.isString())
		{
			res.app = valueJson.toString();
		}
	}
	if (msgObj.contains("streamId"))
	{
		QJsonValue valueJson = msgObj.value("streamId");
		if (valueJson.isString())
		{
			res.streamId = valueJson.toString();
		}
	}
	if (msgObj.contains("mediaServerId"))
	{
		QJsonValue valueJson = msgObj.value("mediaServerId");
		if (valueJson.isString())
		{
			res.mediaServerId = valueJson.toString();
		}
	}
	if (msgObj.contains("rtsp"))
	{
		QJsonValue valueJson = msgObj.value("rtsp");
		if (valueJson.isString())
		{
			res.rtsp = valueJson.toString();
		}
	}
	return true;
}
#else
bool SvrInterface::ParseDeviceStreamUrlInfo(QString szRespond, STRU_DB_STREAMURL &res, bool blSub)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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

	if (rootObj.contains("code"))
	{
		QJsonValue value = rootObj.value("code");
		if (!value.isDouble() || value.toInt() != 200)
		{
			return false;
		}
	}

	if (!rootObj.contains("result") || !rootObj.value("result").isArray())
	{
		return false;
	}

	QJsonArray list = rootObj.value("result").toArray();

	for (int i = 0; i < list.size(); i++)
	{
		if (!list[i].isObject()) 
		{
			continue;
		}
		QJsonObject obj = list[i].toObject();
		if (!obj.contains("deviceId") || !obj["deviceId"].isString())
			continue;
		if (!obj.contains("url") || !obj["url"].isObject())
			continue;

		QJsonObject url = obj["url"].toObject();

		if (!url.contains("rtsp") || !url["rtsp"].isString())
			continue;
		if (!blSub)
			res.rtsp = url["rtsp"].toString();
		else
			res.rtspSub = url["rtsp"].toString();
		//if (!url.contains("ctrl_url") || !obj["ctrl_url"].isString())
		//	res.ctrlUrl = url["ctrl_url"].toString();
	}

	return true;
}

#endif
//设备流信息
bool SvrInterface::GetDeviceStreamUrl(QString deviceId, QString channelId, STRU_DB_STREAMURL& pInfo)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/splitscreen/startMonitor", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());
	else
		sprintf(szPath, "https://%s:%d/%s/splitscreen/startMonitor", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;

	//mapParam["deviceId"] = deviceId;
	//mapParam["channelId"] = channelId;

	//mapParam["streamType"] = "0";
	//mapParam["protocol"] = "2";
	QByteArray ba = MakeUrlJson(deviceId, channelId.toShort());
	
	QString szRespond = m_httpClientSvr->Post(szPath, ba, mapParam, mapHeader);

	if (szRespond == "")
	{
		return false;
	}
	
	return ParseDeviceStreamUrlInfo(szRespond, pInfo, channelId == "1" ? false : true);
}

bool SvrInterface::ParseDefaultInfo(QString szRespond, QString szReason)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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

	if (rootObj.contains("code"))
	{
		QJsonValue value = rootObj.value("code");
		if (!value.isDouble() || value.toInt() != 200)
		{
			return false;
		}
	}

	if (rootObj.contains("message"))
	{
		QJsonValue value = rootObj.value("message");
		if (!value.isString())
		{
			return false;
		}

		szReason = value.toString();
	}
	return true;

}

bool SvrInterface::ParsePresetInfo(QString szRespond, std::vector<STRU_DB_PRESET> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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

	if (rootObj.contains("code"))
	{
		QJsonValue value = rootObj.value("code");
		if (!value.isDouble() || value.toInt() != 200)
		{
			return false;
		}
	}
	if (!rootObj.contains("result") || !rootObj.value("result").isArray())
	{
		return false;
	}

	QJsonArray jsonArray = rootObj.value("result").toArray();
	for (int i = 0; i < jsonArray.count(); i++)
	{
		QJsonValue childValue = jsonArray[i];

		if (childValue.isObject())
		{
			STRU_DB_PRESET preset;
			QJsonObject childObject = childValue.toObject();
			if (childObject.contains("presetId"))
			{
				QJsonValue valueJson = childObject.value("presetId");
				if (valueJson.isString())
				{
					preset.id = valueJson.toString();
				}
			}
			if (childObject.contains("name"))
			{
				QJsonValue valueJson = childObject.value("name");
				if (valueJson.isString())
				{
					preset.name = valueJson.toString();
				}
			}
			if (childObject.contains("protocol"))
			{
				QJsonValue valueJson = childObject.value("protocol");
				if (valueJson.isString())
				{
					preset.protocol = valueJson.toString();
				}
			}

			if (childObject.contains("context"))
			{
				QJsonValue valueJson = childObject.value("context");
				if (valueJson.isString())
				{
					preset.context = valueJson.toString();
				}
			}

			//if (childObject.contains("pan"))
			//{
			//	QJsonValue valueJson = childObject.value("pan");
			//	if (valueJson.isDouble())
			//	{
			//		preset.pan = valueJson.toDouble();
			//	}
			//}
			//if (childObject.contains("tilt"))
			//{
			//	QJsonValue valueJson = childObject.value("tilt");
			//	if (valueJson.isDouble())
			//	{
			//		preset.tilt = valueJson.toDouble();
			//	}
			//}
			//if (childObject.contains("zoom"))
			//{
			//	QJsonValue valueJson = childObject.value("zoom");
			//	if (valueJson.isDouble())
			//	{
			//		preset.zoom = valueJson.toDouble();
			//	}
			//}
			res.push_back(preset);
		}
	}

	return true;
}

//预置位
bool SvrInterface::GetPreset(QString deviceId, QString channelId, std::vector<STRU_DB_PRESET> &res)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/device/ptz/preset/%s", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str(), deviceId.toStdString().c_str());
	else
		sprintf(szPath, "https://%s:%d/%s/device/ptz/preset/%s", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str(), deviceId.toStdString().c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;
	QByteArray ba;
	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	res.clear();
	return ParsePresetInfo(szRespond, res);
}

bool SvrInterface::GotoPreset(QString deviceId, QString presetId)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/device/ptz/preset/goto/%s", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str(), presetId.toStdString().c_str());
	else
		sprintf(szPath, "https://%s:%d/%s/device/ptz/preset/goto/%s", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str(), presetId.toStdString().c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;
	QByteArray ba;

	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	QString szReason;
	return ParseDefaultInfo(szRespond, szReason);
}

bool SvrInterface::MatchLabelType(QString sz, int &type)
{
	for (int i = 0; i < 10; i++)
	{
		if (C_STR_LABEL_TYPE[i][0] == sz)
		{
			type = C_STR_LABEL_TYPE[i][1].toInt();
			return true;
		}
	}
	return false;
}

bool SvrInterface::ParseRecursiveLabel(QJsonArray jsonArray, std::vector<STRU_DB_LABEL> &res)
{
	for (int i = 0; i < jsonArray.count(); i++)
	{
		QJsonValue childValue = jsonArray[i];

		if (childValue.isObject())
		{
			STRU_DB_LABEL label;
			QJsonObject childObject = childValue.toObject();
			if (childObject.contains("flagId"))
			{
				QJsonValue valueJson = childObject.value("flagId");
				if (valueJson.isString())
				{
					label.labelId = valueJson.toString();
				}
			}
			if (childObject.contains("name"))
			{
				QJsonValue valueJson = childObject.value("name");
				if (valueJson.isString())
				{
					label.name = valueJson.toString();
				}
			}

			if (childObject.contains("parentId"))
			{
				QJsonValue valueJson = childObject.value("parentId");
				if (valueJson.isString())
				{
					label.parentId = valueJson.toString();
				}
			}
			if (childObject.contains("parentName"))
			{
				QJsonValue valueJson = childObject.value("parentName");
				if (valueJson.isString())
				{
					label.parentName = valueJson.toString();

				}
			}
			if (childObject.contains("senceId"))
			{
				QJsonValue valueJson = childObject.value("senceId");
				if (valueJson.isString())
				{
					label.sceneId = valueJson.toString();

				}
			}

			if (childObject.contains("flagType"))
			{
				QJsonValue valueJson = childObject.value("flagType");
				if (valueJson.isString())
				{
					label.flagType = valueJson.toString();

					if (!MatchLabelType(valueJson.toString(), label.type))
					{
						//todo log error
					}
				}
			}
			if (childObject.contains("icon"))
			{
				QJsonValue valueJson = childObject.value("icon");
				if (valueJson.isString())
				{
					label.icon = valueJson.toString();
				}
			}
			if (childObject.contains("location"))
			{
				QJsonValue valueJson = childObject.value("location");
				if (valueJson.isString())
				{
					QString sz = valueJson.toString();
					QStringList ss = sz.split(',');
					if (2 == ss.length())
					{
						bool ok = false;
						float xpos = ss[0].toDouble(&ok);
						if (!ok)
							return false;

						ok = false;
						float ypos = ss[1].toDouble(&ok);
						if (!ok)
							return false;
						label.xPos = xpos;
						label.yPos = ypos;
					}
				}
			}

			if (childObject.contains("group"))
			{
				QJsonValue valueJson = childObject.value("group");
				if (valueJson.isBool())
				{
					label.blHasChild = valueJson.toBool();
				}
			}
			
			res.push_back(label);

			if (childObject.contains("child") && childObject["child"].isArray())
			{
				QJsonArray valueChild = childObject["child"].toArray();
				ParseRecursiveLabel(valueChild, res);
			}
		}
	}

	return true;
}
#if 0
bool SvrInterface::ParseLabelsInfo(QString szRespond, std::vector<STRU_DB_LABEL> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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
		if (!value.isDouble() || value.toInt() != 0)
		{
			return false;
		}
	}
	if (!rootObj.contains("msgData") || !rootObj.value("msgData").isObject())
	{
		return false;
	}
	QJsonObject msgObj = rootObj.value("msgData").toObject();

	if (!msgObj.contains("list") || !msgObj.contains("size") || !msgObj.contains("total"))
	{
		return false;
	}
	QJsonValue valueArray = msgObj.value("list");
	if (!valueArray.isArray())
	{
		return false;
	}

	QJsonArray jsonArray = valueArray.toArray();
	for (int i = 0; i < jsonArray.count(); i++)
	{
		QJsonValue childValue = jsonArray[i];

		if (childValue.isObject())
		{
			STRU_DB_LABEL label;
			QJsonObject childObject = childValue.toObject();
			if (childObject.contains("id"))
			{
				QJsonValue valueJson = childObject.value("id");
				if (valueJson.isDouble())
				{
					label.id = valueJson.toDouble();
				}
			}
			if (childObject.contains("name"))
			{
				QJsonValue valueJson = childObject.value("name");
				if (valueJson.isString())
				{
					label.name = valueJson.toString();
				}
			}
			if (childObject.contains("type"))
			{
				QJsonValue valueJson = childObject.value("type");
				if (valueJson.isString())
				{
					label.name = valueJson.toString();
				}
			}

			if (childObject.contains("xPos"))
			{
				QJsonValue valueJson = childObject.value("xPos");
				if (valueJson.isDouble())
				{
					label.xPos = valueJson.toDouble();
				}
			}
			if (childObject.contains("yPos"))
			{
				QJsonValue valueJson = childObject.value("yPos");
				if (valueJson.isDouble())
				{
					label.yPos = valueJson.toDouble();
				}
			}
			res.push_back(label);

		}
	}

	return true;
}
#else
bool SvrInterface::ParseLabelsInfo(QString szRespond, std::vector<STRU_DB_LABEL> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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

	if (rootObj.contains("code"))
	{
		QJsonValue value = rootObj.value("code");
		if (!value.isDouble() || value.toInt() != 200)
		{
			return false;
		}
	}
	if (!rootObj.contains("result") || !rootObj.value("result").isObject())
	{
		return false;
	}
	QJsonObject msgObj = rootObj.value("result").toObject();

	if (!msgObj.contains("list"))
	{
		return false;
	}

	if (msgObj.contains("list") && msgObj["list"].isArray())
	{
		QJsonArray valueChild = msgObj["list"].toArray();
		return ParseRecursiveLabel(valueChild, res);
	}

	return true;
}
#endif

#if 0
//标签
bool SvrInterface::GetLabels(std::string deviceId, std::string senceId, std::vector<STRU_DB_LABEL> &res, int count, int page)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/flag/%s/%s", m_szIp.c_str(), m_nPort, deviceId.c_str(), senceId.c_str());
	else
		sprintf(szPath, "https://%s:%d/flag/device/list/%s/%s", m_szIp.c_str(), m_nPort, deviceId.c_str(), senceId.c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;

	QByteArray ba;
	mapParam["deviceId"] = deviceId.c_str();
	if (senceId != "")
		mapParam["senceId"] = senceId.c_str();
	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	return ParseLabelsInfo(szRespond, res);
}
#else
bool SvrInterface::GetLabels(std::vector<STRU_DB_LABEL> &res, int count, int page)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/flag", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());
	else
		sprintf(szPath, "https://%s:%d/%s/flag", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;

	QByteArray ba;

	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	return ParseLabelsInfo(szRespond, res);
}
#endif

bool SvrInterface::ParseLabelTypeInfo(QString szRespond, std::vector<STRU_DB_LABELTYPE> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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

	if (rootObj.contains("code"))
	{
		QJsonValue value = rootObj.value("code");
		if (!value.isDouble() || value.toInt() != 200)
		{
			return false;
		}
	}
	if (!rootObj.contains("result") || !rootObj.value("result").isArray())
	{
		return false;
	}

	if (rootObj.contains("result") && rootObj["result"].isArray())
	{
		QJsonArray valueArr = rootObj["result"].toArray();
		for (int ii = 0; ii < valueArr.size(); ii++)
		{
			if (!valueArr[ii].isObject())
				continue;

			QJsonObject groupObj = valueArr[ii].toObject();

			if (!groupObj.contains("typeGroupname") || !groupObj.value("typeGroupname").isString())
			{
				return false;
			}

			QString groupname = groupObj.value("typeGroupname").toString();

			if (!groupObj.contains("list") || !groupObj.value("list").isArray())
			{
				continue;
			}

			QJsonArray typeArr = groupObj["list"].toArray();
			for (int jj = 0; jj < typeArr.size(); jj++)
			{
				if (!typeArr[jj].isObject())
					continue;
				QJsonObject typeObj = typeArr[jj].toObject();
				if (!typeObj.contains("flagName") || !typeObj.contains("icon"))
					continue;
				if (!typeObj["flagName"].isString() || !typeObj["icon"].isString())
					continue;

				STRU_DB_LABELTYPE stlbl;

				stlbl.typeGroupname = groupname;
				stlbl.flagName = typeObj["flagName"].toString();
				stlbl.icon = typeObj["icon"].toString();
				res.push_back(stlbl);
			}
		}
	}

	return true;
}

bool SvrInterface::GetLabelType(std::vector<STRU_DB_LABELTYPE> &res)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/flag/type", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());
	else
		sprintf(szPath, "https://%s:%d/%s/flag/type", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;

	QByteArray ba;

	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	return ParseLabelTypeInfo(szRespond, res);
}

bool SvrInterface::ParseLabelLinkDeviceInfo(QString szRespond, std::vector<STRU_DB_LABELLINKDEVICE> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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

	if (rootObj.contains("code"))
	{
		QJsonValue value = rootObj.value("code");
		if (!value.isDouble() || value.toInt() != 200)
		{
			return false;
		}
	}

	if (rootObj.contains("message"))
	{
		QJsonValue value = rootObj.value("message");
		if (!value.isString() || value.toString() != "OK")
		{
			return false;
		}
	}

	QJsonValue valueArray = rootObj.value("result");
	if (!valueArray.isArray())
	{
		return false;
	}

	QJsonArray jsonArray = valueArray.toArray();
	for (int i = 0; i < jsonArray.count(); i++)
	{
		QJsonValue childValue = jsonArray[i];

		if (childValue.isObject())
		{
			STRU_DB_LABELLINKDEVICE labelLink;
			QJsonObject childObject = childValue.toObject();
			if (childObject.contains("deviceId"))
			{
				QJsonValue valueJson = childObject.value("deviceId");
				if (valueJson.isString())
				{
					labelLink.id = valueJson.toString();
				}
			}
			if (childObject.contains("name"))
			{
				QJsonValue valueJson = childObject.value("name");
				if (valueJson.isString())
				{
					labelLink.name = valueJson.toString();
				}
			}

			res.push_back(labelLink);
		}
	}

	return true;
}

//标签对应的设备
bool SvrInterface::GetLabelLinkDevice(QString labelId, std::vector<STRU_DB_LABELLINKDEVICE> &res)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/flag/device/list?flagId=%s", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str(), labelId.toStdString().c_str());
	else
		sprintf(szPath, "https://%s:%d/%s/flag/device/list?flagId=%s", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str(), labelId.toStdString().c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;

	QByteArray ba;
	//mapParam["labelId"] = labelId.c_str();

	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	return ParseLabelLinkDeviceInfo(szRespond, res);
}

bool SvrInterface::ParseScenesInfo(QString szRespond, std::vector<STRU_DB_SCENE> &res)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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
		if (!value.isDouble() || value.toInt() != 0)
		{
			return false;
		}
	}
	if (!rootObj.contains("msgData") || !rootObj.value("msgData").isObject())
	{
		return false;
	}

	QJsonObject msgObj = rootObj.value("msgObj").toObject();
	if (!msgObj.contains("list") || !msgObj.contains("size") || !msgObj.contains("total"))
	{
		return false;
	}
	QJsonValue valueArray = msgObj.value("list");
	if (!valueArray.isArray())
	{
		return false;
	}

	QJsonArray jsonArray = valueArray.toArray();
	for (int i = 0; i < jsonArray.count(); i++)
	{
		QJsonValue childValue = jsonArray[i];

		if (childValue.isObject())
		{
			STRU_DB_SCENE scene;
			QJsonObject childObject = childValue.toObject();
			if (childObject.contains("id"))
			{
				QJsonValue valueJson = childObject.value("id");
				if (valueJson.isDouble())
				{
					scene.id = valueJson.toDouble();
				}
			}

			if (childObject.contains("name"))
			{
				QJsonValue valueJson = childObject.value("name");
				if (valueJson.isString())
				{
					scene.name = valueJson.toString();
				}
			}
			if (childObject.contains("deviceId"))
			{
				QJsonValue valueJson = childObject.value("deviceId");
				if (valueJson.isString())
				{
					scene.deviceId = valueJson.toString();
				}
			}
			if (childObject.contains("rolationAngle"))
			{
				QJsonValue valueJson = childObject.value("rolationAngle");
				if (valueJson.isDouble())
				{
					scene.rolationAngle = valueJson.toDouble();
				}
			}

			if (childObject.contains("pitchAngle"))
			{
				QJsonValue valueJson = childObject.value("pitchAngle");
				if (valueJson.isDouble())
				{
					scene.pitchAngle = valueJson.toDouble();
				}
			}

			if (childObject.contains("actived"))
			{
				QJsonValue valueJson = childObject.value("actived");
				if (valueJson.isDouble())
				{
					scene.actived = valueJson.toDouble();
				}
			}
			if (childObject.contains("description"))
			{
				QJsonValue valueJson = childObject.value("description");
				if (valueJson.isString())
				{
					scene.description = valueJson.toString();
				}
			}

			res.push_back(scene);
		}
	}

	return true;
}

//场景
bool SvrInterface::GetScenes(std::vector<STRU_DB_SCENE> &res, int count, int page)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/api/sences", m_szIp.toStdString().c_str(), m_nPort);
	else
		sprintf(szPath, "https://%s:%d/api/sences", m_szIp.toStdString().c_str(), m_nPort);

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;
	QByteArray ba;
	QString szRespond = m_httpClientSvr->GetTxtBody(szPath, ba, mapParam, mapHeader);
	if (szRespond == "")
	{
		return false;
	}

	return ParseScenesInfo(szRespond, res);
}

bool SvrInterface::GetStreamUrl(QString deviceId, QString channel, STRU_DB_STREAMURL& pInfo)
{
	return GetDeviceStreamUrl(deviceId, channel, pInfo);
}

QByteArray MakeDomeCtrlJson(QString deviceId, int flag, int speed)
{
	QJsonObject jsonRoot;
	QJsonDocument jsonDoc;
	jsonRoot.insert("deviceId", deviceId);
	jsonRoot.insert("command", flag);

	//PTZ_COMMAND_STOP = 0   /* 停止 */
	//	PTZ_COMMAND_ZOOM_IN,   /* 焦距变大(倍率变大) */
	//	PTZ_COMMAND_ZOOM_OUT,  /* 焦距变小(倍率变小) */
	//	PTZ_COMMAND_FOCUS_NEAR,  /* 焦点前调 */
	//	PTZ_COMMAND_FOCUS_FAR,   /* 焦点后调 */
	//	PTZ_COMMAND_IRIS_OPEN,   /* 光圈扩大 */
	//	PTZ_COMMAND_IRIS_CLOSE,  /* 光圈缩小 */

	//	PTZ_COMMAND_UP = 21,     /* 云台上仰 */
	//	PTZ_COMMAND_DOWN,        /* 云台下俯 */
	//	PTZ_COMMAND_LEFT,        /* 云台左转 */
	//	PTZ_COMMAND_RIGHT,       /* 云台右转 */
	//	PTZ_COMMAND_UP_LEFT,     /* 云台上仰和左转 */
	//	PTZ_COMMAND_UP_RIGHT,    /* 云台上仰和右转 */
	//	PTZ_COMMAND_DOWN_LEFT,   /* 云台下府和左转 */
	//	PTZ_COMMAND_DOWN_RIGHT,  /* 云台下府和右转 */

	switch (flag)
	{
	case 0:
		jsonRoot.insert("panSpeed", 0);
		jsonRoot.insert("tiltSpeed", 0);
		jsonRoot.insert("zoomSpeed", 0);

		break;

	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:

		jsonRoot.insert("zoomSpeed", speed);

		break;
	case 21:
	case 22:
	{
		jsonRoot.insert("tiltSpeed", speed);
		break;
	}

	case 23:
	case 24:
	{
		jsonRoot.insert("panSpeed", speed);
		break;
	}
	case 25:
	case 26:
	case 27:
	case 28:
		jsonRoot.insert("panSpeed", speed);
		jsonRoot.insert("tiltSpeed", speed);

		break;
	}

	jsonDoc.setObject(jsonRoot);

	QByteArray postData = jsonDoc.toJson(QJsonDocument::Compact);
	return postData;
}

bool SvrInterface::ParseDomeInf(QString szRespond)
{
	QJsonParseError jsonError;
	QJsonDocument json = QJsonDocument::fromJson(szRespond.toUtf8(), &jsonError);

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

	if (rootObj.contains("code"))
	{
		QJsonValue value = rootObj.value("code");
		if (!value.isDouble() || value.toInt() != 200)
		{
			return false;
		}
	}

	if (rootObj.contains("message"))
	{
		QJsonValue value = rootObj.value("message");
		if (!value.isString() || value.toString() != "OK")
		{
			return false;
		}
	}


	return true;
}

bool SvrInterface::DomeCtrl(QString deviceId, int flag, int speed)
{
	char szPath[1024] = { 0, };
	if (!m_blUsessl)
		sprintf(szPath, "http://%s:%d/%s/splitscreen/ptzControl", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());
	else
		sprintf(szPath, "https://%s:%d/%s/splitscreen/ptzControl", m_szIp.toStdString().c_str(), m_nPort, m_szUriPath.toStdString().c_str());

	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	mapHeader["authorization"] = m_szToken;

	QByteArray ba;
	ba = MakeDomeCtrlJson(deviceId, flag, speed);

	QString szRespond = m_httpClientSvr->Post(szPath, ba, mapParam, mapHeader);
	qDebug() << "dome ctrl :" << szRespond;
	if (szRespond == "")
	{
		return false;
	}

	return ParseDomeInf(szRespond);
}

std::string SvrInterface::GenMd5(std::string szSrc)
{
	MD5 md5;
	md5.read(szSrc);
	return md5.Encryption();
}

#if 0
//原来的逻辑是 组织\设备\标签\关联设备信息 分层  
bool SvrInterface::GetAllOrganization(std::map<std::string, STRU_DB_ORGANIZATION> &resOrg, std::map<std::string, STRU_DB_DEVICE> &resDevice, 
	std::map<std::string, std::string> mapOrgLinkDevice, std::map<std::string, std::vector<STRU_DB_LABEL> > &resLabels)
{
	//1. 先登录 拿token
	STRU_DB_USERINFO resUser;
	if (!GetLogInfo(m_config->stConfig.szUser.toStdString(), m_config->stConfig.szPwd.toStdString(), resUser))
	{
		m_szToken = "";
		return false;
	}
	m_szToken = resUser.token.c_str();

	std::vector<STRU_DB_ORGANIZATION> resVtOrg;
	if (!GetOrganization(resVtOrg))
		return false;

	for (STRU_DB_ORGANIZATION & org : resVtOrg)
	{
		resOrg[org.uuid] = org;

		std::vector<STRU_DB_DEVICE> vDev;
		if (!GetDevices(org.uuid.c_str(), vDev))
		{
			//todo log
		}
		else
		{
			for (STRU_DB_DEVICE & dev : vDev)
			{
				//组织关系和设备之间的关系表
				mapOrgLinkDevice[org.uuid + "/" + dev.deviceId] = dev.deviceId;
				resDevice[dev.deviceId] = dev;

				std::vector<STRU_DB_LABEL> vlbl;
				if (GetLabels(dev.deviceId, "", vlbl))
				{
					for (STRU_DB_LABEL & lbl : vlbl)
					{
						//后台数据 结构 应该是场景id 后台使用deviceId（概念不清容易混乱）
						lbl.sceneId = dev.deviceId;
						resLabels[lbl.sceneId].push_back(lbl);
					}
				}
				else
				{
					//todo log not found labels
				}
			}
		}
	}

	return true;
}

#else

//逻辑是 查组织\设备
//标签 \关联设备信息 两部分  
bool SvrInterface::GetAllOrganization(std::map<QString, STRU_DB_ORGANIZATION> &resOrg, QList<STRU_DB_ORGANIZATION> &vCameraGroup, 
	std::map<QString, STRU_DB_DEVICE> &resDevice, QList<STRU_DB_DEVICE> &lsScene, std::map<QString, QString> &mapOrgLinkDevice,
	std::vector<STRU_DB_LABELTYPE> &labelGroup, std::map<QString, STRU_DB_LABEL > &resLabels, std::map<QString, QString> &mapDeviceLinkLabel)
{
	//1. 先登录 拿token
	STRU_DB_USERINFO resUser;
	if (!GetLogInfo(m_config->stConfig.szUser, m_config->stConfig.szPwd, resUser))
	{
		m_szToken = "";
		return false;
	}
	m_szToken = resUser.token;

	std::vector<STRU_DB_ORGANIZATION> resVtOrg;
	if (!GetOrganization(resVtOrg))
		return false;

	for (STRU_DB_ORGANIZATION & org : resVtOrg)
	{
		resOrg[org.uuid] = org;

		vCameraGroup.push_back(org);
		std::vector<STRU_DB_DEVICE> vDev;
		if (!GetDevices(org.uuid, vDev))
		{
			//todo log
		}
		else
		{
			for (STRU_DB_DEVICE & dev : vDev)
			{
				//组织关系和设备之间的关系表
				mapOrgLinkDevice[org.uuid + "/" + dev.deviceId] = dev.deviceId;
#if _DEFINE_CYCLE_
				STRU_DB_STREAMURL pInfo;
				pInfo.deviceId = dev.deviceId;
				bool bl = GetDeviceStreamUrl(pInfo.deviceId, "1", pInfo);
				bl = GetDeviceStreamUrl(pInfo.deviceId, "2", pInfo);

				dev.rtspUrl = pInfo.rtsp;
				dev.rtspSubUrl = pInfo.rtspSub;
				dev.zhCtlUrl = pInfo.ctrlUrl;
#endif
				//组织结构 添加设备
				STRU_DB_ORGANIZATION orgdev;
				orgdev.uuid = dev.deviceId;
				orgdev.name = dev.name;
				orgdev.type = dev.type;
				orgdev.parentId = dev.orgId;

				resOrg[orgdev.uuid] = orgdev;
				vCameraGroup.push_back(orgdev);

				resDevice[dev.deviceId] = dev;
				//
				//if (dev.type.toUtf8() == QString::fromUtf8("全景相机"))
				QString szref = QString::fromUtf8("全景相机");
				if (dev.type.toUtf8() == szref)
				{
					lsScene.push_back(dev);
				}
			}
		}
	}

	if (!GetLabelType(labelGroup))
	{
		return false;
	}

	std::vector<STRU_DB_LABEL> resLbl;
	if (GetLabels(resLbl))
	{
		for (STRU_DB_LABEL & lbl : resLbl)
		{
			if (GetLabelLinkDevice(lbl.labelId, lbl.deviceLinks))
			{
				for (STRU_DB_LABELLINKDEVICE &ll : lbl.deviceLinks)
				{
					mapDeviceLinkLabel[ll.id + "/" + lbl.labelId] = lbl.labelId;
				}
			}
			
			resLabels[lbl.labelId] = lbl;
			////todo ?? 拧巴
		}
	}

	return true;
}

#endif
