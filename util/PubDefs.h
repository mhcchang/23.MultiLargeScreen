#pragma once

#pragma execution_character_set("utf-8")

#include <string>
#include <vector>
/*
{
    "eCode":0,
    "eNote":"ok",
    "msgType":"queryOrganization",
    "msgId":"12345",
    "msgData":{
        "size":1,
        "total":1,
        "list":[
            {
                "id":10,
                "uuid":"1111111111",
                "name":"大钟寺",
                "type":"地铁站",
                "actived":1,
                "description":"大钟寺地铁站",
                "createTime":"2021-12-12 12:12:12.256",
                "createBy":1,
                "lastUpdateTime":"2021-12-14 12:12:12.256",
                "lastUpdateBy":1,
                "lastUpdateDesc":"更新描述"
            }
        ]
    }
}
*/
#if 0
typedef struct stru_DB_Organization {
	QString id;
	QString uuid;
	QString name;
	QString type;
	QString actived;
	QString description;
	uint64_t createTime;
	QString createBy;
	uint64_t lastUpdateTime;
	QString lastUpdateBy;
	QString  lastUpdateDesc;

	void* userData;
	void* reserve;
} STRU_DB_ORGANIZATION, *PSTRU_DB_ORGANIZATION;
#else

typedef struct stru_DB_Contact {
	QString id;
	QString contactsName;
	QString phoneNo;
	QString idNum;
} STRU_DB_CONTACT, *PSTRU_DB_CONTACT;

typedef struct stru_DB_Organization {
	QString uuid;
	QString name;
	QString type;
	QString parentId;
	int vSort;
	uint64_t createTime;
	QString createBy;
	uint64_t lastUpdateTime;
	QString lastUpdateBy;
	std::vector<STRU_DB_CONTACT> contacts;

	void* userData;
	void* reserve;
} STRU_DB_ORGANIZATION, *PSTRU_DB_ORGANIZATION;

#endif

/* from platform
{
	"eCode":0,
		"eNote" : "ok",
		"msgType" : "queryDevice",
		"msgId" : "12345",
		"msgData" : {
			"size":1,
			"total" : 1,
			"list" : [
				{
				"id":10,
				"uuid" : "1111111111",
				"orgUUID" : "00000000",
				"name" : "大钟寺A口",
				"type" : "监控相机",
				"subtype" : "高点相机",
				"actived" : 1,
				"status" : 1,
				"channelCount" : 1,
				"serial" : "AD5-68A",
				"manufacturer" : "海康威视",
				"ip" : "10.16.100.16",
				"port" : 1,
				"username" : "admin",
				"password" : "admin",
				"protocol" : "GB28181",
				"latitude" : 26.56,
				"langtitude" : 39.58,
				"elevation" : 1270.35,
				"rolationAngle" : 270,
				"pitchAngle" : -38.25,
				"hasPTZ" : true,
				"ptzType" : "GB28181",
				"createTime" : "2021-12-12 12:12:12.256",
				"createBy" : 1,
				"lastUpdateTime" : "2021-12-12 12:12:12.256",
				"lastUpdateBy" : 1,
				"lastUpdateDesc" : "修改名称"
				}
			]
	}
}
*/

#if 0
typedef struct stru_DB_Device {
	QString id;
	QString uuid;
	QString orgUUID;
	QString name;
	QString type;
	QString subtype;
	int actived;
	int status;
	int channelCount;
	QString serial;
	QString manufacturer;
	QString ip;
	int port;
	QString username;
	QString password;
	QString protocol;
	float latitude;
	float longtitude;
	float elevation;
	float rolationAngle;
	float pitchAngle;
	bool hasPTZ;
	QString ptzType;
	uint64_t createTime;
	QString createBy;
	uint64_t lastUpdateTime;
	QString lastUpdateBy;
	QString  lastUpdateDesc;

	void* userData;
	void* reserve;

} STRU_DB_DEVICE, *PSTRU_DB_DEVICE;
#else
typedef struct stru_DB_Device {
	QString deviceId;
	QString sceneId;
	QString name;
	QString type;
	int channelCount;
	int status;
	QString ip;
	int port;
	QString username;
	QString password;
	QString protocol;
	QString serial;
	QString manufacturer;
	QString orgId;
	float latitude;
	float longtitude;
	float elevation;
	float rolationAngle;
	float pitchAngle;
	QString location;
	QString accessGateway;
	QString thumb;

	bool hasPTZ;
	QString ptzType;
	uint64_t createTime;
	QString createBy;
	QString contactsName;
	QString phoneNo;
	QString idNum;

	//相机流
	QString rtspUrl;
	QString rtspSubUrl;
	QString zhCtlUrl;

	void* userData;
	void* reserve;

} STRU_DB_DEVICE, *PSTRU_DB_DEVICE;
#endif

