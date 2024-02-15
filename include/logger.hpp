#pragma once
#include "beatsaber-hook/shared/utils/typedefs.h"
#include <string_view>

#include "paper/shared/logger.hpp"

#include "GlobalNamespace/BeatmapDifficulty.hpp"

template <>
struct fmt::formatter<StringW> : formatter<string_view>
{
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(StringW s, FormatContext& ctx)
    {
        return formatter<string_view>::format(s ? static_cast<std::string>(s) : std::string("NULL"), ctx);
    }
};

template <>
struct fmt::formatter<std::optional<std::string>> : formatter<string_view>
{
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(std::optional<std::string> s, FormatContext& ctx)
    {
        return formatter<string_view>::format(s.value_or("NULL"), ctx);
    }
};

Paper::BaseLoggerContext<std::string>& getLogger();

#define INFO(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::INF>(message, "CINEMA", ## __VA_ARGS__)
#define ERROR(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::ERR>(message, "CINEMA", ## __VA_ARGS__)
#define WARN(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::WRN>(message, "CINEMA", ## __VA_ARGS__)
#define CRITICAL(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::ERR>(message, "CINEMA", ## __VA_ARGS__)
#define DEBUG(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::DBG>(message, "CINEMA", ## __VA_ARGS__)