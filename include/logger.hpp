#pragma once
#include "beatsaber-hook/shared/utils/typedefs.h"
#include <string_view>

#include "paper2_scotland2/shared/logger.hpp"

template <typename T>
struct fmt::formatter<std::optional<T>> : formatter<string_view>
{
    // parse is inherited from formatter<string_view>.
    template <typename FormatContext>
    auto format(std::optional<T> s, FormatContext& ctx) const
    {
        if(s.has_value())
        {
            return formatter<T>().format(s.value(), ctx);
        }

        return formatter<string_view>::format("std::nullopt", ctx);
    }
};

inline constexpr auto Logger = Paper::ConstLoggerContext("CINEMA");

#define INFO(message, ...) Logger.info(message, ## __VA_ARGS__)
#define ERROR(message, ...) Logger.error(message, ## __VA_ARGS__)
#define WARN(message, ...) Logger.warn(message, ## __VA_ARGS__)
#define CRITICAL(message, ...) Logger.critical(message, ## __VA_ARGS__)
#define DEBUG(message, ...) Logger.debug(message, ## __VA_ARGS__)

using Logger_T = decltype(Logger);