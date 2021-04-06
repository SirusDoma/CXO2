#ifndef GENODE_UI_SCROLLBAR_HPP
#define GENODE_UI_SCROLLBAR_HPP

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Sprite.hpp>

namespace Gx
{
    class ScrollBar : public Control
    {
    public:
        enum ScrollOrientation { Horizontal, Vertical };
        
        ScrollBar();
        ScrollBar(const sf::Texture &texture, sf::FloatRect bounds, ScrollBar::ScrollOrientation orientation = ScrollOrientation::Horizontal);
        ScrollBar(const sf::Texture &texture, sf::IntRect texCoords, sf::FloatRect bounds, ScrollBar::ScrollOrientation orientation = ScrollOrientation::Horizontal);

        virtual const sf::FloatRect GetLocalBounds() const;
        void SetLocalBounds(const sf::FloatRect &bounds);

        const sf::Texture *GetTexture() const;
        void SetTexture(const sf::Texture &texture, bool resetRect = false);

        const sf::IntRect &GetTexCoords() const;
        void SetTexCoords(const sf::IntRect &rectangle);

        virtual const sf::Color &GetColor() const;
        virtual void SetColor(const sf::Color &color);
        
        ScrollOrientation GetScrollOrientation() const;
        void SetScrollOrientation(ScrollOrientation orientation);

        float GetValue() const;
        void SetValue(float value);
        void SetValueChangedCallback(std::function<void(ScrollBar&, float)> callback);

        float GetStep() const;
        void SetStep(float step);

        float GetMaximumValue() const;
        void SetMaximumValue(float max);

        void Increase();
        void Decrease();

    private:
        sf::FloatRect GetScrollBarGlobalBounds() const;

        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual void OnMouseMove(sf::Event::MouseMoveEvent ev);
        virtual void OnMouseButtonDown(sf::Event::MouseButtonEvent ev);
        virtual void OnMouseButtonUp(sf::Event::MouseButtonEvent ev);

        virtual void Invalidate();

        Sprite         m_sprite;
        sf::FloatRect  m_bounds;
        sf::Vector2f   m_maxBounds;
        float m_value, m_maxValue, m_step;

        bool m_dragging;
        sf::Vector2f m_anchorPoint;

        ScrollOrientation m_orientation;
        std::function<void(ScrollBar&, float)> m_onValueChanged;
    };
}

#endif
