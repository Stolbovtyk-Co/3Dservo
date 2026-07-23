#include "Logger.h"

#include <chrono>
#include <iostream>

void Logger::run() {
	std::vector<std::string> localBuffer;

	while (m_isRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (!m_buffer.empty()) {
				localBuffer = std::move(m_buffer);
				m_buffer.clear();
			}
		}
		if (!localBuffer.empty()) {
			for (const auto& msg : localBuffer) {
				std::cout << msg << "\n";
			}
			std::cout.flush();
			localBuffer.clear();
		}
	}
}

void Logger::log(std::string message) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_buffer.push_back(std::move(message));
}

void Logger::logHR(HRESULT hr) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_buffer.push_back(std::format("HRESULT: 0x{:08X}", static_cast<unsigned long>(hr)));
}