#include "Logger.h"

#include <iostream>


std::mutex Logger::m_mutex;

Logger::Logger()
{
    m_worker = std::jthread(&Logger::worker, this);
}

Logger::~Logger()
{
    {
        std::lock_guard lock(m_mutex);
        m_running = false;
    }

    m_cv.notify_one();
}

void Logger::log(std::string message)
{
    log(Level::Info, "", std::move(message));
}

void Logger::log(Level level,
    std::string_view className,
    std::string message)
{
    {
        std::lock_guard lock(m_mutex);

        m_queue.emplace(
            Entry{
                std::chrono::system_clock::now(),
                level,
                std::string(className),
                std::move(message)
            });
    }

    m_cv.notify_one();
}

void Logger::logHR(HRESULT hr,
    std::string_view
    className)
{
    log(Level::Error,
        className,
        std::format("HRESULT = 0x{:08X}",
            static_cast<uint32_t>(hr)));
}

void Logger::worker()
{
    while (true)
    {
        Entry entry;

        {
            std::unique_lock lock(m_mutex);

            m_cv.wait(lock, [&]
                {
                    return !m_queue.empty() || !m_running;
                });

            if (!m_running && m_queue.empty())
                break;

            entry = std::move(m_queue.front());
            m_queue.pop();
        }

        print(entry);
    }
}

void Logger::print(const Entry& entry)
{
    auto tt = std::chrono::system_clock::to_time_t(entry.time);

    tm localTime{};
    localtime_s(&localTime, &tt);

    auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            entry.time.time_since_epoch()) %
        1000;

    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(console, &info);

    SetConsoleTextAttribute(console, levelColor(entry.level));

    std::string out = std::format("[{:02}:{:02}:{:02}.{:03}]",
            localTime.tm_hour,
            localTime.tm_min,
            localTime.tm_sec,
            milliseconds.count());

    out += "[" + levelToString(entry.level) + "]";
    if (!entry.className.empty())
        out += "[" + entry.className + "]";

    out += " " + entry.message + '\n';

    std::cout << out;

    SetConsoleTextAttribute(console, info.wAttributes);
}

std::string Logger::levelToString(Level level)
{
    switch (level)
    {
    case Level::Info:    return "INFO";
    case Level::Warning: return "WARN";
    case Level::Error:   return "ERROR";
    case Level::Fatal:   return "FATAL";
    }

    return "UNKNOWN";
}

WORD Logger::levelColor(Level level)
{
    switch (level)
    {
    case Level::Info:
        return FOREGROUND_RED |
            FOREGROUND_GREEN |
            FOREGROUND_BLUE;

    case Level::Warning:
        return FOREGROUND_RED |
            FOREGROUND_GREEN |
            FOREGROUND_INTENSITY;

    case Level::Error:
        return FOREGROUND_RED |
            FOREGROUND_INTENSITY;

    case Level::Fatal:
        return BACKGROUND_RED |
            FOREGROUND_RED |
            FOREGROUND_GREEN |
            FOREGROUND_BLUE |
            FOREGROUND_INTENSITY;
    }

    return FOREGROUND_RED |
        FOREGROUND_GREEN |
        FOREGROUND_BLUE;
}