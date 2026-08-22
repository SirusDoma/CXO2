#pragma once

#include <CXO2/UI/Label.hpp>
#include <Genode/Graphics/Shapes/Rectangle.hpp>

namespace Cx
{
    class ToolTip : public Label
    {
    public:
        enum class Alignment { Left, Center, Right };

        ToolTip();
        ToolTip(const Gx::Font& font, const sf::String& string, unsigned int characterSize = 30);
        ToolTip(Gx::Font&& font, const sf::String& string, unsigned int characterSize = 30) = delete;

        [[nodiscard]] sf::FloatRect GetLocalBounds() const override;

        void Show(const Control& parent);
        void Show(sf::Vector2f position, Alignment alignment = Alignment::Center);
        void Show();
        void Hide();

        [[nodiscard]] const sf::Time& GetDuration() const;
        void SetDuration(const sf::Time& duration);

        [[nodiscard]] const sf::Time& GetDelay() const;
        void SetDelay(const sf::Time& delay);

        [[nodiscard]] sf::Vector2f GetPadding() const;
        void SetPadding(const sf::Vector2f& padding);

        [[nodiscard]] const sf::Color& GetContainerColor() const;
        void SetContainerColor(const sf::Color& fillColor);

        [[nodiscard]] const sf::Color& GetContainerOutlineColor() const;
        void SetContainerOutlineColor(const sf::Color& outlineColor);

        [[nodiscard]] float GetContainerOutlineThickness() const;
        void SetContainerOutlineThickness(float outlineThickness);

    protected:
        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void Invalidate() override;

    private:
        Gx::Rectangle    m_rectangle;
        sf::Vector2f m_padding;
        sf::Color    m_fillColor, m_outlineColor;
        float        m_outlineThickness;
        sf::Time     m_duration, m_elapsed;
        sf::Time     m_delay, m_delayElapsed;
        bool         m_pending;
    };
}
