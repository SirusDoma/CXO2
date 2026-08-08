#pragma once

#include <CXO2/Network/Commands.hpp>
#include <Genode/System/Exception.hpp>

#include <variant>

namespace Cx
{
    template<
        class TMessage,
        typename = std::enable_if_t<IsMessage<TMessage>::value>
    >
    class MessageEnvelope : std::variant<TMessage, std::exception_ptr>
    {
    public:
        using std::variant<TMessage, std::exception_ptr>::variant;

        [[nodiscard]] const TMessage& Open() const
        {
            return std::visit(
                Visitor{
                    [] (const TMessage& message) -> const TMessage&
                    {
                        return message;
                    },
                    [] (const std::exception_ptr& eptr) -> const TMessage&
                    {
                        if (!eptr)
                            throw Gx::Exception("Message envelope holds a null exception pointer");

                        std::rethrow_exception(eptr);
                    }
                },
                static_cast<const std::variant<TMessage, std::exception_ptr>&>(*this));
        }

        [[nodiscard]] TMessage& Open()
        {
            return std::visit(
                Visitor{
                    [] (TMessage& message) -> TMessage&
                    {
                        return message;
                    },
                    [] (const std::exception_ptr& eptr) -> TMessage&
                    {
                        if (!eptr)
                            throw Gx::Exception("Message envelope holds a null exception pointer");

                        std::rethrow_exception(eptr);
                    }
                },
                static_cast<std::variant<TMessage, std::exception_ptr>&>(*this));
        }

    private:
        template <class... Ts>
        struct Visitor : Ts... { using Ts::operator()...; };

        template <class... Ts>
        Visitor(Ts...) -> Visitor<Ts...>;
    };

    template<
        class TMessage,
        typename = std::enable_if_t<IsMessage<TMessage>::value>
    >
    using MessageCallback = std::function<void(const MessageEnvelope<TMessage>&)>;
}
