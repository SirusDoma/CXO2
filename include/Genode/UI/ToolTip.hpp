#pragma once

#include <Genode/UI/Label.hpp>
#include <Genode/Graphics/Shapes/Rectangle.hpp>

namespace Gx
{
    class ToolTip : public Label
    {
    public:
        enum class Alignment { Left, Center, Right };

        ToolTip();
        ToolTip(const Font& font, const sf::String& string, unsigned int characterSize = 30);
        ToolTip(Font&& font, const sf::String& string, unsigned int characterSize = 30) = delete;

        sf::FloatRect GetLocalBounds() const override;

        void Show(Control* parent);
        void Show(sf::Vector2f position, Alignment alignment = Alignment::Center);
        void Show();
        void Hide();

        const sf::Time& GetDuration() const;
        void SetDuration(const sf::Time& duration);

        sf::Vector2f GetPadding() const;
        void SetPadding(const sf::Vector2f& padding);

        const sf::Color& GetContainerColor() const;
        void SetContainerColor(const sf::Color& fillColor);

        const sf::Color& GetContainerOutlineColor() const;
        void SetContainerOutlineColor(const sf::Color& outlineColor);

        float GetContainerOutlineThickness() const;
        void SetContainerOutlineThickness(float outlineThickness);

    protected:
        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        void Invalidate() override;

    private:
        Rectangle    m_rectangle;
        sf::Vector2f m_padding;
        sf::Color    m_fillColor, m_outlineColor;
        float        m_outlineThickness;
        sf::Time     m_duration, m_elapsed;
    };
}
