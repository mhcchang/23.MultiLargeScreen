/***********************************************************************
* @brief qt httpclient��װ
* @detail ֧��ͬ���첽get�ı��Ͷ���������
* @author beijing zhuohe tech.
* @date 01/31/2022
***********************************************************************/

#ifndef _CVCAM_QHTTPCLIENT_H
#define _CVCAM_QHTTPCLIENT_H

#include <functional>
#include <QObject>
class QString;
class QByteArray;
struct HttpClientPrivate;
class QNetworkReply;
class QNetworkAccessManager;

class HttpClient : public QObject
{
	Q_OBJECT
public:
	HttpClient(QObject *parent = nullptr);

	//HttpClient(QObject *parent, const QString &url);
	~HttpClient();

	void Init(const QString &url);
	/**
	* @brief ÿ����һ�� QNetworkAccessManager ���󶼻ᴴ��һ���̣߳���Ƶ���ķ�������ʱ��Ϊ�˽�ʡ�߳���Դ��
	* ����ʹ�ô��˵� QNetworkAccessManager�������ᱻ HttpClient ɾ����
	* ���û��ʹ�� useManager() ����һ�� QNetworkAccessManager���� HttpClient ���Զ��Ĵ���һ�������������������ɺ�ɾ������
	* @param manager QNetworkAccessManager ����
	* @return ���� HttpClient �����ã�����������ʽ����
	*/
	HttpClient& useManager(QNetworkAccessManager *manager);

	/**
	* @brief ���� debug Ϊ true ��ʹ�� debug ģʽ������ִ��ʱ�������� URL �Ͳ�����
	* @param debug �Ƿ����õ���ģʽ
	* @return ���� HttpClient �����ã�����������ʽ����
	*/
	HttpClient& debug(bool debug);

	/**
	* @brief ���Ӳ���
	* @param name ����������
	* @param value ������ֵ
	* @return ���� HttpClient �����ã�����������ʽ����
	*/
	HttpClient& addParam(const QString &name, const QString &value);

	/**
	* @brief ���ӷ���ͷ
	* @param header ����ͷ������
	* @param value ����ͷ��ֵ
	* @return ���� HttpClient �����ã�����������ʽ����
	*/
	HttpClient& addHeader(const QString &header, const QString &value);

	/**
	* @brief ��� POST ��ʹ�õ�ͷ��Ϣ���ȼ��� addHeader("content-type", "application/x-www-form-urlencoded")
	* @return ���� HttpClient �����ã�����������ʽ����
	*/
	HttpClient& addFormHeader();

	/**
	* @brief ִ�� GET ����
	* @param successHandler ����ɹ��Ļص� lambda ����
	* @param errorHandler ����ʧ�ܵĻص� lambda ����
	* @param encoding ������Ӧ�ı���
	*/
	void get(std::function<void(const QString &)> successHandler,
		std::function<void(const QString &)> errorHandler = NULL,
		const char *encoding = "UTF-8");

	/**
	* @brief ִ�� POST ����
	* @param successHandler ����ɹ��Ļص� lambda ����
	* @param errorHandler ����ʧ�ܵĻص� lambda ����
	* @param encoding ������Ӧ�ı���
	*/
	void post(std::function<void(const QString &)> successHandler,
		std::function<void(const QString &)> errorHandler = NULL,
		const char *encoding = "UTF-8");

	/**
	* @brief ʹ�� GET �������أ��������ݿɶ�ȡʱ�ص� readyRead(), ����������Ӧ���� readyRead() ������ݱ��浽�ļ�
	* @param readyRead �����ݿɶ�ȡʱ�Ļص� lambda ����
	* @param finishHandler ��������ɺ�Ļص� lambda ����
	* @param errorHandler ����ʧ�ܵĻص� lambda ����
	*/
	void download(std::function<void(const QByteArray &)> readyRead,
		std::function<void()> finishHandler = NULL,
		std::function<void(const QString &)> errorHandler = NULL);

	/**
	* @brief �ϴ��ļ�
	* @param path Ҫ�ϴ����ļ���·��
	* @param successHandler ����ɹ��Ļص� lambda ����
	* @param errorHandler ����ʧ�ܵĻص� lambda ����
	* @param encoding ������Ӧ�ı���
	*/
	void upload(const QString &path, std::function<void(const QString &)> successHandler = NULL,
		std::function<void(const QString &)> errorHandler = NULL, const char *encoding = "UTF-8");

	bool isFinished() { return m_blFinished; };

	/**
	* @brief get������ȡ�ı����� ͬ������ ʱ��Ϊ
	* @param[out] ba ����˷��ص��ı�����
	* @param nTimeout ��ʱ���� ��λ����
	* @param encoding ������Ӧ�ı���
	* @return �ɹ�����true ʧ�ܷ���false
	*/
	bool getTextBody(QString &str, int requestFlag = 0, QByteArray postData = "", int nTimeout = 3000, const char *encoding = "UTF-8");
	/**
	* @brief get������ȡ���������� ͬ������ ʱ��Ϊ
	* @param[out] ba ����˷��صĶ���������
	* @param nTimeout ��ʱ���� ��λ����
	* @param encoding ������Ӧ�ı���
	* @return �ɹ�����true ʧ�ܷ���false
	*/
	bool getBinaryBody(QByteArray &ba, int requestFlag = 0, QByteArray postData = "", int nTimeout = 3000, const char *encoding = "UTF-8");

	inline HttpClient &operator=(const HttpClient &other) {};
	inline HttpClient &operator=(HttpClient &&other) {};
private:
	/**
	* @brief ִ������ĸ�������
	* @param posted Ϊ true ��ʾ POST ����Ϊ false ��ʾ GET ����
	* @param successHandler ����ɹ��Ļص� lambda ����
	* @param errorHandler ����ʧ�ܵĻص� lambda ����
	* @param encoding ������Ӧ�ı���
	*/
	void execute(bool posted,
		std::function<void(const QString &)> successHandler,
		std::function<void(const QString &)> errorHandler,
		const char *encoding);

	/**
	* @brief ��ȡ��������Ӧ������
	* @param reply ����� QNetworkReply ����
	* @param encoding ������Ӧ�ı��룬Ĭ��ʹ�� UTF-8
	* @return ����������Ӧ���ַ���
	*/
	QString readResponse(QNetworkReply *reply, const char *encoding = "UTF-8");

	//
	void SuccessBinaryImp(QNetworkReply* reply);
	void SuccessStringImp(QNetworkReply* reply);

	volatile bool m_blFinished;
	HttpClientPrivate *d;

public:
	QString m_strGet;
	QByteArray m_baGet;
};

#endif // _CVCAM_QHTTPCLIENT_H
