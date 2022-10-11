#pragma once
//#include "utils.h"
#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

class ConsumerMsg
{
public:
	struct AmqpParam {
		std::string ip;
		int port;
		std::vector<std::string> scenes;
		std::string username;
		std::string password;
		std::string path;
		std::string queuename;
		std::string consumertag;
		std::vector<std::string> exchangename;
		std::vector<std::string> routingkey;
		AmqpParam() {};
		AmqpParam(std::string ip, int port, std::string username, std::string password, std::string path, 
			std::string queuename, std::string consumertag, std::vector<std::string> exchangename, std::vector<std::string> _routingkey, std::vector<std::string> _scenes):
			ip(ip), port(port), username(username), password(password), path(path), 
			queuename(queuename), consumertag(consumertag), exchangename(exchangename), routingkey(_routingkey), scenes(_scenes){};
	};

public:
	//exchange routingkey tag message
	typedef std::function<void(std::string, std::string, std::string, std::string)> ReceiveMessageCallback;

	ConsumerMsg();
	~ConsumerMsg();

	bool Connect(const AmqpParam &param);
	void Disconnect();
	void start();
	void stop();
	bool GetConnectStatus() { return m_blConnnect; };

	void SetReceiveMessageCallback(ReceiveMessageCallback callback);

	void SetParam(AmqpParam param) { m_param = param; };
private:
	//Zh_Log4j_Logger::Log4j * m_logger;
	AmqpClient::Channel::ptr_t m_channel;
	AmqpParam m_param;

	volatile std::atomic_bool m_blConnnect;

	std::thread thread;
	std::timed_mutex m_mutex;
	volatile std::atomic_bool m_blRunning;
	ReceiveMessageCallback m_callback;

	std::vector<std::string> m_consumerTags;
protected:
	void startConsumer(void* param);
};

