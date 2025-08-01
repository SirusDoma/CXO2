#pragma once

#include <Genode/IO/Json.hpp>

#include <magic_enum/magic_enum.hpp>

template<typename T, typename = void>
struct JsonKeyConverter
{
    static bool FromString(const std::string& key, T& value);
};

template<typename Numeric>
struct JsonKeyConverter<Numeric, std::enable_if_t<std::is_integral_v<Numeric>>>
{
    static bool FromString(const std::string& key, Numeric& value)
    {
        value = static_cast<Numeric>(std::stoi(key));
        return true;
    }
};

template<typename Enum>
struct JsonKeyConverter<Enum, std::enable_if_t<std::is_enum_v<Enum>>>
{
    static bool FromString(const std::string& key, Enum& value)
    {
        const auto parsed = magic_enum::enum_cast<Enum>(key, magic_enum::case_insensitive);
        value = parsed.value_or(Enum());

        return parsed.has_value();
    }
};

template <typename K, typename V>
struct nlohmann::adl_serializer<std::map<K, V>>
{
    static void from_json(const json& j, std::map<K, V>& map)
    {
        for (const auto& e : j.items())
        {
            auto key = K();
            if (JsonKeyConverter<K>::FromString(e.key(), key))
                map[key] = e.value().get<V>();
        }
    }
};

template <typename K, typename V>
struct nlohmann::adl_serializer<std::unordered_map<K, V>>
{
    static void from_json(const json& j, std::unordered_map<K, V>& map)
    {
        for (const auto& e : j.items())
        {
            auto key = K();
            if (JsonKeyConverter<K>::FromString(e.key(), key))
                map[key] = e.value().get<V>();
        }
    }
};

template<typename Enum>
struct nlohmann::adl_serializer<Enum, std::enable_if_t<std::is_enum_v<Enum>>>
{
    static void from_json(const json& j, Enum& p)
    {
        if (const auto parsed = magic_enum::enum_cast<Enum>(j.get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
            p = parsed.value();
    }
};
