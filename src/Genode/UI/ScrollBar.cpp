#include <Genode/UI/ScrollBar.hpp>

#include <cmath>

namespace Gx
{
    ScrollBar::ScrollBar() :
        m_sprite(),
        m_bounds(),
        m_value(),
        m_maxValue(100.f),
        m_step(1.f),
        m_scrollDelta(),
        m_dragging(false),
        m_anchorPoint(),
        m_orientation()
    {
    }

    ScrollBar::ScrollBar(const sf::Texture& texture, const sf::FloatRect& bounds, const ScrollOrientation orientation) :
        m_sprite(texture),
        m_bounds(),
        m_value(),
        m_maxValue(100.f),
        m_step(1.f),
        m_scrollDelta(),
        m_dragging(false),
        m_anchorPoint(),
        m_orientation(orientation)
    {
        SetLocalBounds(bounds);
    }

    ScrollBar::ScrollBar(const sf::Texture& texture, const sf::IntRect& texCoords, const sf::FloatRect& bounds, const ScrollOrientation orientation) :
        m_sprite(texture, texCoords),
        m_bounds(),
        m_step(1.f),
        m_value(),
        m_maxValue(100.f),
        m_scrollDelta(),
        m_orientation(orientation),
        m_dragging(false),
        m_anchorPoint()
    {
        SetLocalBounds(bounds);
    }

    sf::FloatRect ScrollBar::GetLocalBounds() const
    {
        return m_bounds;
    }

    void ScrollBar::SetLocalBounds(const sf::FloatRect& bounds)
    {
        if (m_bounds != bounds)
        {
            m_bounds = bounds;
            if (m_orientation == ScrollOrientation::Horizontal && m_bounds.size.y == 0)
                m_bounds.size.y = m_sprite.GetLocalBounds().size.y;
            else if (m_orientation == ScrollOrientation::Vertical && m_bounds.size.x == 0)
                m_bounds.size.x = m_sprite.GetLocalBounds().size.x;

            Invalidate();
        }
    }

    const sf::Texture* ScrollBar::GetTexture() const
    {
        return m_sprite.GetTexture();
    }

    void ScrollBar::SetTexture(const sf::Texture& texture, const bool resetRect)
    {
        m_sprite.SetTexture(texture, resetRect);
        Invalidate();
    }

    const sf::IntRect& ScrollBar::GetTexCoords() const
    {
        return m_sprite.GetTexCoords();
    }

    void ScrollBar::SetTexCoords(const sf::IntRect& rectangle)
    {
        if (m_sprite.GetTexCoords() != rectangle)
        {
            m_sprite.SetTexCoords(rectangle);
            Invalidate();
        }
    }

    const sf::Color& ScrollBar::GetColor() const
    {
        return m_sprite.GetColor();
    }

    void ScrollBar::SetColor(const sf::Color& color)
    {
        m_sprite.SetColor(color);
    }

    ScrollBar::ScrollOrientation ScrollBar::GetScrollOrientation() const
    {
        return m_orientation;
    }

    void ScrollBar::SetScrollOrientation(const ScrollOrientation orientation)
    {
        m_orientation = orientation;
    }

    float ScrollBar::GetValue() const
    {
        return m_value;
    }

    void ScrollBar::SetValue(float value)
    {
        value = std::min(value, m_maxValue);
        value = std::max(value, 0.f);

        if (value != m_value)
        {
            m_value = value;
            if (m_onValueChanged)
                m_onValueChanged(*this, m_value);

            Invalidate();
        }
    }

    void ScrollBar::SetValueChangedCallback(std::function<void(ScrollBar &, float)> callback)
    {
        m_onValueChanged = std::move(callback);
    }

    float ScrollBar::GetStep() const
    {
        return m_step;
    }

    void ScrollBar::SetStep(const float step)
    {
        m_step = step;
    }

    float ScrollBar::GetMaximumValue() const
    {
        return m_maxValue;
    }

    void ScrollBar::SetMaximumValue(float max)
    {
        if (max < 0)
            max = 0;

        if (max != m_maxValue)
        {
            m_maxValue = max;
            if (m_value > m_maxValue)
                SetValue(m_maxValue);
            else
                Invalidate();
        }
    }

    sf::FloatRect ScrollBar::GetScrollBarGlobalBounds() const
    {
        auto parent    = GetParent();
        auto transform = sf::Transform::Identity;
        while (parent)
        {
            transform *= parent->GetTransform();
            parent = parent->GetParent();
        }

        transform *= GetTransform();
        transform *= m_sprite.GetTransform();

        return transform.transformRect(m_sprite.GetLocalBounds());
    }

