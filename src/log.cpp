#include "log.hpp"

void Log::Init() {
    spdlog::set_pattern("%n [%^%l%$] %v");
    spdlog::set_level(spdlog::level::debug);
}

void Log::SetLevel(Log::Level level) {
    spdlog::set_level((spdlog::level::level_enum)level);
}

Log::Level Log::GetLevel() {
    return (Log::Level)spdlog::get_level();
}