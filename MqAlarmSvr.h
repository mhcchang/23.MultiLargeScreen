#pragma once
#pragma execution_character_set("utf-8")
#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include <map>

#include <QThread>

#include "Config.h"

#include "PubDefs.h"
#define C_MAXTRAFFICINF_COUNT (512)

class MqAlarmSvr : public QThread
{
	Q_OBJECT
public:
	explicit MqAlarmSvr(QObject *parent = 0);
	~MqAlarmSvr();

	bool startThread(const Config* params);
	// 停止客户端
	bool stop();

	void ConsumerReceiveMessageCallbackImp(std::string exchange, std::string key, std::string tags, std::string msg);

	void run() Q_DECL_OVERRIDE;
public slots:

signals:
	void showAlarm(QString sceneId, qint64 timestamp, qreal lng, qreal lat, MqAlarmInfo inf);

private:
	std::thread m_thread;
	std::mutex m_mutex;
	volatile bool m_blRunning;
	Config* m_params;

	//外部设定的 最后一次接收到数据的时间
	volatile uint64_t m_lastTm;
	//开启报警功能
	volatile bool m_blUseAlarm;
	
	//consumer 
	std::mutex m_mutexVtInfos;

	std::vector<DectectMqInf> m_vtInfos;
	//
	void ProcessMsg();
	void CleanUp();

	bool ParseMsg(QString msg, MqAlarmInfo* inf);
};
