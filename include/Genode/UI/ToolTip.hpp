#ifndef GENODE_UI_TOOLTIP_HPP
#define GENODE_UI_TOOLTIP_HPP

#include <Genode/UI/Label.hpp>
#include <Genode/Graphics/Shapes/Rectangle.hpp>
#include <Genode/Graphics/Text.hpp>

namespace Gx
{
    class ToolTip : public Label
    {
    public:
        enum class Alignment { Left, Center, Right };

        ToolTip();
        ToolTip(const sf::String &string, const sf::Font &font, unsigned int characterSize = 30);

        virtual const sf::FloatRect GetLocalBounds() const;

        void Show(Control *parent);
        void Show(sf::Vector2f position, Alignment alignment = Alignment::Center);
        void Show();
        void Hide();

        const sf::Time &GetDuration() const;
        void SetDuration(const sf::Time &duration);

        sf::Vector2f GetPadding() const;
        void SetPadding(const sf::Vector2f &padding);

        const sf::Color &GetContainerFillColor() const;
        void SetContainerFillColor(const sf::Color &fillColor);

        const sf::Color &GetContainerOutlineColor() const;
        void SetContainerOutlineColor(const sf::Color &outlineColor);

        float GetContainerOutlineThickness() const;
        void SetContainerOutlineThickness(float outlineThickness);

    protected:
        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual void Invalidate();

    private:
        Rectangle    m_rectangle;
        sf::Vector2f m_padding;
        sf::Color    m_fillColor, m_outlineColor;
        float        m_outlineThickness;
        sf::Time     m_duration, m_elapsed;
    };
}

#endif
