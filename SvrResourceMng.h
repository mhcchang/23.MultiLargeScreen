/*******************************************************************************
* @brief ��Զ�˻�ȡ��Դ  ���浽����  ������Դ������
* @detail  ÿ��������һ��
*		   ��Ҫʹ��rest�ӿڻ�ȡԶ����Դ �������ݿ⡢ͼ��ȵ�
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
	* @brief  ��ȡ���ݿ������Ϣ
	* @param  flag���ͱ�� �������ֻ�ȡ����  
	* @param  url ��Ӧ��rest�ӿڵ�ַ
	* @param  input ... ��Ӧ����������
	* @param[out] infos ... ��Ӧ��������ݽ�� ����
	* @return �ɹ�������ֵ ����0�� ʧ�ܷ��ظ�ֵ Ϊ������
	****************************************/
	//todo
	bool GetRemoteDbInfo(int flag, QString url /*, struct ... info, QMap<> infos*/);

	/****************************************!
	* @brief  ��ȡ��icon�����Ϣ
	* @param  url ��Ӧ��rest�ӿڵ�ַ
	* @param  input ... ��Ӧ����������
	* @param[out] infos ... ��Ӧ��������ݽ�� ����
	* @return �ɹ�������ֵ ����0�� ʧ�ܷ��ظ�ֵ Ϊ������
	****************************************/
	bool GetRemoteIconInfo(QString url);

	/****************************************!
	* @brief  ��ȡ�����Ϣ ���طַ� û�еĴ�Զ���ٻ�ȡһ��
	* @param  flag���ͱ�� �������ֻ�ȡ����
	* @param  input ... ��Ӧ����������
	* @param[out] infos ... ��Ӧ��������ݽ�� ����
	* @return �ɹ�������ֵ ����0�� ʧ�ܷ��ظ�ֵ Ϊ������
	****************************************/
	//todo
	bool GetLocalInfo(int flag /*, struct ... info, QMap<> infos*/);

	/****************************************!
	* @brief  ��ȡ��icon�����Ϣ
	* @param  iconId icon��id
	* @param[out] QImage �ɹ������image
	* @return �ɹ�����0�� ʧ�ܷ��ظ�ֵ Ϊ������
	****************************************/
	bool GetLocalIcon(int iconId, QImage &img);

	/****************************************!
	* @brief  ���ò��� todo....
	* @param  
	* @param[out] 
	* @return �ɹ�����0�� ʧ�ܷ��ظ�ֵ Ϊ������
	****************************************/
	void SetParam(HttpClientSvr* httpClientSvr);

	/****************************************!
	* @brief  ����rest�ӿڷ��ز��� todo....
	* @param  
	* @param[out] 
	* @return �ɹ�����0�� ʧ�ܷ��ظ�ֵ Ϊ������
	****************************************/
	void ParseMsg(QString str);
public:
	Config * m_config;
private:
	//label ��ͼ�� <��ǩ��, ͼ��>
	QMap<int, QImage> m_mapLblIcon;
	//.............. todo ���ݽṹ����
	//QMap<QString, struct> m_mapDBInfo;

	HttpClientSvr* m_httpClientSvr;
};
