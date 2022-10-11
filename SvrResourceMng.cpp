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
* @brief  获取数icon相关信息
* @param  url 对应的rest接口地址
* @param  input ... 对应的其他参数
* @param[out] infos ... 对应的输出数据结果 链表
* @return 成功返回正值 或者0， 失败返回负值 为错误码
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
* @brief  获取相关信息 本地分发 没有的从远端再获取一次
* @param  flag类型标记 用于区分获取数据
* @param  input ... 对应的其他参数
* @param[out] infos ... 对应的输出数据结果 链表
* @return 成功返回正值 或者0， 失败返回负值 为错误码
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
