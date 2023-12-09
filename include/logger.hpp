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
struct fmt::formatter<GlobalNamespace::BeatmapDifficulty> : formatter<string_view>
{
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(GlobalNamespace::BeatmapDifficulty difficulty, FormatContext& ctx)
    {
        std::string s;

        using GlobalNamespace::BeatmapDifficulty;
        switch (difficulty) {
            case BeatmapDifficulty::Easy:
                s = "Easy";
                break;
            case BeatmapDifficulty::Normal:
                s = "Normal";
                break;
            case BeatmapDifficulty::Hard:
                s = "Hard";
                break;
            case BeatmapDifficulty::Expert:
                s = "Expert";
                break;
            case BeatmapDifficulty::ExpertPlus:
                s = "Expert+";
                break;
            default:
                s = "Unknown";
                break;
        }

        return formatter<string_view>::format(s, ctx);
    }
};

#define INFO(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::INF>(message, "CINEMA", ## __VA_ARGS__)
#define ERROR(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::ERR>(message, "CINEMA", ## __VA_ARGS__)
#define WARN(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::WRN>(message, "CINEMA", ## __VA_ARGS__)
#define CRITICAL(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::ERR>(message, "CINEMA", ## __VA_ARGS__)
#define DEBUG(message, ...) Paper::Logger::fmtLogTag<Paper::LogLevel::DBG>(message, "CINEMA", ## __VA_ARGS__)