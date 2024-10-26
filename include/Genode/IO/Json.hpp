#pragma once

#include <string>
#include <optional>

#include <nlohmann/json.hpp>

template <typename T>
struct nlohmann::adl_serializer<std::optional<T>>
{
    static void to_json(json& j, const std::optional<T>& opt)
    {
        if (opt == std::nullopt)
            j = nullptr;
        else
            nlohmann::to_json(j, opt.value());
    }

    static void from_json(const json& j, std::optional<T>& opt)
    {
        if (j.is_null())
            opt = std::nullopt;
        else
            opt = j.get<T>();
    }
};

namespace Gx
{
    using Json = nlohmann::ordered_json;
}
