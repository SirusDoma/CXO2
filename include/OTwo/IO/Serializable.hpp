#pragma once

#include <vector>

template<typename T>
class Serializable
{
public:
    Serializable() = default;
    virtual ~Serializable() = default;

    virtual std::vector<std::uint8_t> Serialize() const;
    virtual void Deserialize(const void* data, std::size_t sizeInBytes) const;
    void Deserialize(const std::vector<std::uint8_t>& data) const;

    // ReSharper disable once CppNonExplicitConversionOperator
    operator const T&() const;

protected:
    T Record;
};

#include <OTwo/IO/Serializable.inl>
