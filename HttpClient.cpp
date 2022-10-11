
#include "HttpClient.h"
#include <thread>
#include <chrono>

#include <QDebug>
#include <QFile>
#include <QHash>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QHttpPart>
#include <QHttpMultiPart>

#include <QEventLoop>

struct HttpClientPrivate 
{
	HttpClientPrivate(const QString &url) : url(url), networkAccessManager(NULL), useInternalNetworkAccessManager(true), debug(false) {}

	QString url; // ����� URL
	QUrlQuery params; // ����Ĳ���
	QHash<QString, QString> headers; // �����ͷ
	QNetworkAccessManager *networkAccessManager;
	bool useInternalNetworkAccessManager; // �Ƿ�ʹ���ڲ��� QNetworkAccessManager
	bool debug;
};

HttpClient::HttpClient(QObject *parent) : QObject(parent), d(nullptr), m_blFinished(false)
{
}

//HttpClient::HttpClient(QObject *parent, const QString &url) : QObject(parent), d(new HttpClientPrivate(url)), m_blFinished(false)
//{
//}

HttpClient::~HttpClient() 
{
	delete d;
}

void HttpClient::Init(const QString &url)
{
	d = new HttpClientPrivate(url);
}

HttpClient &HttpClient::useManager(QNetworkAccessManager *manager) 
{
	if (!d)
		return *this;

	d->networkAccessManager = manager;
	d->useInternalNetworkAccessManager = false;
	return *this;
}

// ���� debug Ϊ true ��ʹ�� debug ģʽ������ִ��ʱ�������� URL �Ͳ�����
HttpClient &HttpClient::debug(bool debug) 
{
	if (!d)
		return *this;
	d->debug = debug;
	return *this;
}

// ���Ӳ���
HttpClient &HttpClient::addParam(const QString &name, const QString &value) 
{
	if (!d)
		return *this;
	d->params.addQueryItem(name, value);
	return *this;
}

// ���ӷ���ͷ
HttpClient &HttpClient::addHeader(const QString &header, const QString &value) 
{
	if (!d)
		return *this;
	d->headers[header] = value;
	return *this;
}

HttpClient &HttpClient::addFormHeader() 
{
	return addHeader("content-type", "application/x-www-form-urlencoded");
}

// ִ�� GET ����
void HttpClient::get(std::function<void(const QString &)> successHandler,
	std::function<void(const QString &)> errorHandler, const char *encoding) 
{
	execute(false, successHandler, errorHandler, encoding);
}

void HttpClient::SuccessBinaryImp(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray ba = reply->readAll();
		m_baGet.resize(ba.size());
		memcpy(m_baGet.data(), ba.data(), ba.size());
	}

	// �ͷ���Դ
	reply->deleteLater();
	if (d->useInternalNetworkAccessManager)
	{
		QNetworkAccessManager * manager = d->networkAccessManager;
		manager->deleteLater();
	}

	m_blFinished = true;
}

void HttpClient::SuccessStringImp(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray ba = reply->readAll();
		m_baGet.resize(ba.size());
		memcpy(m_baGet.data(), ba.data(), ba.size());
	}

	// �ͷ���Դ
	reply->deleteLater();
	if (d->useInternalNetworkAccessManager)
	{
		QNetworkAccessManager * manager = d->networkAccessManager;
		manager->deleteLater();
	}

	m_blFinished = true;
}

