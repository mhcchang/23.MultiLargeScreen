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

	//登录信息 包含全部可操作内容
	bool GetLogInfo(QString username, QString password, STRU_DB_USERINFO &res);
	//查询 资源类型
	bool GetResource(std::vector<STRU_DB_RESOURCE> &res);
	//组织结构
	bool GetOrganization(std::vector<STRU_DB_ORGANIZATION> &res);
	//设备
	bool GetDevices(QString orgId, std::vector<STRU_DB_DEVICE> &res);
	//设备流信息
	bool GetDeviceStreamUrl(QString deviceId, QString channelId, STRU_DB_STREAMURL& pInfo);
	//预置位
	bool GetPreset(QString deviceId, QString channelId, std::vector<STRU_DB_PRESET> &res);
	bool GotoPreset(QString deviceId, QString presetId);

	//标签
#if 0
	bool GetLabels(std::string deviceId, std::string senceId, std::vector<STRU_DB_LABEL> &res, int count = 10000, int page = 0);
#else
	bool GetLabels(std::vector<STRU_DB_LABEL> &res, int count = 10000, int page = 0);
#endif
	//标签对应的设备
	bool GetLabelLinkDevice(QString labelId, std::vector<STRU_DB_LABELLINKDEVICE> &res);

	bool GetLabelType(std::vector<STRU_DB_LABELTYPE> &res);
	//场景
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

	//接口为递归方式
	bool ParseRecursiveOrganization(QJsonArray childValue, std::vector<STRU_DB_ORGANIZATION> &res);

	bool ParseOrganizationInfo(QString szRespond, std::vector<STRU_DB_ORGANIZATION> &res);
	bool ParseDevicesInfo(QString szRespond, std::vector<STRU_DB_DEVICE> &res);
	bool ParseDeviceStreamUrlInfo(QString szRespond, STRU_DB_STREAMURL &res, bool blSub = false);
	bool ParsePresetInfo(QString szRespond, std::vector<STRU_DB_PRESET> &res);

	//MatchLabelType 目前不知道都有什么类型 最大程度解耦 数据在C_STR_LABEL_TYPE 里面定义
	bool MatchLabelType(QString sz, int &type);
	//接口定义 java侧给出的树型结构 需要重新整理
	bool ParseRecursiveLabel(QJsonArray childValue, std::vector<STRU_DB_LABEL> &res);

	bool ParseLabelsInfo(QString szRespond, std::vector<STRU_DB_LABEL> &res);
	bool ParseLabelLinkDeviceInfo(QString szRespond, std::vector<STRU_DB_LABELLINKDEVICE> &res);

	bool ParseLabelTypeInfo(QString szRespond, std::vector<STRU_DB_LABELTYPE> &res);

	bool ParseScenesInfo(QString szRespond, std::vector<STRU_DB_SCENE> &res);

	//bool ParseStreamUrl(QString szRespond, std::vector<STRU_DB_SCENE> &res);
	bool ParseDomeInf(QString szRespond);

	//缺省json
	bool ParseDefaultInfo(QString szRespond, QString szReason);

};
