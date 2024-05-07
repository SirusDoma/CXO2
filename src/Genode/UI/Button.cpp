#include <Genode/UI/Button.hpp>

namespace Gx
{
    Button::Button(const sf::Texture &texture) :
        Sprite(texture)
    {
    }

    Button::Button(const sf::Texture &texture, const sf::IntRect &rectangle) :
        Sprite(texture, rectangle)
    {
    }

    const sf::Color &Button::GetColor() const
    {
        return Sprite::GetColor();
    }

    void Button::SetColor(const sf::Color &color)
    {
        Sprite::SetColor(color);
    }

    sf::FloatRect Button::GetLocalBounds() const
    {
        // Use frame for active state first before looking for other frames
        auto bounds = m_stateData[GetControlState()].TexCoords;
        if (bounds.width <= 0 || bounds.height <= 0)
        {
            // There's no frame for active state, look for valid frame
            for (auto [_, frame] : m_stateData)
            {
                bounds = frame.TexCoords;
                if (bounds.width > 0 && bounds.height > 0)
                    break;
            }
        }

        return sf::FloatRect(
            {
                0.f,
                0.f
            },
            {
                static_cast<float>(bounds.width),
                static_cast<float>(bounds.height)
            }
        );
    }

    sf::FloatRect Button::GetGlobalBounds() const
    {
        return Control::GetGlobalBounds();
    }

    void Button::SetTexture(const sf::Texture &texture)
    {
        Sprite::SetTexture(texture);
    }

    Button::Frame Button::GetStateFrame(const Control::State state) const
    {
        return m_stateData[state];
    }

    void Button::SetStateFrame(const Control::State state, const Button::Frame &frame)
    {
        m_stateData[state] = frame;
        Invalidate();
    }

    void Button::ApplyFrame(const Button::Frame &frame)
    {
        SetTexCoords(frame.TexCoords);
        SetColor(frame.Color);
    }

    void Button::PerformClick()
    {
        if (const auto callback = GetClickCallback())
        {
            auto uiEvent = Event{false, GetControlState()};
            callback(*this, uiEvent);

            SetControlState(uiEvent.State);
            if (uiEvent.Handled)
                return;
        }

        OnControlClick(this, sf::Event::MouseButtonEvent{});
    }

    RenderStates Button::Render(RenderSurface &surface, RenderStates states) const
    {
        if (!IsVislble())
            return states;

        return Sprite::Render(surface, states);
    }

    void Button::Invalidate()
    {
        if (!IsEnabled())
            return;

        const auto frame = m_stateData[GetControlState()];
        ApplyFrame(frame);
    }
}
