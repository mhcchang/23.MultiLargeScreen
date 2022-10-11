#include "HttpClientSvr.h"
#include <QDebug>
#include <QFile>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QMapIterator>

HttpClientSvr::HttpClientSvr(QObject *parent)
	: QThread(parent)
{
	m_manager = new QNetworkAccessManager();
	m_blRunning = false;
}

HttpClientSvr::~HttpClientSvr()
{
	QMap<QString, HttpClient*>::iterator it;

	for (it = m_mapClients.begin(); it != m_mapClients.end(), m_mapClients.size() > C_MAX_CLIENT; )
	{
		//HttpClient* clt = (HttpClient*)&it.value();
		delete &it.value();
	}
	m_mapClients.clear();

	delete m_manager;
}

bool HttpClientSvr::GetResponse(QString url, QMap<QString, QString> map, std::function<void(const QString &)> successHandler, std::function<void(const QString &)> errorHandler, bool blPush)
{
	HttpClient clt;
	clt.Init(url);
	clt.useManager(m_manager);
	QMap<QString, QString>::Iterator it;
	for (it = map.begin(); it != map.end(); it++)
	{
		clt.addParam(it.key(), it.value());
	}

	clt.get(successHandler, errorHandler);

	if (blPush)
	{
		m_mapClients.insert(url, &clt);
	}

	return true;
}

bool HttpClientSvr::PostRequest(QString url, QMap<QString, QString> map, std::function<void(const QString &)> successHandler, std::function<void(const QString &)> errorHandler, bool blPush)
{
	HttpClient clt;
	clt.Init(url);
	clt.useManager(m_manager);
	QMap<QString, QString>::Iterator it;
	for (it = map.begin(); it != map.end(); it++)
	{
		clt.addParam(it.key(), it.value());
	}
	clt.post(successHandler, errorHandler);

	if (blPush)
	{
		m_mapClients.insert(url, &clt);
	}

	return true;
}

bool HttpClientSvr::Download(QString url, QMap<QString, QString> map, std::function<void(const QByteArray &)> readyRead, std::function<void()> finishHandler, std::function<void(const QString &)> errorHandler, bool blPush)
{
	HttpClient clt;
	clt.Init(url);
	clt.useManager(m_manager);
	QMap<QString, QString>::Iterator it;
	for (it = map.begin(); it != map.end(); it++)
	{
		clt.addParam(it.key(), it.value());
	}

	clt.download(readyRead, finishHandler, errorHandler);
	if (blPush)
	{
		m_mapClients.insert(url, &clt);
	}

	return true;
}

bool HttpClientSvr::Upload(QString url, QMap<QString, QString> map, const QString &path, std::function<void(const QString &)> successHandler, std::function<void(const QString &)> errorHandler, bool blPush)
{
	HttpClient clt;
	clt.Init(url);
	clt.useManager(m_manager);

	QMap<QString, QString>::Iterator it;
	for (it = map.begin(); it != map.end(); it++)
	{
		clt.addParam(it.key(), it.value());
	}
	clt.upload(path, successHandler, errorHandler);

	if (blPush)
	{
		m_mapClients.insert(url, &clt);
	}

	return true;
}

//		// �ڴ������ִ��������ʣ���Ϊ�˲��� HttpClient �����ڱ�������������ʵĻص�������Ȼ������ִ��
//		// [[1]] GET �����޲���
//		HttpClient("http://localhost:8080/device").get([](const QString &response) {
//			qDebug() << response;
//		});
//
//		// [[2]] GET �����в��������Զ��� header
//		HttpClient("http://localhost:8080/signIn")
//			.addParam("id", "1")
//			.addParam("name", "�����")
//			.addHeader("token", "123AS#D")
//			.get([](const QString &response) {
//			qDebug() << response;
//		});
//
//		// [[3]] POST �����в��������Զ��� header
//		HttpClient("http://localhost:8080/signIn")
//			.addParam("id", "2")
//			.addParam("name", "����")
//			.addHeader("token", "DER#2J7")
//			.addHeader("content-type", "application/x-www-form-urlencoded")
//			.post([](const QString &response) {
//			qDebug() << response;
//		});
//
//		// [[4]] ÿ����һ�� QNetworkAccessManager ���󶼻ᴴ��һ���̣߳���Ƶ���ķ�������ʱ��Ϊ�˽�ʡ�߳���Դ������ useManager()
//		// ʹ�ù���� QNetworkAccessManager�������ᱻ HttpClient ɾ����
//		// �������Ĵ��벻���� QNetworkAccessManager�����������������Կ��������˼�ǧ���̡߳�
//		QNetworkAccessManager *manager = new QNetworkAccessManager();
//		for (int i = 0; i < 5000; ++i) {
//			HttpClient("http://localhost:8080/device").useManager(manager).get([=](const QString &response) {
//				qDebug() << response << ", " << i;
//			});
//		}
//
//		// [[5]] ����
//		QFile *file = new QFile("dog.png");
//		if (file->open(QIODevice::WriteOnly)) {
//			HttpClient("http://xtuer.github.io/img/dog.png").debug(true).download([=](const QByteArray &data) {
//				file->write(data);
//			}, [=] {
//				file->flush();
//				file->close();
//				file->deleteLater();
//
//				qDebug() << "Download file finished";
//			});
//		}
//
//		// [[6]] �ϴ�
//		HttpClient("http://localhost:8080/upload").upload("/Users/Biao/Pictures/ade.jpg");
//
//}

