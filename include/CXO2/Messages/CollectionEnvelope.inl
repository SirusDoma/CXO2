#pragma once

#include <Genode/System/Exception.hpp>

namespace Cx
{
    template<typename TContainer, typename TPrefixSize, typename T0>
    CollectionEnvelope<TContainer, TPrefixSize, T0>::CollectionEnvelope() :
        m_container{}
    {
        UsePrefixSizeType();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    CollectionEnvelope<TContainer, TPrefixSize, T0>::CollectionEnvelope(const std::size_t minSize, const std::size_t maxSize)
    {
        UsePrefixSizeType();
        SetMinimumSize(minSize);
        SetMinimumSize(maxSize);
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    CollectionEnvelope<TContainer, TPrefixSize, T0>::CollectionEnvelope(const Gx::type_identity_t<TContainer>& container) :
        m_container{container}
    {
        UsePrefixSizeType();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    CollectionEnvelope<TContainer, TPrefixSize, T0>::CollectionEnvelope(Gx::type_identity_t<TContainer>&& container) :
        m_container{std::move(container)}
    {
        UsePrefixSizeType();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    CollectionEnvelope<TContainer, TPrefixSize, T0>::CollectionEnvelope(const Gx::type_identity_t<TContainer>& container, const std::size_t minSize, const std::size_t maxSize) :
        m_container{container}

    {
        UsePrefixSizeType();
        SetMinimumSize(minSize);
        SetMinimumSize(maxSize);
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    CollectionEnvelope<TContainer, TPrefixSize, T0>::CollectionEnvelope(Gx::type_identity_t<TContainer>&& container, const std::size_t minSize, const std::size_t maxSize) :
        m_container{container}
    {
        UsePrefixSizeType();
        SetMinimumSize(minSize);
        SetMinimumSize(maxSize);
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    void CollectionEnvelope<TContainer, TPrefixSize, T0>::UsePrefixSizeType()
    {
        m_sizeWriter = [](Packet& packet, std::size_t size)
        {
            if constexpr (!std::is_void_v<TPrefixSize>)
                packet << static_cast<TPrefixSize>(size);
        };

        m_sizeReader = [this](Packet& packet) -> std::size_t
        {
            if constexpr (!std::is_void_v<TPrefixSize>)
            {
                TPrefixSize size{};
                packet >> size;
                return static_cast<std::size_t>(size);
            }
            else
            {
                return m_minSize.value_or(0);
            }
        };
    }
    template<typename TContainer, typename TPrefixSize, typename T0>
    std::optional<std::size_t> CollectionEnvelope<TContainer, TPrefixSize, T0>::GetMinimumSize() const
    {
        return m_minSize;
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    std::optional<std::size_t> CollectionEnvelope<TContainer, TPrefixSize, T0>::GetMaximumSize() const
    {
        return m_maxSize;
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    void CollectionEnvelope<TContainer, TPrefixSize, T0>::SetMinimumSize(std::size_t value)
    {
        if (m_maxSize.has_value() && value > m_maxSize.value())
            throw Gx::ArgumentOutOfRangeException("The specified value must be equal or less than the maximum size");

        if (value == 0)
            m_minSize = std::nullopt;
        else
            m_minSize = value;

    }
    template<typename TContainer, typename TPrefixSize, typename T0>
    void CollectionEnvelope<TContainer, TPrefixSize, T0>::SetMaximumSize(std::size_t value)
    {
        if (m_minSize.has_value() && value < m_minSize.value())
            throw Gx::ArgumentOutOfRangeException("The specified value must be equal or less than the minimum size");

        if (value == 0)
            m_maxSize = std::nullopt;
        else
            m_maxSize = value;
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    TContainer& CollectionEnvelope<TContainer, TPrefixSize, T0>::GetContainer()
    {
        return m_container;
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    const TContainer& CollectionEnvelope<TContainer, TPrefixSize, T0>::GetContainer() const
    {
        return m_container;
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    CollectionEnvelope<TContainer, TPrefixSize, T0>::operator TContainer&()
    {
        return GetContainer();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    CollectionEnvelope<TContainer, TPrefixSize, T0>::operator const TContainer&() const
    {
        return GetContainer();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    typename TContainer::iterator CollectionEnvelope<TContainer, TPrefixSize, T0>::begin()
    {
        return GetContainer().begin();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    typename TContainer::iterator CollectionEnvelope<TContainer, TPrefixSize, T0>::end()
    {
        return GetContainer().end();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    typename TContainer::const_iterator CollectionEnvelope<TContainer, TPrefixSize, T0>::begin() const
    {
        return GetContainer().begin();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    typename TContainer::const_iterator CollectionEnvelope<TContainer, TPrefixSize, T0>::end() const
    {
        return GetContainer().end();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    TContainer* CollectionEnvelope<TContainer, TPrefixSize, T0>::operator ->()
    {
        return &GetContainer();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    const TContainer* CollectionEnvelope<TContainer, TPrefixSize, T0>::operator ->() const
    {
        return &GetContainer();
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    std::size_t CollectionEnvelope<TContainer, TPrefixSize, T0>::NormalizeEnvelopeSize(std::size_t size) const
    {
        if (m_minSize.has_value())
            size = std::max(size, m_minSize.value());

        if (m_maxSize.has_value())
            size = std::min(size, m_maxSize.value());

        return size;
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    std::size_t CollectionEnvelope<TContainer, TPrefixSize, T0>::ReadEnvelopeSize(Packet& packet) const
    {
        return m_sizeReader(packet);
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    void CollectionEnvelope<TContainer, TPrefixSize, T0>::WriteEnvelopeSize(Packet& packet) const
    {
        const std::size_t normalizedSize = NormalizeEnvelopeSize(GetContainerSize());
        m_sizeWriter(packet, normalizedSize);
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    void CollectionEnvelope<TContainer, TPrefixSize, T0>::OnSend(Packet& packet) const
    {
        WriteEnvelopeSize(packet);

        for (const typename TContainer::value_type& item : m_container)
        {
            if constexpr (sizeof(typename TContainer::value_type) == 1)
                packet << static_cast<std::uint8_t>(item); // most likely char
            else
                packet << item;
        }
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    void CollectionEnvelope<TContainer, TPrefixSize, T0>::OnReceive(Packet& packet)
    {
        const std::size_t size = NormalizeEnvelopeSize(ReadEnvelopeSize(packet));

        m_container.clear();
    
        for (std::size_t i = 0; i < size; ++i)
        {
            typename TContainer::value_type item{};
            if constexpr (sizeof(typename TContainer::value_type) == 1)
                packet >> *reinterpret_cast<std::uint8_t*>(&item); // most likely char
            else
                packet >> item;

            if constexpr (HasPushBack<TContainer>::value)
            {
                m_container.push_back(std::move(item));
            }
            else if constexpr (HasInsert<TContainer>::value)
            {
                m_container.insert(std::move(item));
            }
            else
            {
                static_assert(HasPushBack<TContainer>::value || HasInsert<TContainer>::value, 
                              "Container type must have either push_back() or insert() method");

                throw Gx::NotSupportedException();
            }
        }
    }

    template<typename TContainer, typename TPrefixSize, typename T0>
    std::size_t CollectionEnvelope<TContainer, TPrefixSize, T0>::GetContainerSize() const
    {
        if constexpr (HasSize<TContainer>::value)
        {
            return m_container.size();
        }
        else
        {
            static_assert(HasSize<TContainer>::value, "Container type must have size() method");

            throw Gx::NotSupportedException();
        }
    }
}
