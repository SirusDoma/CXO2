#include <Genode/UI/List.hpp>

#include <cmath>

namespace Gx
{
    List::List() :
        List(1, 0, 1, 0)
    {
    }

    List::List(const int verticalCount, const float verticalSpacing) :
        List(verticalCount, verticalSpacing, 1, 0)
    {
    }

    List::List(const int verticalCount, const float verticalSpacing, const int horizontalCount, const float horizontalSpacing) :
        m_order(Order::Vertical),
        m_verticalCount(verticalCount),
        m_horizontalCount(horizontalCount),
        m_verticalSpacing(verticalSpacing),
        m_horizontalSpacing(horizontalSpacing),
        m_verticalCounter(),
        m_horizontalCounter(),
        m_layouts()
    {
    }

    List::Order List::GetOrder() const
    {
        return m_order;
    }

    void List::SetOrder(List::Order order)
    {
        m_order = order;
    }

    void List::SetVerticalRepeat(int count, float spacing)
    {
        m_verticalCount   = count;
        m_verticalSpacing = spacing;
    }

    void List::SetHorizontalRepeat(int count, float spacing)
    {
        m_horizontalCount   = count;
        m_horizontalSpacing = spacing;
    }

    int List::GetRepeatCount() const
    {
        return m_verticalCount + m_horizontalCount;
    }

    int List::GetVerticalCount() const
    {
        return m_verticalCount;
    }

    int List::GetHorizontalCount() const
    {
        return m_horizontalCount;
    }

    float List::GetVerticalSpacing() const
    {
        return m_verticalSpacing;
    }

    float List::GetHorizontalSpacing() const
    {
        return m_horizontalSpacing;
    }

    void List::AddLayout(const LayoutItem& layout)
    {
        m_layouts.push_back(layout);
    }

    void List::ClearLayouts()
    {
        m_layouts.clear();
    }

    void List::Apply(const std::function<void(Control*)>& fun) const
    {
        if (!fun)
            return;

        for (const auto child : GetChildren())
        {
            if (const auto control = dynamic_cast<Control*>(child))
                fun(control);
        }
    }

    bool List::IsSpaceAvailable() const
    {
        return m_verticalCounter <= m_verticalCount && m_horizontalCounter <= m_horizontalCount;
    }

    sf::Vector2f List::GetNextItemPosition() const
    {
        return {
            std::ceil(m_horizontalSpacing * static_cast<float>(m_horizontalCounter)),
            std::ceil(m_verticalSpacing * static_cast<float>(m_verticalCounter))
        };
    }

    void List::IncreaseSpacingCounter()
    {
        if (!IsSpaceAvailable())
            return;

        if (m_order == Order::Vertical)
        {
            m_verticalCounter++;
            if (m_verticalCounter >= m_verticalCount)
            {
                m_verticalCounter = 0;
                m_horizontalCounter++;
            }
        }
        else
        {
            m_horizontalCounter++;
            if (m_horizontalCounter >= m_horizontalCount)
            {
                m_horizontalCounter = 0;
                m_verticalCounter++;
            }
        }
    }

    void List::Update(const double delta)
    {
        UiContainer::Update(delta);
    }

    RenderStates List::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        return UiContainer::Render(surface, states);
    }

    void List::AddChild(Node* node)
    {
        if (!node || (!m_layouts.empty() && GetChildren().size() >= m_layouts.size()) || (m_layouts.empty() && m_order == Order::Vertical && m_horizontalCounter >= m_horizontalCount) || (m_layouts.empty() && m_order == Order::Horizontal && m_verticalCounter >= m_verticalCount))
            return;

        if (m_layouts.empty())
        {
            node->SetPosition(GetNextItemPosition());
            IncreaseSpacingCounter();
        }
        else
        {
            const auto& [origin, position, rotation, scale] = m_layouts[GetChildren().size()];
            node->SetOrigin(origin);
            node->SetPosition(position);
            node->SetRotation(rotation);
            node->SetScale(scale);
        }

        Control::AddChild(node);
    }

    void List::Invalidate()
    {
        UiContainer::Invalidate();
    }

    void List::ClearChildren()
    {
        Control::ClearChildren();
        m_verticalCounter   = 0;
        m_horizontalCounter = 0;
    }
}