/*
	"id": 10,
	"username": "xiaoming",
	"phoneNo": "1111111111",
	"fullname": "xiaoming",
	"actived": 1,
	"roles": "100,101,102,103,105",
	"description": "值班员",
	"createTime": "2021-12-12 12:12:12.256",
	"createBy": 1,
	"lastUpdateTime": "2021-12-14 12:12:12.256",
	"lastUpdateBy": 1,
	"lastUpdateDesc": "增加105角色"
*/

typedef struct stru_DB_Role {
	QString roleId;
	QString roleName;
	QString roleDesc;
} STRU_DB_ROLE, *PSTRU_DB_ROLE;

#if 0
typedef struct stru_DB_UserInfo {
	int id;
	QString username;
	QString phoneNo;
	QString fullname;
	int actived;
	std::vector<STRU_DB_ROLE> roles;
	QString description;
} STRU_DB_USERINFO, *PSTRU_DB_USERINFO;
#else
typedef struct stru_DB_UserInfo {
	QString role;
	QString username;
	//int actived;
	std::vector<STRU_DB_ROLE> menus;
	QString token;
} STRU_DB_USERINFO, *PSTRU_DB_USERINFO;
#endif

typedef struct stru_DB_MenuInfo {
	QString id;
	QString menuName;
	QString parentId;
	QString viewUrl;
	QString icon;
	QString sort;

	void* userData;
	void* reserve;
}STRU_DB_MENUINFO, *PSTRU_DB_MENUINFO;

/*
	"id": 100,
	"name": "控制无人机云台",
	"resType": 100,
	"optType": 100,
	"actived": 1,
	"description": "控制无人机云台的权限",
	"createTime": "2021-12-12 12:12:12.256",
	"createBy": 1,
	"lastUpdateTime": "2021-12-13 12:12:12.256",
	"lastUpdateBy": 1,
	"lastUpdateDesc": "更改描述"
*/
//资源
typedef struct stru_DB_Resource {
	int id;
	QString name;
	int resType;
	int optType;
	int actived;
	QString description;

	void* userData;
	void* reserve;
	stru_DB_Resource() : userData(nullptr), reserve(nullptr) {};
} STRU_DB_RESOURCE, *PSTRU_DB_RESOURCE;

/*
	"id": 10,
	"username": "xiaoming",
	"phoneNo": "1111111111",
	"fullname": "xiaoming",
	"actived": 1,
	"roles":
	[
		{
			"roleId": 100,
			"roleName": "设备管理",
			"roleDesc": "管理设备(增删改查)的权限"
		},
		{
			"roleId": 101,
			"roleName": "组织管理",
			"roleDesc": "管理组织(增删改查)的权限"
		},
		{
			"roleId": 102,
			"roleName": "设备点播",
			"roleDesc": "预览设备码流的权限"
		},
		{
			"roleId": 105,
			"roleName": "设备回放",
			"roleDesc": "回放设备录像的权限"
		}
	],
	"description": "值班员",
	"createTime": "2021-12-12 12:12:12.256",
	"createBy": 1,
	"lastUpdateTime": "2021-12-14 12:12:12.256",
	"lastUpdateBy": 1,
	"lastUpdateDesc": "增加105角色"

*/