bool HttpClientSvr::Add(QString url)
{
	HttpClient clt;
	clt.Init(url);
	clt.useManager(m_manager);
	m_mapClients.insert(url, &clt);
	return true;
}

bool HttpClientSvr::Del(QString url)
{
	QMap<QString, HttpClient*>::iterator it = m_mapClients.find(url);
	if (it != m_mapClients.end())
		m_mapClients.erase(it);

	return true;
}

bool HttpClientSvr::Find(QString url)
{
	QMap<QString, HttpClient*>::iterator it = m_mapClients.find(url);
	return (it != m_mapClients.end());
}

QString HttpClientSvr::GetTxtBody(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead)
{
	HttpClient clt;
	clt.Init(url);
	clt.useManager(m_manager);
	//
	m_mapClients.insert(url, &clt);
	QMap<QString, QString>::Iterator it;
	for (it = mapParam.begin(); it != mapParam.end(); it++)
	{
		clt.addParam(it.key(), it.value());
	}

	for (it = mapHead.begin(); it != mapHead.end(); it++)
	{
		clt.addHeader(it.key(), it.value());
	}
	QString str;
	if (!clt.getTextBody(str, 0, postData))
	{
		//todo log
	}

	qDebug() << "------Get  : " << postData;
	qDebug() << "------Get  respond : " << str.toUtf8();

	return str;// QString(str.c_str());
}

/*
	QByteArray GetBinaryBody(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead);
	QString Post(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead);
	QByteArray PostRequestBinaryBody(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead);

*/
QByteArray HttpClientSvr::GetBinaryBody(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead)
{
	HttpClient clt;
	clt.Init(url);
	clt.useManager(m_manager);
	//
	m_mapClients.insert(url, &clt);
	QMap<QString, QString>::Iterator it;
	for (it = mapParam.begin(); it != mapParam.end(); it++)
	{
		clt.addParam(it.key(), it.value());
	}

	for (it = mapHead.begin(); it != mapHead.end(); it++)
	{
		clt.addHeader(it.key(), it.value());
	}
	//unsigned char* buf;
	//int len = 0;
	QByteArray ba;
	if (!clt.getBinaryBody(ba, 0))
	{
		//todo log
	}

	qDebug() << "------getBinary : " << postData;
	qDebug() << "------getBinary respond : " << ba;

	//QByteArray ba;
	//ba.resize(len);
	//memcpy(ba.data(), buf, len);
	return ba;
}

QString HttpClientSvr::Post(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead)
{
	HttpClient clt;
	clt.Init(url);
	clt.useManager(m_manager);
	//
	m_mapClients.insert(url, &clt);
	QMap<QString, QString>::Iterator it;
	for (it = mapParam.begin(); it != mapParam.end(); it++)
	{
		clt.addParam(it.key(), it.value());
	}

	for (it = mapHead.begin(); it != mapHead.end(); it++)
	{
		clt.addHeader(it.key(), it.value());
	}

	QString sz;
	if (!clt.getTextBody(sz, 1, postData))
	{
		//todo log
	}

	qDebug() << "------Post : " << postData;
	qDebug() << "------Post respond : " << sz;

	return sz;
}

QByteArray HttpClientSvr::PostRequestBinaryBody(QString url, QByteArray postData, QMap<QString, QString> mapParam, QMap<QString, QString> mapHead)
{
	HttpClient clt;
	clt.Init(url);
	clt.useManager(m_manager);
	//
	m_mapClients.insert(url, &clt);
	QMap<QString, QString>::Iterator it;
	for (it = mapParam.begin(); it != mapParam.end(); it++)
	{
		clt.addParam(it.key(), it.value());
	}

	for (it = mapHead.begin(); it != mapHead.end(); it++)
	{
		clt.addHeader(it.key(), it.value());
	}

	QByteArray ba;
	if (!clt.getBinaryBody(ba, 1, postData))
	{
		//todo log
	}
	
	qDebug() << "------PostBinary  : " << postData;
	qDebug() << "------PostBinary respond : " << ba;

	return ba;
}

HttpClient* HttpClientSvr::GetHttpClient(QString url)
{
	QMap<QString, HttpClient*>::iterator it = m_mapClients.find(url);
	if (it != m_mapClients.end())
	{
		return it.value();
	}
	else
		return nullptr;
}

void HttpClientSvr::StartSvr()
{
	m_blRunning = true;
	_sleep(10);
	//run�Ĳ�������qt �̲߳���Ҫ�Լ�����
	start();
}

void HttpClientSvr::StopSvr()
{
	m_blRunning = false;
}

///////////////////////////1 ���״̬�� 2ɾ�����ֵ ...
void HttpClientSvr::run()
{
	while (m_blRunning)
	{
		QMap<QString, HttpClient*>::iterator it;

		for (it = m_mapClients.begin(); it != m_mapClients.end(), m_mapClients.size() > C_MAX_CLIENT; )
		{
			HttpClient* clt = (HttpClient*)&it.value();
			if (clt->isFinished())
			{
				delete clt;
				m_mapClients.erase(it);
			}
			else
				it++;
		}
		//QMapIterator<QString, HttpClient> it(m_mapClients);
		//while (it.hasNext() && m_mapClients.size() > C_MAX_CLIENT)
		//{
		//	HttpClient* clt = (HttpClient*)&it.value();
		//	if (clt->isFinished())
		//		m_mapClients.erase(it);
		//	it.next();
		//}

		_sleep(1);
	}
	_sleep(10);
}
