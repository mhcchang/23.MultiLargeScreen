//#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include "ConsumerMsg.h"
#include <iostream>

using namespace AmqpClient;

ConsumerMsg::ConsumerMsg():
	m_blConnnect(false),
	m_callback(nullptr)
{
	m_blConnnect = false;
	m_channel = false;
}

ConsumerMsg::~ConsumerMsg()
{
	Disconnect();
	stop();
}

bool ConsumerMsg::Connect(const AmqpParam &param)
{
	m_param = param;

	try
	{
		if (m_channel == nullptr)
		{
			m_channel = AmqpClient::Channel::Create(param.ip, param.port, param.username, param.password, param.path);
		}
		if (m_channel == nullptr)
			return false;

		m_consumerTags.clear();
		
		//1. 多场景
		for (auto sceneId : m_param.scenes)
		{
			std::string queuename = param.queuename + "." + sceneId;
			std::string consumertag = "";
			queuename = m_channel->DeclareQueue(queuename, false, false, false, true);
			// 2. 多交换器
			for (auto exchangename1 : m_param.exchangename)
			{
				std::string exchangename = exchangename1 + "." + sceneId;

				m_channel->DeclareExchange(exchangename, AmqpClient::Channel::EXCHANGE_TYPE_TOPIC, false, true, false);

				if (m_param.routingkey.size() == 0)
					m_channel->BindQueue(queuename, exchangename, "#");
				else
				{
					for (auto & it : m_param.routingkey)
						m_channel->BindQueue(queuename, exchangename, it);
				}
			}

			//4.将队列与交换器绑定。
			consumertag = m_channel->BasicConsume(queuename, m_param.consumertag, true, true, false, 1);

			m_consumerTags.push_back(consumertag);
		}
	}
	catch (std::runtime_error &e)
	{
		m_channel.reset();
		//ZH_LOG_FMT_LEVEL(SENTRYSVR_NAME, ERROR, "error[%s] ip=%s port=%d\n", e.what(), param.ip.c_str(), param.port);

		std::printf("error[%s] ip=%s port=%d\n", e.what(), param.ip.c_str(), param.port);
		m_blConnnect = false;
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		return false;
	}
	catch (std::exception &e)
	{
		m_channel.reset();
		//ZH_LOG_FMT_LEVEL(SENTRYSVR_NAME, ERROR, "exception[%s] ip=%s port=%d\n",e.what(), param.ip.c_str(), param.port);
		m_blConnnect = false;
		return false;
	}
	catch (...)
	{
		m_channel.reset();
		//ZH_LOG_FMT_LEVEL(SENTRYSVR_NAME, ERROR, "exception ... ip=%s port=%d\n", param.ip.c_str(), param.port);
		m_blConnnect = false;
		return false;
	}
	//ZH_LOG_LEVEL(SENTRYSVR_NAME, INFO, "ConsumerMsg::Connect \n");
	m_blConnnect = true;
	return true;
}

void ConsumerMsg::Disconnect()
{
	if (m_blConnnect == true)
	{
		try
		{
			m_channel->BasicCancel(m_param.consumertag);
		}
		catch (std::runtime_error &e)
		{
			m_channel.reset();
			//ZH_LOG_FMT_LEVEL(SENTRYSVR_NAME, ERROR, "mq error[%s] \n", e.what());
			m_blConnnect = false;
		}
	}

	m_blConnnect = false;
}

void ConsumerMsg::start()
{
	m_blRunning = true;
	thread = std::thread(&ConsumerMsg::startConsumer, this, (void*)nullptr);
}

void ConsumerMsg::stop()
{
	m_blRunning = false;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	int i = 4;
	while (i --)
	{
		if (thread.joinable())
		{
			thread.join();
			return;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

void ConsumerMsg::startConsumer(void* param)
{
	int i = 0;
	while (m_blRunning)
	{
		if (m_channel == nullptr || m_blConnnect == false)
		{
			if (!Connect(m_param))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				continue;
			}
			else
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		AmqpClient::Envelope::ptr_t envelope;
		std::string routingKey;
		std::string exchange;
		std::string buffer;
		std::string consumerTag;
		try
		{
			envelope = m_channel->BasicConsumeMessage(m_consumerTags);
			routingKey = envelope->RoutingKey();
			exchange = envelope->Exchange();
			consumerTag = envelope->ConsumerTag();
			buffer = envelope->Message()->Body();
		}
		catch (std::runtime_error &e)
		{
			m_channel.reset();
			//ZH_LOG_FMT_LEVEL(SENTRYSVR_NAME, ERROR, "error[%s] exchange=%s\n", e.what(), exchange.c_str());
			m_blConnnect = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			continue;
		}
		catch (std::exception &e)
		{
			m_channel.reset();
			//ZH_LOG_FMT_LEVEL(SENTRYSVR_NAME, ERROR, "exception[%s] exchange=%s\n", e.what(), exchange.c_str());
			m_blConnnect = false;
			continue;
		}
		catch (...)
		{
			m_channel.reset();
			//ZH_LOG_FMT_LEVEL(SENTRYSVR_NAME, ERROR, " exception ... exchange=%s\n", exchange.c_str());
			m_blConnnect = false;
			continue;
		}
		//std::cout << "[y] receve " << i++ << std::endl;
		if (m_blRunning && m_callback)
			m_callback(exchange, routingKey, consumerTag, buffer);
	}

	Disconnect();
}

void ConsumerMsg::SetReceiveMessageCallback(ReceiveMessageCallback callback)
{
	m_callback = callback;
}
