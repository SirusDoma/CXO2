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

    void List::SetOrder(const Order order)
    {
        m_order = order;
    }

    void List::SetVerticalRepeat(const int count, const float spacing)
    {
        m_verticalCount   = count;
        m_verticalSpacing = spacing;
    }

    void List::SetHorizontalRepeat(const int count, const float spacing)
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

    void List::DecreaseSpacingCounter()
    {
        if (m_order == Order::Vertical)
        {
            m_verticalCounter--;
            if (m_verticalCounter < 0)
            {
                m_verticalCounter = 0;
                m_horizontalCounter = std::max(0, m_horizontalCounter - 1);
            }
        }
        else
        {
            m_horizontalCounter--;
            if (m_horizontalCounter < 0)
            {
                m_horizontalCounter = 0;
                m_verticalCounter = std::max(0, m_verticalCounter - 1);
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

    void List::OnChildAdded(Node& node)
    {
        if (m_layouts.empty())
        {
            // Case 1: List use vertical ordering and ran out horizontal slots
            if (m_order == Order::Vertical && m_horizontalCounter >= m_horizontalCount)
                return;

            // Case 2: List use horizontal ordering and ran out vertical slots
            if (m_order == Order::Horizontal && m_verticalCounter >= m_verticalCount)
                return;

            node.SetPosition(GetNextItemPosition());
            IncreaseSpacingCounter();
        }
        else
        {
            // Case 3: List use predetermined layouts and all slots are preoccupied
            if (GetChildrenCount() - 1 >= m_layouts.size())
                return;

            const auto& [origin, position, rotation, scale] = m_layouts[GetChildrenCount() - 1];
            node.SetOrigin(origin);
            node.SetPosition(position);
            node.SetRotation(rotation);
            node.SetScale(scale);
        }

        UiContainer::OnChildAdded(node);
    }

    void List::OnChildRemove(Node& node)
    {
        if (!m_layouts.empty())
        {
            // Re-arrange the layouts so empty slots that were used by old node is backfilled
            const auto children = GetChildren();
            for (std::size_t i = 0; i < children.size(); i++)
            {
                const auto& [origin, position, rotation, scale] = m_layouts[i];
                children[i]->SetOrigin(origin);
                children[i]->SetPosition(position);
                children[i]->SetRotation(rotation);
                children[i]->SetScale(scale);
            }
        }
        else
            DecreaseSpacingCounter();


        UiContainer::OnChildRemove(node);
    }

    void List::Invalidate()
    {
        UiContainer::Invalidate();
    }
}
