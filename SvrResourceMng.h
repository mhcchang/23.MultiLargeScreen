/*******************************************************************************
* @brief 从远端获取资源  保存到本地  本地资源请求处理
* @detail  每次启动读一次
*		   主要使用rest接口获取远端资源 包括数据库、图标等等
* @author  zhuohe tech
* @date    1/7/2022
*******************************************************************************/
#pragma once
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QHttpMultiPart>
#include <QImage>
#include <QMap>
#include <QThread>

#include "PubDefs.h"
#include "PubUnit.h"
#include "Config.h"

#include "HttpClientSvr.h"

class SvrResourceMng : public QThread
{
	Q_OBJECT

public:
	SvrResourceMng(QObject *parent);
	~SvrResourceMng();

public:
	/****************************************!
	* @brief  获取数据库相关信息
	* @param  flag类型标记 用于区分获取数据  
	* @param  url 对应的rest接口地址
	* @param  input ... 对应的其他参数
	* @param[out] infos ... 对应的输出数据结果 链表
	* @return 成功返回正值 或者0， 失败返回负值 为错误码
	****************************************/
	//todo
	bool GetRemoteDbInfo(int flag, QString url /*, struct ... info, QMap<> infos*/);

	/****************************************!
	* @brief  获取数icon相关信息
	* @param  url 对应的rest接口地址
	* @param  input ... 对应的其他参数
	* @param[out] infos ... 对应的输出数据结果 链表
	* @return 成功返回正值 或者0， 失败返回负值 为错误码
	****************************************/
	bool GetRemoteIconInfo(QString url);

	/****************************************!
	* @brief  获取相关信息 本地分发 没有的从远端再获取一次
	* @param  flag类型标记 用于区分获取数据
	* @param  input ... 对应的其他参数
	* @param[out] infos ... 对应的输出数据结果 链表
	* @return 成功返回正值 或者0， 失败返回负值 为错误码
	****************************************/
	//todo
	bool GetLocalInfo(int flag /*, struct ... info, QMap<> infos*/);

	/****************************************!
	* @brief  获取数icon相关信息
	* @param  iconId icon的id
	* @param[out] QImage 成功的输出image
	* @return 成功返回0， 失败返回负值 为错误码
	****************************************/
	bool GetLocalIcon(int iconId, QImage &img);

	/****************************************!
	* @brief  设置参数 todo....
	* @param  
	* @param[out] 
	* @return 成功返回0， 失败返回负值 为错误码
	****************************************/
	void SetParam(HttpClientSvr* httpClientSvr);

	/****************************************!
	* @brief  解析rest接口返回参数 todo....
	* @param  
	* @param[out] 
	* @return 成功返回0， 失败返回负值 为错误码
	****************************************/
	void ParseMsg(QString str);
public:
	Config * m_config;
private:
	//label 的图标 <标签名, 图标>
	QMap<int, QImage> m_mapLblIcon;
	//.............. todo 数据结构待定
	//QMap<QString, struct> m_mapDBInfo;

	HttpClientSvr* m_httpClientSvr;
};