//typedef struct stru_DB_LoginAuthList {
//	int id;
//	QString name;
//	int resType;
//	int optType;
//	int actived;
//	QString roles;
//	QString description;
//
//	//QString createTime;
//	//int createBy;
//	//QString lastUpdateTime;
//	//int lastUpdateBy;
//	//QString lastUpdateDesc;
//
//} STRU_DB_USERINFO, *PSTRU_DB_USERINFO;

/*
		"app": "",
		"streamId": "",
		"mediaServerId": "",
		"flv": "",
		"fmp4": "",
		"hls": "",
		"https_flv": "",
		"https_fmp4": "",
		"https_hls": "",
		"https_ts": "",
		"rtc": "",
		"rtmp": "",
		"rtmps": "",
		"rtsp": "",
		"rtsps": "",
		"tracks": [],
		"transactionInfo": {
			"branch": "",
			"callId": "",
			"localTag": "",
			"remoteTag": ""
		},
		"ts": "",
		"ws_flv": "",
		"ws_fmp4": "",
		"ws_hls": "",
		"ws_ts": "",
		"wss_flv": "",
		"wss_fmp4": "",
		"wss_hls": "",
		"wss_ts": ""

*/
//流地址

#if 0
typedef struct stru_DB_StreamUrl {
	QString app;
	QString streamId;
	QString mediaServerId;
	QString flv;
	QString fmp4;
	QString hls;
	QString https_flv;
	QString https_fmp4;
	QString https_hls;
	QString https_ts;
	QString rtc;
	QString rtmp;
	QString rtmps;
	QString rtsp;
	QString rtsps;
	QString ts;
	QString ws_flv;
	QString ws_fmp4;
	QString ws_hls;
	QString ws_ts;
	QString wss_flv;
	QString wss_fmp4;
	QString wss_hls;
	QString wss_ts;

	void* userData;
	void* reserve;
	stru_DB_StreamUrl() : userData(nullptr), reserve(nullptr) {};
} STRU_DB_STREAMURL, *PSTRU_DB_STREAMURL;
#else
typedef struct stru_DB_StreamUrl {
	QString deviceId;
	QString rtsp;
	QString rtspSub;
	QString ctrlUrl;
	void* userData;
	void* reserve;
	stru_DB_StreamUrl() : userData(nullptr), reserve(nullptr) {};
} STRU_DB_STREAMURL, *PSTRU_DB_STREAMURL;

#endif
/*
	"id": 1,
	"name": "preset_1",
	"protocol": "GB28181",
	"context": "12",
	"pan": "25.64",
	"tilt": "33.23",
	"zoom": "6.0",
	"createTime": "2021-12-12 12:12:12.256",
	"createBy": 1,
	"lastUpdateTime": "2021-12-14 12:12:12.256",
	"lastUpdateBy": 1,
	"lastUpdateDesc": "更新交互协议"
*/

//预置位
typedef struct stru_DB_Preset {

	QString id;
	QString name;
	QString protocol;
	QString context;
	QString pan;
	QString tilt;
	QString zoom;

	void* userData;
	void* reserve;

	stru_DB_Preset() : userData(nullptr), reserve(nullptr) {};
} STRU_DB_PRESET, *PSTRU_DB_PRESET;

/*
	"id": "",
	"name": "",
	"type": 0,
	"xPos": 0.0,
	"yPos": 0.0,
	"createTime": "",
	"updateTime": ""
*/

/*
"deviceId": "",
"deviceName" : ""
*/
typedef struct stru_DB_LabelLinkDevice {
	QString id;
	QString name;
} STRU_DB_LABELLINKDEVICE, *PSTRU_DB_LABELLINKDEVICE;

//标签 
typedef struct stru_DB_Label {
	QString sceneId;
	QString labelId;
	QString name;
	QString parentId;
	QString parentName;
	QString flagType;
	QString icon;
	bool blHasChild;
	int type;
	float xPos;
	float yPos;

	std::vector<STRU_DB_LABELLINKDEVICE> deviceLinks;

	void* userData;
	void* userData1;
	void* reserve;

	stru_DB_Label() : userData(nullptr), userData1(nullptr), reserve(nullptr), xPos(-1), yPos(-1), blHasChild(false){};
} STRU_DB_LABEL, *PSTRU_DB_LABEL;

