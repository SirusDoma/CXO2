#pragma once

#include <CXO2/Network/CollectionEnvelope.hpp>
#include <CXO2/Network/StringEncoder.hpp>
#include <SFML/System/String.hpp>

#include <string>
#include <type_traits>

namespace Cx
{
    template<typename T>
    struct IsBasicString : std::false_type {};

    template<typename CharT, typename Traits, typename Allocator>
    struct IsBasicString<std::basic_string<CharT, Traits, Allocator>> : std::true_type {};

    template<
        typename TString = std::string,
        typename TPrefixSize = void,
        typename = std::enable_if_t<IsBasicString<TString>::value>
    >
    class StringEnvelope : public CollectionEnvelope<TString, TPrefixSize>
    {
    public:
        explicit StringEnvelope(bool nullTerminated = true);

        StringEnvelope(std::size_t minSize, std::size_t maxSize, bool nullTerminated = true);

        // ReSharper disable CppNonExplicitConvertingConstructor
        StringEnvelope(const Gx::type_identity_t<TString>& container, bool nullTerminated = true);
        StringEnvelope(Gx::type_identity_t<TString>&& container, bool nullTerminated = true);
        // ReSharper restore CppNonExplicitConvertingConstructor

        StringEnvelope(const Gx::type_identity_t<TString>& container, std::size_t minSize, std::size_t maxSize, bool nullTerminated = true);
        StringEnvelope(Gx::type_identity_t<TString>&& container, std::size_t minSize, std::size_t maxSize, bool nullTerminated = true);

        bool IsNullTerminated() const;
        void SetNullTerminated(bool value);

        // ReSharper disable CppNonExplicitConversionOperator
        operator sf::String() const;
        operator std::string();
        // ReSharper restore CppNonExplicitConversionOperator

    protected:
        void OnSend(Gx::Packet& packet) const override;
        void OnReceive(Gx::Packet& packet) override;

    private:
        bool m_nullTerminated{true};
    };

}

#include <CXO2/Network/StringEnvelope.inl>