    void ScrollBar::Increase()
    {
        SetValue(m_value + (m_step == 0 ? 1 : m_step));
    }

    void ScrollBar::Decrease()
    {
        SetValue(m_value - (m_step == 0 ? 1 : m_step));
    }

    RenderStates ScrollBar::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states = Control::Render(surface, states);
        // TODO: Cache global bounds (or transform) to avoid recalculating bounds on mouse events
        // m_globalBounds = states.transform.transformRect(GetLocalBounds);
        // m_scrollGlobalBounds = (m_sprite.GetTransform() * states.transform).transformRect(GetLocalBounds);

        surface.Render(m_sprite, states);
        return states;
    }

    void ScrollBar::OnMouseMoved(const sf::Event::MouseMoved& ev)
    {
        Control::OnMouseMoved(ev);

        if (!IsEnabled())
            return;

        if (m_dragging)
        {
            const auto bounds   = GetGlobalBounds();
            const auto mpos     = sf::Vector2f(ev.position.x, ev.position.y);
            auto position = m_sprite.GetPosition();
            float value   = 0.f;
            if (m_orientation == ScrollOrientation::Horizontal)
            {
                position.x = mpos.x - bounds.position.x - m_anchorPoint.x;
                position.x = std::min(position.x, m_maxBounds.x);
                position.x = std::max(position.x, 0.f);

                value = (position.x / m_maxBounds.x) * m_maxValue;
            }
            else
            {
                position.y = mpos.y - bounds.position.y - m_anchorPoint.y;
                position.y = std::min(position.y, m_maxBounds.y);
                position.y = std::max(position.y, 0.f);

                value = (position.y / m_maxBounds.y) * m_maxValue;
            }

            value = std::min(value, m_maxValue);
            value = std::max(value, 0.f);

            if (m_step != 0)
            {
                if (std::abs(m_value - value) < m_step)
                    return;

                const float step = value > m_value ? m_step : -m_step;
                value = m_value + step;
                value = std::min(value, m_maxValue);
                value = std::max(value, 0.f);
            }

            if (m_value != value)
            {
                m_value = value;
                if (m_onValueChanged)
                    m_onValueChanged(*this, m_value);

                m_sprite.SetPosition(position);
            }
        }
    }

    void ScrollBar::OnMouseButtonPressed(const sf::Event::MouseButtonPressed& ev)
    {
        Control::OnMouseButtonPressed(ev);

        if (!IsEnabled())
            return;

        if (!m_dragging && m_maxValue > 0.f && GetScrollBarGlobalBounds().contains(sf::Vector2f(ev.position.x, ev.position.y)))
        {
            const auto bounds   = GetScrollBarGlobalBounds();
            m_dragging    = true;
            m_anchorPoint = sf::Vector2f(ev.position.x - bounds.position.x, ev.position.y - bounds.position.y);
        }
    }

    void ScrollBar::OnMouseButtonReleased(const sf::Event::MouseButtonReleased& ev)
    {
        Control::OnMouseButtonReleased(ev);
        m_dragging = false;
    }

    void ScrollBar::OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& ev)
    {
        Control::OnMouseWheelScrolled(ev);

        if (!IsEnabled())
            return;

        const auto position = sf::Vector2f(ev.position.x, ev.position.y);
        float delta = ev.delta;
        if (m_orientation == ScrollOrientation::Vertical)
            delta *= -1;

        if (GetGlobalBounds().contains(position))
        {
            if (m_step != 0)
            {
                m_scrollDelta += delta;
                if (std::abs(m_scrollDelta) >= m_step)
                {
                    SetValue(m_value + (m_scrollDelta > 0 ? m_step : -m_step));
                    m_scrollDelta = 0;
                }
            }
            else
                SetValue(m_value + ev.delta);

        }
    }

    void ScrollBar::Invalidate()
    {
        auto position = m_sprite.GetPosition();
        if (m_maxValue <= 0.f)
            return;

        const auto barBounds = m_sprite.GetLocalBounds();
        m_maxBounds    = sf::Vector2f(
          std::abs(m_bounds.size.x  - barBounds.size.x),
          std::abs(m_bounds.size.y - barBounds.size.y)
        );

        if (m_orientation == ScrollOrientation::Horizontal)
        {
            position.x = (m_value / m_maxValue) * m_maxBounds.x;
            position.x = std::min(position.x, m_maxBounds.x);
            position.x = std::max(position.x, 0.f);
        }
        else
        {
            position.y = (m_value / m_maxValue) * m_maxBounds.y;
            position.y = std::min(position.y, m_maxBounds.y);
            position.y = std::max(position.y, 0.f);
        }

        m_sprite.SetPosition(position);
    }
}