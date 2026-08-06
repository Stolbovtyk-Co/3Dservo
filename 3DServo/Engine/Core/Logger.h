#pragma once
#define LOG_INFO(logger, msg) \
    logger.log(Logger::Level::Info, typeid(*this).name(), msg)

#define LOG_WARN(logger, msg) \
    logger.log(Logger::Level::Warning, typeid(*this).name(), msg)

#define LOG_ERROR(logger, msg) \
    logger.log(Logger::Level::Error, typeid(*this).name(), msg)

#define LOG_FATAL(logger, msg) \
    logger.log(Logger::Level::Fatal, typeid(*this).name(), msg)
#include <Windows.h>

#include <atomic>
#include <condition_variable>
#include <chrono>
#include <format>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class Logger
{
public:

    enum class Level
    {
        Info,
        Warning,
        Error,
        Fatal
    };

    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Simple message
    void log(std::string message);

    // Message with level and class/module
    void log(Level level,
        std::string_view className,
        std::string message);

    void logHR(HRESULT hr,
        std::string_view className = "");

private:

    struct Entry
    {
        std::chrono::system_clock::time_point time;
        Level level;
        std::string className;
        std::string message;
    };

    std::queue<Entry> m_queue;

    static std::mutex m_mutex;
    std::condition_variable m_cv;

    std::jthread m_worker;

    bool m_running = true;

    void worker();

    static std::string levelToString(Level level);
    static WORD levelColor(Level level);

    void print(const Entry& entry);
};