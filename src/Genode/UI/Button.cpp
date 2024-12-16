#include <Genode/UI/Button.hpp>

namespace Gx
{
    Button::Button(const sf::Texture& texture) :
        m_vertices(),
        m_texture(nullptr)
    {
        SetTexture(texture);
    }

    const sf::Color& Button::GetColor() const
    {
        return m_vertices[0].color;
    }

    void Button::SetColor(const sf::Color& color)
    {
        for (auto& vertex : m_vertices)
            vertex.color = color;
    }

    const sf::Texture* Button::GetTexture() const
    {
        return m_texture;
    }

    const sf::IntRect& Button::GetTextCoords() const
    {
        return m_frames[GetControlState()].TexCoords;
    }

    sf::FloatRect Button::GetLocalBounds() const
    {
        // Use frame for active state first before looking for other frames
        const auto state = m_frames[GetControlState()];
        auto bounds = state.Bounds;
        if (bounds == sf::Vector2f())
        {
            bounds = {
                static_cast<float>(state.TexCoords.size.x),
                static_cast<float>(state.TexCoords.size.y)
            };
        }

        if (bounds == sf::Vector2f())
        {
            // There's no frame for active state, look for valid frame
            for (auto [_, frame] : m_frames)
            {
                bounds = frame.Bounds;
                if (bounds != sf::Vector2f())
                    break;

                bounds = {
                    static_cast<float>(frame.TexCoords.size.x),
                    static_cast<float>(frame.TexCoords.size.y)
                };

                if (bounds != sf::Vector2f())
                    break;
            }
        }

        return { { 0.f, 0.f }, bounds };
    }

    void Button::SetTexture(const sf::Texture& texture)
    {
        m_texture = &texture;
    }

    Button::Frame Button::GetStateFrame(const State state) const
    {
        return m_frames[state];
    }

    void Button::SetFrame(const State state, const Frame& frame)
    {
        m_frames[state] = frame;
        Invalidate();
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

        OnControlClick(*this, sf::Event::MouseButtonReleased());
    }

    Button::Frame Button::GetCurrentFrame() const
    {
        return m_frames[GetControlState()];
    }

    RenderStates Button::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states.transform     *= GetTransform();
        states.coordinateType = sf::CoordinateType::Pixels;

        const auto currentTexCoords = GetCurrentFrame().TexCoords;
        if (m_texture && currentTexCoords.size.x > 0 && currentTexCoords.size.y > 0)
        {
            states.texture = m_texture;
            surface.Render(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TriangleStrip, states);
        }

        return RenderableContainer::Render(surface, states);
    }

    void Button::UpdatePositions()
    {
        const auto bounds = GetCurrentFrame().TexCoords;
        m_vertices[0].position = sf::Vector2f(0, 0);
        m_vertices[1].position = sf::Vector2f(0, bounds.size.y);
        m_vertices[2].position = sf::Vector2f(bounds.size.x, 0);
        m_vertices[3].position = sf::Vector2f(bounds.size.x, bounds.size.y);
    }

    void Button::UpdateTexCoords()
    {
        const auto bounds = GetCurrentFrame().TexCoords;
        const auto left = static_cast<float>(bounds.position.x);
        const float right = left + static_cast<float>(bounds.size.x);
        const auto top = static_cast<float>(bounds.position.y);
        const float bottom = top + static_cast<float>(bounds.size.y);

        m_vertices[0].texCoords = sf::Vector2f(left, top);
        m_vertices[1].texCoords = sf::Vector2f(left, bottom);
        m_vertices[2].texCoords = sf::Vector2f(right, top);
        m_vertices[3].texCoords = sf::Vector2f(right, bottom);
    }

    void Button::Invalidate()
    {
        if (!IsEnabled())
            return;

        const auto frame = GetCurrentFrame();
        SetColor(frame.Color);
        UpdatePositions();
        UpdateTexCoords();
    }
}
