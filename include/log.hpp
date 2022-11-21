#ifndef LOG_HPP
#define LOG_HPP

#include <memory> // for circular dependency issues
                  // pch.hpp can't be included here

#include <spdlog/spdlog.h>

namespace Log {
enum Level {
    TRACE = spdlog::level::trace,
    DEBUG = spdlog::level::debug,
    INFO = spdlog::level::info,
    WARN = spdlog::level::warn,
    ERROR = spdlog::level::err,
    CRITICAL = spdlog::level::critical
};

void Init();
void SetLevel(Log::Level level);
Log::Level GetLevel();
}; // namespace Log

#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)

#endif