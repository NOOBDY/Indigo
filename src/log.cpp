#include <spdlog/sinks/stdout_color_sinks.h>

#include "log.hpp"

std::shared_ptr<spdlog::logger> Log::m_Logger;

void Log::Init() {
    m_Logger = spdlog::stdout_color_st("Indigo");
    m_Logger->set_pattern("%n [%^%l%$] %v");
}