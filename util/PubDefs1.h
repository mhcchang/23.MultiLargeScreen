#pragma once

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
	std::string id;
	std::string uuid;
	std::string name;
	std::string type;
	std::string actived;
	std::string description;
	uint64_t createTime;
	std::string createBy;
	uint64_t lastUpdateTime;
	std::string lastUpdateBy;
	std::string  lastUpdateDesc;

	void* userData;
	void* reserve;
} STRU_DB_ORGANIZATION, *PSTRU_DB_ORGANIZATION;
#else

typedef struct stru_DB_Contact {
	std::string id;
	std::wstring contactsName;
	std::string phoneNo;
	std::string idNum;
} STRU_DB_CONTACT, *PSTRU_DB_CONTACT;

typedef struct stru_DB_Organization {
	std::string uuid;
	std::wstring name;
	std::string type;
	std::wstring parentId;
	int vSort;
	uint64_t createTime;
	std::string createBy;
	uint64_t lastUpdateTime;
	std::string lastUpdateBy;
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
	std::string id;
	std::string uuid;
	std::string orgUUID;
	std::string name;
	std::string type;
	std::string subtype;
	int actived;
	int status;
	int channelCount;
	std::string serial;
	std::string manufacturer;
	std::string ip;
	int port;
	std::string username;
	std::string password;
	std::string protocol;
	float latitude;
	float longtitude;
	float elevation;
	float rolationAngle;
	float pitchAngle;
	bool hasPTZ;
	std::string ptzType;
	uint64_t createTime;
	std::string createBy;
	uint64_t lastUpdateTime;
	std::string lastUpdateBy;
	std::string  lastUpdateDesc;

	void* userData;
	void* reserve;

} STRU_DB_DEVICE, *PSTRU_DB_DEVICE;
#else
typedef struct stru_DB_Device {
	std::string deviceId;
	std::string sceneId;
	std::wstring name;
	std::string type;
	int channelCount;
	int status;
	std::string ip;
	int port;
	std::wstring username;
	std::string password;
	std::string protocol;
	std::wstring serial;
	std::wstring manufacturer;
	float latitude;
	float longtitude;
	float elevation;
	float rolationAngle;
	float pitchAngle;
	std::string location;
	std::string accessGateway;
	std::string thumb;

	bool hasPTZ;
	std::string ptzType;
	uint64_t createTime;
	std::string createBy;
	std::wstring contactsName;
	std::string phoneNo;
	std::string idNum;

	//相机流
	std::string rtspUrl;
	std::string rtspSubUrl;
	std::string zhCtlUrl;

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
	std::string roleId;
	std::wstring roleName;
	std::wstring roleDesc;
} STRU_DB_ROLE, *PSTRU_DB_ROLE;

#if 0
typedef struct stru_DB_UserInfo {
	int id;
	std::string username;
	std::string phoneNo;
	std::string fullname;
	int actived;
	std::vector<STRU_DB_ROLE> roles;
	std::string description;
} STRU_DB_USERINFO, *PSTRU_DB_USERINFO;
#else
typedef struct stru_DB_UserInfo {
	std::string role;
	std::wstring username;
	//int actived;
	std::vector<STRU_DB_ROLE> menus;
	std::string token;
} STRU_DB_USERINFO, *PSTRU_DB_USERINFO;
#endif

typedef struct stru_DB_MenuInfo {
	std::string id;
	std::wstring menuName;
	std::wstring parentId;
	std::string viewUrl;
	std::string icon;
	std::string sort;

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
	std::wstring name;
	int resType;
	int optType;
	int actived;
	std::wstring description;

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
//	std::string name;
//	int resType;
//	int optType;
//	int actived;
//	std::string roles;
//	std::string description;
//
//	//std::string createTime;
//	//int createBy;
//	//std::string lastUpdateTime;
//	//int lastUpdateBy;
//	//std::string lastUpdateDesc;
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
	std::string app;
	std::string streamId;
	std::string mediaServerId;
	std::string flv;
	std::string fmp4;
	std::string hls;
	std::string https_flv;
	std::string https_fmp4;
	std::string https_hls;
	std::string https_ts;
	std::string rtc;
	std::string rtmp;
	std::string rtmps;
	std::string rtsp;
	std::string rtsps;
	std::string ts;
	std::string ws_flv;
	std::string ws_fmp4;
	std::string ws_hls;
	std::string ws_ts;
	std::string wss_flv;
	std::string wss_fmp4;
	std::string wss_hls;
	std::string wss_ts;

	void* userData;
	void* reserve;
	stru_DB_StreamUrl() : userData(nullptr), reserve(nullptr) {};
} STRU_DB_STREAMURL, *PSTRU_DB_STREAMURL;
#else
typedef struct stru_DB_StreamUrl {
	std::string deviceId;
	std::string rtsp;
	std::string rtspSub;
	std::string ctrlUrl;
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

	int id;
	std::wstring name;
	std::string protocol;
	std::wstring context;
	std::string pan;
	std::string tilt;
	std::string zoom;

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
	std::string id;
	std::wstring name;
} STRU_DB_LABELLINKDEVICE, *PSTRU_DB_LABELLINKDEVICE;

//标签 
typedef struct stru_DB_Label {
	std::wstring sceneId;
	std::string labelId;
	std::wstring name;
	std::string parentId;
	std::wstring parentName;
	std::string flagType;
	std::string icon;
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
	std::string id;
	std::wstring name;
	std::string deviceId;
	float rolationAngle;
	float pitchAngle;
	int actived;
	std::wstring description;

	void* userData;
	void* reserve;
	stru_DB_Scene() : userData(nullptr), reserve(nullptr) {};
} STRU_DB_SCENE, *PSTRU_DB_SCENE;

////
//typedef struct stru_DB_StreamUrl {
//	std::string url;
//	std::string deviceId;
//	std::string rtsp;
//	std::string rtspsub;
//
//	void* userData;
//	void* reserve;
//	stru_DB_StreamUrl() : userData(nullptr), reserve(nullptr) {};
//} STRU_DB_SCENE, *PSTRU_DB_SCENE;
//

struct DectectMqInf {
	uint64_t ts;
	std::string exchange;
	std::string key;
	std::string tag;
	std::string msg;
	DectectMqInf() : ts(0u) {};
};

struct MqAlarmInfo {
	std::string msgType;
	std::string msgId;
	struct MsgData {

		std::string sceneId;
		std::string alertDateTime;
		std::string deviceIp;

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
			std::string algoType;
			std::string ruleType;
			std::string ruleName;
			std::string ruleDescription;
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
			std::string detectId;
			std::string trackId;
			std::string objectClass;
			std::string objectCategory;
			std::string detectImage;
			float azimuth;
			float elevation;
			float longitude;
			float latitude;
			std::string videoUrl;
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
			std::string objectCategory;
			int objectCount;
			std::string image;

			float azimuth;
			float elevation;
			float longitude;
			float latitude;
			std::string videoUrl;
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

#define M_MAX_SEG (64)

#ifndef EPSILON
#define EPSILON   (1e-10)
#endif 
//float
#ifndef EPSILON_FLOAT
#define EPSILON_FLOAT   (1e-6)
#endif 
