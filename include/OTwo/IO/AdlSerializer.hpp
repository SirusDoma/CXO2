#ifndef O2JAM_ADL_SERIALIZER_HPP
#define O2JAM_ADL_SERIALIZER_HPP

#include <OTwo/Serializable.g.hpp>
#include <Genode/IO/Json.hpp>

#include <magic_enum.hpp>

template<>
struct nlohmann::adl_serializer<Gender>
{
    static void from_json(const nlohmann::json& j, Gender& p)
    {
        if (const auto parsed = magic_enum::enum_cast<Gender>(j.get<std::string>()); parsed.has_value())
            p = parsed.value();
    }
};

template<>
struct nlohmann::adl_serializer<Planet>
{
    static void from_json(const nlohmann::json& j, Planet& p)
    {
        if (const auto parsed = magic_enum::enum_cast<Planet>(j.get<std::string>()); parsed.has_value())
            p = parsed.value();
    }
};

template<>
struct nlohmann::adl_serializer<Currency>
{
    static void from_json(const nlohmann::json& j, Currency& p)
    {
        if (const auto parsed = magic_enum::enum_cast<Currency>(j.get<std::string>()); parsed.has_value())
            p = parsed.value();
    }
};


#endif
