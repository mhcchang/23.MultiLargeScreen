#include "SvrResourceMng.h"

SvrResourceMng::SvrResourceMng(QObject *parent)
	: QThread(parent)
{
	m_httpClientSvr = nullptr;
}

SvrResourceMng::~SvrResourceMng()
{
}

void SvrResourceMng::SetParam(HttpClientSvr* httpClientSvr)
{
	m_httpClientSvr = httpClientSvr;
}

bool SvrResourceMng::GetLocalIcon(int iconId, QImage &img)
{
	if (m_mapLblIcon.empty())
		return false;

	QMap<int, QImage>::Iterator it = m_mapLblIcon.find(iconId);

	if (it != m_mapLblIcon.end())
	{
		img = it.value();
	}

	return true;
}

bool SvrResourceMng::GetRemoteDbInfo(int flag, QString url)
{
	//todo
	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	//mapParam.insert("", "");
	QByteArray ba;
	QString str = m_httpClientSvr->GetTxtBody(url, ba, mapParam, mapHeader);
	ParseMsg(str);
	return false;
}

/****************************************!
* @brief  ��ȡ��icon�����Ϣ
* @param  url ��Ӧ��rest�ӿڵ�ַ
* @param  input ... ��Ӧ����������
* @param[out] infos ... ��Ӧ��������ݽ�� ����
* @return �ɹ�������ֵ ����0�� ʧ�ܷ��ظ�ֵ Ϊ������
****************************************/
bool SvrResourceMng::GetRemoteIconInfo(QString url)
{
	//todo
	//1;. http get database info...
	QMap<QString, QString> mapParam;
	QMap<QString, QString> mapHeader;
	QByteArray ba;
	QString str = m_httpClientSvr->GetTxtBody(url, ba, mapParam, mapHeader);
	//2. parse json to struct...
	ParseMsg(str);
	//3. output info.....

	return false;
}

/****************************************!
* @brief  ��ȡ�����Ϣ ���طַ� û�еĴ�Զ���ٻ�ȡһ��
* @param  flag���ͱ�� �������ֻ�ȡ����
* @param  input ... ��Ӧ����������
* @param[out] infos ... ��Ӧ��������ݽ�� ����
* @return �ɹ�������ֵ ����0�� ʧ�ܷ��ظ�ֵ Ϊ������
****************************************/
//todo
bool SvrResourceMng::GetLocalInfo(int flag /*, struct ... info, QMap<> infos*/)
{
	//todo
	return false;
}

void SvrResourceMng::ParseMsg(QString str)
{
	//todo
}
