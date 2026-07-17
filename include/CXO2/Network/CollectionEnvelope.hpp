#pragma once

#include <Genode/Network/Packet.hpp>
#include <Genode/Utilities/Extensions.hpp>

#include <type_traits>
#include <functional>

namespace Cx
{
    class MessageFieldEnvelope
    {
    protected:
        virtual ~MessageFieldEnvelope() = default;

        virtual std::size_t NormalizeEnvelopeSize(std::size_t size) const = 0;

        virtual std::size_t ReadEnvelopeSize(Gx::Packet& packet) const = 0;
        virtual void WriteEnvelopeSize(Gx::Packet& packet) const = 0;

        virtual void OnSend(Gx::Packet& packet) const = 0;
        virtual void OnReceive(Gx::Packet& packet) = 0;

        virtual std::size_t GetContainerSize() const = 0;
    };

    template<
        typename TContainer,
        typename TPrefixSize = void,
        typename = std::enable_if_t<std::is_integral_v<TPrefixSize> || std::is_void_v<TPrefixSize>>
    >
    class CollectionEnvelope : public MessageFieldEnvelope
    {
    public:
        CollectionEnvelope();
        CollectionEnvelope(std::size_t minSize, std::size_t maxSize);

        // ReSharper disable CppNonExplicitConvertingConstructor
        CollectionEnvelope(const Gx::type_identity_t<TContainer>& container);
        CollectionEnvelope(Gx::type_identity_t<TContainer>&& container);
        // ReSharper restore CppNonExplicitConvertingConstructor

        CollectionEnvelope(const Gx::type_identity_t<TContainer>& container, std::size_t minSize, std::size_t maxSize);
        CollectionEnvelope(Gx::type_identity_t<TContainer>&& container, std::size_t minSize, std::size_t maxSize);

        friend Gx::Packet& operator<<(Gx::Packet& packet, const CollectionEnvelope<TContainer, TPrefixSize>& envelope)
        {
            envelope.OnSend(packet);
            return packet;
        }

        friend Gx::Packet& operator>>(Gx::Packet& packet, CollectionEnvelope<TContainer, TPrefixSize>& envelope)
        {
            envelope.OnReceive(packet);
            return packet;
        }

        std::optional<std::size_t> GetMinimumSize() const;
        std::optional<std::size_t> GetMaximumSize() const;

        void SetMinimumSize(std::size_t value);
        void SetMaximumSize(std::size_t value);

        TContainer& GetContainer();
        const TContainer& GetContainer() const;

        // ReSharper disable CppNonExplicitConversionOperator
        operator TContainer&();
        operator const TContainer&() const;
        // ReSharper restore CppNonExplicitConversionOperator

        typename TContainer::iterator begin();
        typename TContainer::iterator end();

        typename TContainer::const_iterator begin() const;
        typename TContainer::const_iterator end() const;

        TContainer* operator->();
        const TContainer* operator->() const;

    protected:
        template<typename T, typename = void>
        struct HasSize : std::false_type {};

        template<typename T>
        struct HasSize<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};

        template<typename T, typename = void>
        struct HasPushBack : std::false_type {};

        template<typename T>
        struct HasPushBack<T, std::void_t<decltype(std::declval<T>().push_back(std::declval<typename T::value_type>()))>> : std::true_type {};

        template<typename T, typename = void>
        struct HasInsert : std::false_type {};

        template<typename T>
        struct HasInsert<T, std::void_t<decltype(std::declval<T>().insert(std::declval<typename T::value_type>()))>> : std::true_type {};

        std::size_t NormalizeEnvelopeSize(std::size_t size) const override;

        std::size_t ReadEnvelopeSize(Gx::Packet& packet) const override;
        void WriteEnvelopeSize(Gx::Packet& packet) const override;

        void OnSend(Gx::Packet& packet) const override;
        void OnReceive(Gx::Packet& packet) override;

        std::size_t GetContainerSize() const override;

    private:
        void UsePrefixSizeType();

        TContainer m_container;
        std::optional<std::size_t> m_minSize{std::nullopt};
        std::optional<std::size_t> m_maxSize{std::numeric_limits<std::uint16_t>::max()};
        std::function<void(Gx::Packet&, std::size_t)> m_sizeWriter;
        std::function<std::size_t(Gx::Packet&)> m_sizeReader;
    };

}

#include <CXO2/Network/CollectionEnvelope.inl>
