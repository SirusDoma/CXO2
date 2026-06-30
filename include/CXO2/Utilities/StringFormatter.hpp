#pragma once

#include <SFML/System/String.hpp>
#include <fmt/format.h>
#include <fmt/xchar.h>

template <>
struct fmt::formatter<sf::String, wchar_t>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(sf::String const& string, FormatContext &ctx) const -> decltype(ctx.out())
    {
        auto out = ctx.out();
        for (const auto& codepoint : string)
            *out++ = static_cast<wchar_t>(codepoint);

        return out;
    }
};
