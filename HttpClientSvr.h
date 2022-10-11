/***********************************************************************
* @brief qt httpclient��װ
* @detail ֧��ͬ���첽get�ı��Ͷ���������
* @author beijing zhuohe tech.
* @date 01/31/2022
***********************************************************************/

#pragma once

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QMap>
#include "HttpClient.h"

#define C_MAX_CLIENT (256)

typedef std::function<void(const QString &)> HttpHandle;

class HttpClientSvr : public QThread
{
	Q_OBJECT

public:
	HttpClientSvr(QObject *parent);
	~HttpClientSvr();

	bool GetResponse(QString url, QMap<QString, QString> map, std::function<void(const QString &)> successHandler, std::function<void(const QString &)> errorHandler = NULL, bool blPush= false);
	bool PostRequest(QString url, QMap<QString, QString> map, std::function<void(const QString &)> successHandler, std::function<void(const QString &)> errorHandler = NULL, bool blPush = false);
	bool Download(QString url, QMap<QString, QString> map, std::function<void(const QByteArray &)> readyRead, std::function<void()> finishHandler, std::function<void(const QString &)> errorHandler, bool blPush = false);
	bool Upload(QString url, QMap<QString, QString> map, const QString &path, std::function<void(const QString &)> successHandler, std::function<void(const QString &)> errorHandler = NULL, bool blPush = false);

	QString GetTxtBody(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead);
	QByteArray GetBinaryBody(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead);
	QString Post(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead);
	QByteArray PostRequestBinaryBody(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead);

	virtual void run() Q_DECL_OVERRIDE;

	/***************************
	* @brief ��ɾ��
	****************************/
	bool Add(QString url);
	bool Del(QString url);
	bool Find(QString url);
	HttpClient* GetHttpClient(QString url);

	void StartSvr();
	void StopSvr();
private:
	//ip + token, ... 
	QMap<QString, HttpClient*> m_mapClients;
	//���󶼻ᴴ��һ���̣߳���Ƶ���ķ�������ʱ��Ϊ�˽�ʡ�߳���Դ������ useManager()
	QNetworkAccessManager * m_manager;

	volatile bool m_blRunning;
	//std::function<void(const QString &)>
};
