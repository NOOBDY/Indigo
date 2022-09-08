#ifndef LOG_HPP
#define LOG_HPP

#include <memory> // for circular dependency issues
                  // pch.hpp can't be included here

#include <spdlog/spdlog.h>

class Log {
public:
    enum Level {
        TRACE = spdlog::level::trace,
        DEBUG = spdlog::level::debug,
        INFO = spdlog::level::info,
        WARN = spdlog::level::warn,
        ERROR = spdlog::level::err,
        CRITICAL = spdlog::level::critical
    };

    static void Init();

    static std::shared_ptr<spdlog::logger> GetLogger() { return m_Logger; }

    static void SetLevel(Log::Level level) {
        m_Logger->set_level((spdlog::level::level_enum)level);
    }
    static Log::Level GetLevel() { return (Log::Level)m_Logger.get()->level(); }

private:
    static std::shared_ptr<spdlog::logger> m_Logger;
};

#define LOG_TRACE(...) Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...) Log::GetLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...) Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Log::GetLogger()->critical(__VA_ARGS__)

#endif