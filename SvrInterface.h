#pragma once

#pragma execution_character_set("utf-8")

#include "HttpClientSvr.h"
#include "PubDefs.h"
#include "Config.h"

#include"MD5.hpp"

class SvrInterface
{
public:
	SvrInterface();
	~SvrInterface();

	void SetParam(HttpClientSvr* httpClientSvr, Config* config, std::string ip, std::string szUriPath, int port, bool blUsessl = false);

	bool GetAllOrganization(std::map<QString, STRU_DB_ORGANIZATION> &resOrg, QList<STRU_DB_ORGANIZATION> &vCameraGroup, 
		std::map<QString, STRU_DB_DEVICE> &resDevice, QList<STRU_DB_DEVICE> &lsScene, std::map<QString, QString> &mapOrgLinkDevice, 
		std::vector<STRU_DB_LABELTYPE> &labelGroup, std::map<QString, STRU_DB_LABEL > &resLabels, std::map<QString, QString> &mapDeviceLinkLabel);

	//��¼��Ϣ ����ȫ���ɲ�������
	bool GetLogInfo(QString username, QString password, STRU_DB_USERINFO &res);
	//��ѯ ��Դ����
	bool GetResource(std::vector<STRU_DB_RESOURCE> &res);
	//��֯�ṹ
	bool GetOrganization(std::vector<STRU_DB_ORGANIZATION> &res);
	//�豸
	bool GetDevices(QString orgId, std::vector<STRU_DB_DEVICE> &res);
	//�豸����Ϣ
	bool GetDeviceStreamUrl(QString deviceId, QString channelId, STRU_DB_STREAMURL& pInfo);
	//Ԥ��λ
	bool GetPreset(QString deviceId, QString channelId, std::vector<STRU_DB_PRESET> &res);
	bool GotoPreset(QString deviceId, QString presetId);

	//��ǩ
#if 0
	bool GetLabels(std::string deviceId, std::string senceId, std::vector<STRU_DB_LABEL> &res, int count = 10000, int page = 0);
#else
	bool GetLabels(std::vector<STRU_DB_LABEL> &res, int count = 10000, int page = 0);
#endif
	//��ǩ��Ӧ���豸
	bool GetLabelLinkDevice(QString labelId, std::vector<STRU_DB_LABELLINKDEVICE> &res);

	bool GetLabelType(std::vector<STRU_DB_LABELTYPE> &res);
	//����
	bool GetScenes(std::vector<STRU_DB_SCENE> &res, int count = 10000, int page = 0);
	//== GetDeviceStreamUrl
	bool GetStreamUrl(QString deviceId, QString channel, STRU_DB_STREAMURL& pInfo);
	bool DomeCtrl(QString deviceId, int flag, int speed);

private:
	HttpClientSvr * m_httpClientSvr;
	QString m_szIp;
	QString m_szUriPath;
	int m_nPort;
	bool m_blUsessl;
	Config* m_config;

	QString m_szToken;
	uint64_t m_tmToken;
private:
	std::string GenMd5(std::string szSrc);

	bool ParseLogInfo(QString szRespond, STRU_DB_USERINFO &res);
	bool ParseResourceInfo(QString szRespond, std::vector<STRU_DB_RESOURCE> &res);

	//�ӿ�Ϊ�ݹ鷽ʽ
	bool ParseRecursiveOrganization(QJsonArray childValue, std::vector<STRU_DB_ORGANIZATION> &res);

	bool ParseOrganizationInfo(QString szRespond, std::vector<STRU_DB_ORGANIZATION> &res);
	bool ParseDevicesInfo(QString szRespond, std::vector<STRU_DB_DEVICE> &res);
	bool ParseDeviceStreamUrlInfo(QString szRespond, STRU_DB_STREAMURL &res, bool blSub = false);
	bool ParsePresetInfo(QString szRespond, std::vector<STRU_DB_PRESET> &res);

	//MatchLabelType Ŀǰ��֪������ʲô���� ���̶Ƚ��� ������C_STR_LABEL_TYPE ���涨��
	bool MatchLabelType(QString sz, int &type);
	//�ӿڶ��� java����������ͽṹ ��Ҫ��������
	bool ParseRecursiveLabel(QJsonArray childValue, std::vector<STRU_DB_LABEL> &res);

	bool ParseLabelsInfo(QString szRespond, std::vector<STRU_DB_LABEL> &res);
	bool ParseLabelLinkDeviceInfo(QString szRespond, std::vector<STRU_DB_LABELLINKDEVICE> &res);

	bool ParseLabelTypeInfo(QString szRespond, std::vector<STRU_DB_LABELTYPE> &res);

	bool ParseScenesInfo(QString szRespond, std::vector<STRU_DB_SCENE> &res);

	//bool ParseStreamUrl(QString szRespond, std::vector<STRU_DB_SCENE> &res);
	bool ParseDomeInf(QString szRespond);

	//ȱʡjson
	bool ParseDefaultInfo(QString szRespond, QString szReason);

};
