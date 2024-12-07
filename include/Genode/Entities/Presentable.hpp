#pragma once

#include <SFML/Graphics/Rect.hpp>

namespace Gx
{
    struct PresentationContext
    {
    public:
        virtual ~PresentationContext() = default;
        static const PresentationContext Default;

    protected:
        PresentationContext() = default;
    };

    inline const PresentationContext PresentationContext::Default = {};

    struct GraphicalPresentationContext : PresentationContext
    {
        sf::FloatRect Bounds{};
        bool IsCentered{true};
    };

    class Presentable
    {
    public:
        class Parent;

        virtual ~Presentable() = default;
        virtual bool Dismiss() = 0;

    protected:
        virtual void OnPresented(Parent& parent, const PresentationContext& context) = 0;
        virtual void OnDismissed(Parent& parent) = 0;
    };

    class Presentable::Parent
    {
    public:
        Parent() = default;
        virtual ~Parent() = default;

        virtual bool IsPresenting(Presentable& presentable) const = 0;
        virtual void Present(Presentable& presentable, const PresentationContext& context = PresentationContext::Default)
        {
            presentable.OnPresented(*this, context);
        };

        virtual bool Dismiss() = 0;

    protected:
        virtual bool Dismiss(Presentable& presentable)
        {
            presentable.OnDismissed(*this);
            return true;
        };
    };
}