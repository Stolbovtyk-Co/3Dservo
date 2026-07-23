#pragma once
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <wrl/client.h>

class Logger
{
public:
	Logger() {
		m_isRunning = true;
		m_workerThread = std::jthread(&Logger::run, this);
	}

	~Logger() {
		m_isRunning = false;
	}

	void log(std::string message);

	void logHR(HRESULT hr);
private :
	std::vector<std::string> m_buffer;
	std::jthread m_workerThread;
	std::mutex m_mutex;
	std::atomic<bool> m_isRunning;

	void run();
};

