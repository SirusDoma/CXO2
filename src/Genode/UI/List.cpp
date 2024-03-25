#include <Genode/UI/List.hpp>

#include <cmath>

namespace Gx
{
    List::List() :
        List(1, 0, 1, 0)
    {
    }

    List::List(int verticalCount, float verticalSpacing) :
        List(verticalCount, verticalSpacing, 1, 0)
    {
    }

    List::List(int verticalCount, float verticalSpacing, int horizontalCount, float horizontalSpacing) :
        m_order(Order::Vertical),
        m_verticalCount(verticalCount),
        m_verticalSpacing(verticalSpacing),
        m_horizontalCount(horizontalCount),
        m_horizontalSpacing(horizontalSpacing),
        m_verticalCounter(),
        m_horizontalCounter()
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

    void List::Apply(const std::function<void(Control *)>& fun)
    {
        if (!fun)
            return;

        for (auto child : GetChildren())
        {
            auto control = dynamic_cast<Control*>(child);
            if (control)
                fun(control);
        }
    }

    bool List::IsAvailable() const
    {
        return m_verticalCounter <= m_verticalCount &&  m_horizontalCounter <= m_horizontalCount;
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
        if (!IsAvailable())
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

    void List::AddChild(Node *node)
    {
        if (!node || (m_order == Order::Vertical && m_horizontalCounter >= m_horizontalCount) || (m_verticalCounter >= m_verticalCount))
            return;

        node->SetPosition(GetNextItemPosition());
        IncreaseSpacingCounter();

        Node::AddChild(node);
    }

    void List::RemoveChild(Node *node)
    {
        Node::RemoveChild(node);
    }

    void List::AddChild(Control *control)
    {
        if (!control || (m_order == Order::Vertical && m_horizontalCounter >= m_horizontalCount) || (m_verticalCounter >= m_verticalCount))
            return;

        control->SetPosition(GetNextItemPosition());
        IncreaseSpacingCounter();

        Control::AddChild(control);
    }

    void List::RemoveChild(Control *control)
    {
        Control::RemoveChild(control);
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