#if  0
// ִ�� GET ���� ͬ��
bool HttpClient::getTextBody(QString &str, int nTimeout, const char *encoding)
{
	if (!d)
		return false;
	// ����� GET ���󣬲��Ҳ�����Ϊ�գ����������Ĳ������ŵ� URL ����
	if (!d->params.isEmpty())
	{
		d->url += "?" + d->params.toString(QUrl::FullyEncoded);
	}

	if (d->debug)
	{
		qDebug() << QString("URL: %1?%2").arg(d->url).arg(d->params.toString());
	}

	QUrl urlx(d->url);
	QNetworkRequest request(urlx);

	m_blFinished = false;

	// �������ͷ��ӵ� request ��
	QHashIterator<QString, QString> iter(d->headers);
	while (iter.hasNext())
	{
		iter.next();
		request.setRawHeader(iter.key().toUtf8(), iter.value().toUtf8());
	}

	// ע��: ������ Lambda ���ʽ��ʹ�� HttpClient ����ĳ�Ա���ݣ�����������������δ����ʱ���Ѿ����������ˣ�
	// �������Ҫʹ������������ݣ�����һ���ֲ����������������ݣ�Ȼ���� Lambda ���ʽ���������ֲ�����

	// �����ʹ���ⲿ�� manager �򴴽�һ���µģ��ڷ�����ɺ���Զ�ɾ����
	bool internal = true; // d->useInternalNetworkAccessManager;
	QNetworkAccessManager * manager = internal ? new QNetworkAccessManager() : d->networkAccessManager;
	QNetworkReply *reply = manager->get(request);
	QEventLoop eventLoop;
	connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));

	if (reply->error() == QNetworkReply::NoError)
	{
		qDebug() << "request protobufHttp NoError";
	}
	else
	{
		qDebug() << "request protobufHttp handle errors here";
		QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		//statusCodeV��HTTP����������Ӧ�룬reply->error()��Qt����Ĵ����룬���Բο�QT���ĵ�
		qDebug("request protobufHttp found error ....code: %d %d\n", statusCodeV.toInt(), (int)reply->error());
		qDebug(qPrintable(reply->errorString()));
		if (statusCodeV != "200")
		{
			reply->deleteLater();
			if (internal)
				manager->deleteLater();
			return false;
		}
	}
	// �������ʱһ���Զ�ȡ������Ӧ����
	//QObject::connect(reply, &QNetworkReply::finished, this, &HttpClient::SuccessStringImp, Qt::UniqueConnection);
	//QObject::connect(manager, &QNetworkAccessManager::finished, this, &HttpClient::SuccessStringImp, Qt::UniqueConnection);
	// ���������
	QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
		//if (NULL != errorHandler)
		{
			//todo log	errorHandler(reply->errorString());
			m_blFinished = true;
			return false;
		}
	});

	uint64_t tms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	uint64_t tme = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	//while (!m_blFinished && tme - tms < nTimeout)
	//{
	//	str = m_strGet;
	//	tme = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	//}
	str = reply->readAll();

	return true;
}
#else
bool HttpClient::getTextBody(QString &str, int requestFlag, QByteArray postData, int nTimeout, const char *encoding)
{
	//����url
	if (!d)
		return false;
	// ����� GET ���󣬲��Ҳ�����Ϊ�գ����������Ĳ������ŵ� URL ����
	if (!d->params.isEmpty())
	{
		d->url += "?" + d->params.toString(QUrl::FullyEncoded);
	}

	if (d->debug)
	{
		qDebug() << QString("URL: %1?%2").arg(d->url).arg(d->params.toString());
	}

	QUrl urlx(d->url);
	//QNetworkRequest request(urlx);
	QNetworkRequest request;

	request.setUrl(QUrl(d->url));

	//QByteArray postData = "";
	//int postlenth = postData.length();

	//����post����    
	QNetworkRequest::KnownHeaders;
	QHashIterator<QString, QString> iter(d->headers);
	while (iter.hasNext())
	{
		iter.next();
		request.setRawHeader(iter.key().toUtf8(), iter.value().toUtf8());
	}
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	//m_request.setHeader(QNetworkRequest::ContentLengthHeader, postlenth);
	//pReply = manager->post(request, postData);

	QNetworkAccessManager * pHttpMgr = new QNetworkAccessManager();
	QNetworkReply * pReply;
	QEventLoop eventLoop;

	if (requestFlag == 0)
		pReply = pHttpMgr->get(request);
	else
		pReply = pHttpMgr->post(request, postData);

	connect(pReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
	eventLoop.exec();     

	if (pReply->error() == QNetworkReply::NoError)
	{
		qDebug() << "request protobufHttp NoError";
	}
	else
	{
		qDebug() << "request protobufHttp handle errors here";
		QVariant statusCodeV = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		//statusCodeV��HTTP����������Ӧ�룬reply->error()��Qt����Ĵ����룬���Բο�QT���ĵ�
		qDebug("request protobufHttp found error ....code: %d %d\n", statusCodeV.toInt(), (int)pReply->error());
		qDebug(qPrintable(pReply->errorString()));
		if (statusCodeV != "200")
		{
			pReply->deleteLater();
			pHttpMgr->deleteLater();
			return false;
		}
	}
	
	//���󷵻صĽ��
	//str = pReply->readAll();
	str = QString::fromLocal8Bit(pReply->readAll());
	pReply->deleteLater();
	pHttpMgr->deleteLater();

	return true;
}
#endif

#if 0
bool HttpClient::getBinaryBody(int requestFlag, QByteArray &ba, QByteArray postData, int nTimeout, const char *encoding)
{
	if (!d)
		return false;
	// ����� GET ���󣬲��Ҳ�����Ϊ�գ����������Ĳ������ŵ� URL ����
	if (!d->params.isEmpty())
	{
		d->url += "?" + d->params.toString(QUrl::FullyEncoded);
	}

	if (d->debug)
	{
		qDebug() << QString("URL: %1?%2").arg(d->url).arg(d->params.toString());
	}

	QUrl urlx(d->url);
	QNetworkRequest request(urlx);

	m_blFinished = false;

	// �������ͷ��ӵ� request ��
	QHashIterator<QString, QString> iter(d->headers);
	while (iter.hasNext())
	{
		iter.next();
		request.setRawHeader(iter.key().toUtf8(), iter.value().toUtf8());
	}

	// ע��: ������ Lambda ���ʽ��ʹ�� HttpClient ����ĳ�Ա���ݣ�����������������δ����ʱ���Ѿ����������ˣ�
	// �������Ҫʹ������������ݣ�����һ���ֲ����������������ݣ�Ȼ���� Lambda ���ʽ���������ֲ�����

	// �����ʹ���ⲿ�� manager �򴴽�һ���µģ��ڷ�����ɺ���Զ�ɾ����
	bool internal = d->useInternalNetworkAccessManager;
	QNetworkAccessManager *manager = internal ? new QNetworkAccessManager() : d->networkAccessManager;
	QNetworkReply * reply;
	if (requestFlag == 0)
		reply = manager->get(request);
	else
		reply = manager->post(request, postData);

	// �������ʱһ���Զ�ȡ������Ӧ����
	QObject::connect(manager, &QNetworkAccessManager::finished, this, &HttpClient::SuccessBinaryImp, Qt::UniqueConnection);
	// ���������
	QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
		//if (NULL != errorHandler)
		{
			//todo log	errorHandler(reply->errorString());
			m_blFinished = true;
		}
	});

	uint64_t tms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	uint64_t tme = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	while (!m_blFinished && tme - tms < nTimeout)
	{
		ba.resize(m_baGet.size());
		memcpy(ba.data(), m_baGet.data(), m_baGet.size());

		tme = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return true;
}
#else
bool HttpClient::getBinaryBody(QByteArray &ba, int requestFlag, QByteArray postData, int nTimeout, const char *encoding)
{
	if (!d)
		return false;
	// ����� GET ���󣬲��Ҳ�����Ϊ�գ����������Ĳ������ŵ� URL ����
	if (!d->params.isEmpty())
	{
		d->url += "?" + d->params.toString(QUrl::FullyEncoded);
	}

	if (d->debug)
	{
		qDebug() << QString("URL: %1?%2").arg(d->url).arg(d->params.toString());
	}

	QUrl urlx(d->url);
	//QNetworkRequest request(urlx);
	QNetworkRequest request;

	request.setUrl(QUrl(d->url));

	//QByteArray postData = "";
	//int postlenth = postData.length();

	//����post����    
	QNetworkRequest::KnownHeaders;
	QHashIterator<QString, QString> iter(d->headers);
	while (iter.hasNext())
	{
		iter.next();
		request.setRawHeader(iter.key().toUtf8(), iter.value().toUtf8());
	}
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	//m_request.setHeader(QNetworkRequest::ContentLengthHeader, postlenth);
	//pReply = manager->post(request, postData);

	QNetworkAccessManager * pHttpMgr = new QNetworkAccessManager();
	QNetworkReply * pReply;
	//����¼�ѭ�����ƣ����غ������к����
	QEventLoop eventLoop;
	//pReply = pHttpMgr->get(request);
	if (requestFlag == 0)
		pReply = pHttpMgr->get(request);
	else
		pReply = pHttpMgr->post(request, postData);

	connect(pReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
	eventLoop.exec();       //block until finish
							//������
	if (pReply->error() == QNetworkReply::NoError)
	{
		qDebug() << "request protobufHttp NoError";
	}
	else
	{
		qDebug() << "request protobufHttp handle errors here";
		QVariant statusCodeV = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		//statusCodeV��HTTP����������Ӧ�룬reply->error()��Qt����Ĵ����룬���Բο�QT���ĵ�
		qDebug("request protobufHttp found error ....code: %d %d\n", statusCodeV.toInt(), (int)pReply->error());
		qDebug(qPrintable(pReply->errorString()));
		if (statusCodeV != "200")
		{
			pReply->deleteLater();
			pHttpMgr->deleteLater();
			return false;
		}
	}
	//���󷵻صĽ��
	ba = pReply->readAll();
	//QByteArray data = pReply->readAll();
	/**
	{
	"ip": "192.168.1.100",
	"gateway": "192.168.1.1",
	"submask": "255.255.255.0",
	"dns1": "8.8.8.8",
	"dns2": "8.8.8.8",
	"mac": "00:B5:7B:B4:EC:FB"
}
*/
	pReply->deleteLater();
	pHttpMgr->deleteLater();

	return true;
}

#endif
// ִ�� POST ����
void HttpClient::post(std::function<void(const QString &)> successHandler,
	std::function<void(const QString &)> errorHandler, const char *encoding)
{
	execute(true, successHandler, errorHandler, encoding);
}

// ʹ�� GET �������أ��������ݿɶ�ȡʱ�ص� readyRead(), ����������Ӧ���� readyRead() ������ݱ��浽�ļ�
void HttpClient::download(std::function<void(const QByteArray &)> readyRead,
	std::function<void()> finishHandler, std::function<void(const QString &)> errorHandler)
{
	if (!d)
		return;
	// ����� GET ���󣬲��Ҳ�����Ϊ�գ����������Ĳ������ŵ� URL ����
	if (!d->params.isEmpty()) 
	{
		d->url += "?" + d->params.toString(QUrl::FullyEncoded);
	}

	if (d->debug) 
	{
		qDebug() << QString("URL: %1?%2").arg(d->url).arg(d->params.toString());
	}

	m_blFinished = false;
	QUrl urlx(d->url);
	QNetworkRequest request(urlx);
	bool internal = d->useInternalNetworkAccessManager;
	QNetworkAccessManager *manager = internal ? new QNetworkAccessManager() : d->networkAccessManager;
	QNetworkReply *reply = manager->get(request);

	// �����ݿɶ�ȡʱ�ص� readyRead()
	QObject::connect(reply, &QNetworkReply::readyRead, [=] {
		readyRead(reply->readAll());
	});

	// �������
	QObject::connect(reply, &QNetworkReply::finished, [=] {
		if (reply->error() == QNetworkReply::NoError && NULL != finishHandler) {
			finishHandler();
			m_blFinished = true;
		}

		// �ͷ���Դ
		reply->deleteLater();
		if (internal) 
		{
			manager->deleteLater();
		}
	});

	// ���������
	QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
		if (NULL != errorHandler) 
		{
			errorHandler(reply->errorString());
			m_blFinished = true;
		}
	});
}