typedef struct stru_DB_LabelType {
	QString groupid;
	QString typeGroupname;
	QString flagName;
	QString flagType;
	QString icon;

	void* userData;
	void* userData1;
	void* reserve;

	stru_DB_LabelType() : userData(nullptr), userData1(nullptr), reserve(nullptr){};
} STRU_DB_LABELTYPE, *PSTRU_DB_LABELTYPE;

/*
	"id": 123,
	"name": "",
	"deviceId": "",
	"rolationAngle": 0.0,
	"pitchAngle": 0.0,
	"actived": 1,
	"description": "",
	"createTime": "",
	"createBy": 1,
	"lastUpdateTime": "",
	"lastUpdateBy": 1,
	"lastUpdateDesc": ""
*/

//场景
typedef struct stru_DB_Scene {
	QString id;
	QString name;
	QString deviceId;
	float rolationAngle;
	float pitchAngle;
	int actived;
	QString description;

	void* userData;
	void* reserve;
	stru_DB_Scene() : userData(nullptr), reserve(nullptr) {};
} STRU_DB_SCENE, *PSTRU_DB_SCENE;

////
//typedef struct stru_DB_StreamUrl {
//	QString url;
//	QString deviceId;
//	QString rtsp;
//	QString rtspsub;
//
//	void* userData;
//	void* reserve;
//	stru_DB_StreamUrl() : userData(nullptr), reserve(nullptr) {};
//} STRU_DB_SCENE, *PSTRU_DB_SCENE;
//

struct DectectMqInf {
	uint64_t ts;
	QString exchange;
	QString key;
	QString tag;
	QString msg;
	DectectMqInf() : ts(0u) {};
};

struct MqAlarmInfo {
	QString msgType;
	QString msgId;
	struct MsgData {

		QString sceneId;
		QString alertDateTime;
		QString deviceIp;

		struct Point {
			float x;
			float y;
			Point() : x(0.0), y(0.0) {};
			//Point& operator=(const Point& r)
			//{
			//	this->x = r.x;
			//	this->y = r.y;
			//	return *this;
			//};
			//Point(const Point& r) { *this = r; };
		};
		struct stru_rule {
			int ruleId;
			QString algoType;
			QString ruleType;
			QString ruleName;
			QString ruleDescription;
			int direction;
			int place;
			int detectLinePoint;

			std::vector<Point> detectLine;
			std::vector<Point> detectLine_16x9;
		};
		stru_rule rule;

		Point targetPoint;
		Point targetPoint_16x9;

		struct stru_detectInfo {

			uint64_t timestamp;
			QString detectId;
			QString trackId;
			QString objectClass;
			QString objectCategory;
			QString detectImage;
			float azimuth;
			float elevation;
			float longitude;
			float latitude;
			QString videoUrl;
			struct stru_coord {
				float x;
				float y;
				float w;
				float h;
				float pan;
				float tilt;
				float zoom;
			};
			stru_coord coord;
			stru_coord coord_16x9;
		};
		stru_detectInfo detectInfo;

		struct stru_densityInfo {

			uint64_t timestamp;
			int sensitivity;
			QString objectCategory;
			int objectCount;
			QString image;

			float azimuth;
			float elevation;
			float longitude;
			float latitude;
			QString videoUrl;
			struct stru_coord {
				float x;
				float y;
				float w;
				float h;
				float pan;
				float tilt;
				float zoom;
			};
			stru_coord coord;
			stru_coord coord_16x9;
		};
		stru_densityInfo densityInfo;

		//MsgData::~MsgData() { };
		//MsgData(const MsgData& t) { *this = t; };
	};
	MsgData msgData;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

struct UiLocalStatus {
	QString user;
	QString sceneId;
	struct SceneInfo {
		QString sceneId;
		int count;
		QStringList cameraIds;
		QStringList targets;
		QStringList cameras;
	};
	QList<SceneInfo> scenes;
};

#define M_MAX_SEG (9)

#ifndef EPSILON
#define EPSILON   (1e-10)
#endif 
//float
#ifndef EPSILON_FLOAT
#define EPSILON_FLOAT   (1e-6)
#endif 
