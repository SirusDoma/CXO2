#pragma once

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Sprite.hpp>

namespace Gx
{
    class ScrollBar : public Control, public virtual Colorable
    {
    public:
        enum class ScrollOrientation { Horizontal, Vertical };

        ScrollBar();
        ScrollBar(const sf::Texture& texture, sf::FloatRect bounds, ScrollBar::ScrollOrientation orientation = ScrollOrientation::Horizontal);
        ScrollBar(const sf::Texture& texture, sf::IntRect texCoords, sf::FloatRect bounds, ScrollBar::ScrollOrientation orientation = ScrollOrientation::Horizontal);

        sf::FloatRect GetLocalBounds() const override;
        void SetLocalBounds(const sf::FloatRect& bounds);

        const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture, bool resetRect = false);

        const sf::IntRect& GetTexCoords() const;
        void SetTexCoords(const sf::IntRect& rectangle);

        const sf::Color& GetColor() const override;
        void SetColor(const sf::Color& color) override;

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

        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        void OnMouseMoved(const sf::Event::MouseMoved& ev) override;
        void OnMouseButtonPressed(const sf::Event::MouseButtonPressed& ev) override;
        void OnMouseButtonReleased(const sf::Event::MouseButtonReleased& ev) override;
        void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& ev) override;

        void Invalidate() override;

        Sprite         m_sprite;
        sf::FloatRect  m_bounds;
        sf::Vector2f   m_maxBounds;
        float m_value, m_maxValue, m_step, m_scrollDelta;

        bool m_dragging;
        sf::Vector2f m_anchorPoint;

        ScrollOrientation m_orientation;
        std::function<void(ScrollBar&, float)> m_onValueChanged;
    };
}
