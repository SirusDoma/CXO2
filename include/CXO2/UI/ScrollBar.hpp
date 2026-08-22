#pragma once

#include <CXO2/UI/Control.hpp>
#include <Genode/Graphics/Sprite.hpp>

namespace Cx
{
    class ScrollBar : public Control, public virtual Gx::Colorable
    {
    public:
        enum class ScrollOrientation { Horizontal, Vertical };
        struct ValueChangedEvent : Event
        {
            float Value;
        };

        ScrollBar();
        ScrollBar(const sf::Texture& texture, const sf::FloatRect& bounds, ScrollBar::ScrollOrientation orientation = ScrollOrientation::Horizontal);
        ScrollBar(const sf::Texture& texture, const sf::IntRect& texCoords, const sf::FloatRect& bounds, ScrollBar::ScrollOrientation orientation = ScrollOrientation::Horizontal);

        [[nodiscard]] sf::FloatRect GetLocalBounds() const override;
        void SetLocalBounds(const sf::FloatRect& bounds);

        [[nodiscard]] const sf::Texture* GetTexture() const;
        void SetTexture(const sf::Texture& texture, bool resetRect = false);

        [[nodiscard]] const sf::IntRect& GetTexCoords() const;
        void SetTexCoords(const sf::IntRect& rectangle);

        [[nodiscard]] const sf::Color& GetColor() const override;
        void SetColor(const sf::Color& color) override;

        [[nodiscard]] ScrollOrientation GetScrollOrientation() const;
        void SetScrollOrientation(ScrollOrientation orientation);

        [[nodiscard]] float GetValue() const;
        void SetValue(float value);
        void SetValueChangedCallback(std::function<void(ScrollBar&, ValueChangedEvent&)> callback);

        [[nodiscard]] float GetStep() const;
        void SetStep(float step);

        [[nodiscard]] float GetMaximumValue() const;
        void SetMaximumValue(float max);

        void Increase();
        void Decrease();

    private:
        [[nodiscard]] sf::FloatRect GetScrollBarGlobalBounds() const;

        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void OnMouseMoved(const sf::Event::MouseMoved& ev) override;
        void OnMouseButtonPressed(const sf::Event::MouseButtonPressed& ev) override;
        void OnMouseButtonReleased(const sf::Event::MouseButtonReleased& ev) override;
        void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& ev) override;

        void Invalidate() override;

        Gx::Sprite         m_sprite;
        sf::FloatRect  m_bounds;
        sf::Vector2f   m_maxBounds;
        float m_value, m_maxValue, m_step, m_scrollDelta;

        bool m_dragging;
        sf::Vector2f m_anchorPoint;

        ScrollOrientation m_orientation;
        std::function<void(ScrollBar&, ValueChangedEvent&)> m_onValueChanged;
    };
}