void HttpClient::upload(const QString &path,
	std::function<void(const QString &)> successHandler,
	std::function<void(const QString &)> errorHandler,
	const char *encoding) 
{
	if (!d)
		return;
	if (d->debug) 
	{
		qDebug() << QString("URL: %1").arg(d->url);
	}

	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

	QFile *file = new QFile(path);
	file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

	// ����ļ���ʧ�ܣ����ͷ���Դ����
	if (!file->open(QIODevice::ReadOnly)) 
	{
		if (NULL != errorHandler) 
		{
			errorHandler(QString("�ļ���ʧ��: %1").arg(file->errorString()));
			multiPart->deleteLater();
			return;
		}
	}
	m_blFinished = false;

	// �������ļ��ϴ�
	QString disposition = QString("form-data; name=\"file\"; filename=\" % 1\"").arg(file->fileName());
	QHttpPart imagePart;
	imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(disposition));
	imagePart.setBodyDevice(file);
	multiPart->append(imagePart);

	bool internal = d->useInternalNetworkAccessManager;
	QNetworkRequest request(QUrl(d->url));
	QNetworkAccessManager *manager = internal ? new QNetworkAccessManager() : d->networkAccessManager;
	QNetworkReply *reply = manager->post(request, multiPart);
	multiPart->setParent(reply);

	// �������ʱһ���Զ�ȡ������Ӧ����
	QObject::connect(reply, &QNetworkReply::finished, [=] {
		if (reply->error() == QNetworkReply::NoError && NULL != successHandler) 
		{
			successHandler(readResponse(reply, encoding)); // �ɹ�ִ��
			m_blFinished = true;
		}

		// �ͷ���Դ
		reply->deleteLater();
		if (internal) 
		{
			manager->deleteLater();
		}
	});

	// ���������
	QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
		if (NULL != errorHandler) 
		{
			errorHandler(reply->errorString());
			m_blFinished = true;
		}
	});
}

