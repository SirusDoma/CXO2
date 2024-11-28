#pragma once

template<typename T>
std::vector<std::uint8_t> Serializable<T>::Serialize() const
{
    auto result = std::vector<std::uint8_t>();
    std::size_t size = 0;

    if (size = T::encodeInto(Record, result); size <= 0)
        return {};

    if (size != result.size())
        result.resize(size);

    return std::move(result);
}

template<typename T>
void Serializable<T>::Deserialize(const void* data, std::size_t sizeInBytes) const
{
    Record.decode(static_cast<const std::uint8_t*>(data), sizeInBytes);
}

template<typename T>
void Serializable<T>::Deserialize(const std::vector<std::uint8_t>& data) const
{
    Deserialize(data.data(), data.size());
}

template<typename T>
Serializable<T>::operator const T&() const
{
    return Record;
}
