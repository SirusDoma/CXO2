#pragma once
#include <Genode/System/Exception.hpp>

namespace Cx
{
    template<typename TString, typename TPrefixSize, typename T0>
    StringEnvelope<TString, TPrefixSize, T0>::StringEnvelope(bool nullTerminated) :
        CollectionEnvelope<TString, TPrefixSize,T0>::CollectionEnvelope()
    {
        SetNullTerminated(nullTerminated);
    }

    template<typename TString, typename TPrefixSize, typename T0>
    StringEnvelope<TString, TPrefixSize, T0>::StringEnvelope(std::size_t minSize, std::size_t maxSize, bool nullTerminated) :
        CollectionEnvelope<TString, TPrefixSize,T0>::CollectionEnvelope(minSize, maxSize)
    {
        SetNullTerminated(nullTerminated);
    }

    template<typename TString, typename TPrefixSize, typename T0>
    StringEnvelope<TString, TPrefixSize, T0>::StringEnvelope(const Gx::type_identity_t<TString>& container, const bool nullTerminated) :
        CollectionEnvelope<TString, TPrefixSize,T0>::CollectionEnvelope(container)
    {
        SetNullTerminated(nullTerminated);
    }

    template<typename TString, typename TPrefixSize, typename T0>
    StringEnvelope<TString, TPrefixSize, T0>::StringEnvelope(Gx::type_identity_t<TString>&& container, const bool nullTerminated) :
        CollectionEnvelope<TString, TPrefixSize,T0>::CollectionEnvelope(container)
    {
        SetNullTerminated(nullTerminated);
    }

    template<typename TString, typename TPrefixSize, typename T0>
    StringEnvelope<TString, TPrefixSize, T0>::StringEnvelope(const Gx::type_identity_t<TString>& container, std::size_t minSize, std::size_t maxSize, const bool nullTerminated) :
        CollectionEnvelope<TString, TPrefixSize,T0>::CollectionEnvelope(container, minSize, maxSize)
    {
        SetNullTerminated(nullTerminated);
    }

    template<typename TString, typename TPrefixSize, typename T0>
    StringEnvelope<TString, TPrefixSize, T0>::StringEnvelope(Gx::type_identity_t<TString>&& container, std::size_t minSize, std::size_t maxSize, const bool nullTerminated) :
        CollectionEnvelope<TString, TPrefixSize,T0>::CollectionEnvelope(container, minSize, maxSize)
    {
        SetNullTerminated(nullTerminated);
    }

    template<typename TString, typename TPrefixSize, typename T0>
    bool StringEnvelope<TString, TPrefixSize, T0>::IsNullTerminated() const
    {
        return m_nullTerminated;
    }

    template<typename TString, typename TPrefixSize, typename T0>
    void StringEnvelope<TString, TPrefixSize, T0>::SetNullTerminated(const bool value)
    {
        m_nullTerminated = value;
    }

    template<typename TString, typename TPrefixSize, typename T0>
    StringEnvelope<TString, TPrefixSize, T0>::operator sf::String() const
    {
        return sf::String(this->GetContainer());
    }

    template<typename TString, typename TPrefixSize, typename T0>
    StringEnvelope<TString, TPrefixSize, T0>::operator std::string()
    {
        if (typeid(this->GetContainer()) == typeid(std::string))
            return this->GetContainer();

        if (typeid(this->GetContainer()) == typeid(sf::String))
            return this->GetContainer();

        return std::string(this->GetContainer());
    }

    template<typename TString, typename TPrefixSize, typename T0>
    void StringEnvelope<TString, TPrefixSize, T0>::OnSend(Gx::Packet& packet) const
    {
        this->WriteEnvelopeSize(packet);

        const std::size_t size = this->NormalizeEnvelopeSize(this->GetContainerSize());
        const auto& container  = this->GetContainer();

        for (std::size_t i = 0; i < size; i++)
        {
            const typename TString::value_type& item = container[i];
            if (i == size - 1 && m_nullTerminated)
            {
                if constexpr (sizeof(typename TString::value_type) == 1)
                    packet << std::uint8_t{0};
                else
                    packet << typename TString::value_type{};

                break;
            }

            if constexpr (sizeof(typename TString::value_type) == 1)
                packet << static_cast<std::uint8_t>(item); // most likely char
            else
                packet << item;
        }
    }

    template<class T>
    struct always_false : std::false_type {};

    template<typename TString, typename TPrefixSize, typename T0>
    void StringEnvelope<TString, TPrefixSize, T0>::OnReceive(Gx::Packet& packet)
    {
        const std::size_t size = this->NormalizeEnvelopeSize(this->ReadEnvelopeSize(packet));

        auto& container = this->GetContainer();
        container.clear();

        for (std::size_t i = 0; i < size; ++i)
        {
            typename TString::value_type item{};
            if constexpr (sizeof(typename TString::value_type) == 1)
                packet >> *reinterpret_cast<std::uint8_t*>(&item); // most likely char
            else
                packet >> item;

            if (m_nullTerminated)
            {
                if (static_cast<std::uint32_t>(item) == 0)
                    break;
            }

            if constexpr (CollectionEnvelope<TString, TPrefixSize, T0>::template HasPushBack<TString>::value)
            {
                container.push_back(std::move(item));
            }
            else if constexpr (CollectionEnvelope<TString, TPrefixSize, T0>::template HasInsert<TString>::value)
            {
                container.insert(std::move(item));
            }
            else
            {
                static_assert(
                    !CollectionEnvelope<TString, TPrefixSize, T0>::template HasPushBack<TString>::value &&
                    !CollectionEnvelope<TString, TPrefixSize, T0>::template HasInsert<TString>::value,
                    "Container type must have either push_back() or insert() method"
                );

                throw Gx::NotSupportedException();
            }
        }
    }
}