// ִ������ĸ�������
void HttpClient::execute(bool posted,
	std::function<void(const QString &)> successHandler,
	std::function<void(const QString &)> errorHandler,
	const char *encoding) 
{
	if (!d)
		return;
	// ����� GET ���󣬲��Ҳ�����Ϊ�գ����������Ĳ������ŵ� URL ����
	if (!posted && !d->params.isEmpty()) 
	{
		d->url += "?" + d->params.toString(QUrl::FullyEncoded);
	}

	if (d->debug)
	{
		qDebug() << QString("URL: %1?%2").arg(d->url).arg(d->params.toString());
	}

	QUrl urlx(d->url);
	QNetworkRequest request(urlx);

	m_blFinished = false;

	// �������ͷ��ӵ� request ��
	QHashIterator<QString, QString> iter(d->headers);
	while (iter.hasNext())
	{
		iter.next();
		request.setRawHeader(iter.key().toUtf8(), iter.value().toUtf8());
	}

	// ע��: ������ Lambda ���ʽ��ʹ�� HttpClient ����ĳ�Ա���ݣ�����������������δ����ʱ���Ѿ����������ˣ�
	// �������Ҫʹ������������ݣ�����һ���ֲ����������������ݣ�Ȼ���� Lambda ���ʽ���������ֲ�����

	// �����ʹ���ⲿ�� manager �򴴽�һ���µģ��ڷ�����ɺ���Զ�ɾ����
	bool internal = d->useInternalNetworkAccessManager;
	QNetworkAccessManager *manager = internal ? new QNetworkAccessManager() : d->networkAccessManager;
	QNetworkReply *reply = posted ? manager->post(request, d->params.toString(QUrl::FullyEncoded).toUtf8()) : manager->get(request);

	// �������ʱһ���Զ�ȡ������Ӧ����
	QObject::connect(reply, &QNetworkReply::finished, [=] {
		if (reply->error() == QNetworkReply::NoError && NULL != successHandler) 
		{
			successHandler(readResponse(reply, encoding)); // �ɹ�ִ��
			m_blFinished = true;
		}

		// �ͷ���Դ
		reply->deleteLater();
		if (internal) 
		{
			manager->deleteLater();
		}
	});

	// ���������
	QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
		if (NULL != errorHandler) 
		{
			errorHandler(reply->errorString());
			m_blFinished = true;
		}
	});
}

QString HttpClient::readResponse(QNetworkReply *reply, const char *encoding) 
{
	QTextStream in(reply);
	QString result;
	in.setCodec(encoding);

	while (!in.atEnd()) 
	{
		result += in.readLine();
	}

	return result;
}
