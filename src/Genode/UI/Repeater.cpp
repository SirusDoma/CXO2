#include <Genode/UI/Repeater.hpp>

namespace Gx
{
    Repeater::Repeater() :
        Repeater(1, 0, 1, 0)
    {
    }

    Repeater::Repeater(int verticalCount, float verticalSpacing) :
        Repeater(verticalCount, verticalSpacing, 1, 0)
    {
    }

    Repeater::Repeater(int verticalCount, float verticalSpacing, int horizontalCount, float horizontalSpacing) :
        m_verticalCount(verticalCount),
        m_verticalSpacing(verticalSpacing),
        m_horizontalCount(horizontalCount),
        m_horizontalSpacing(horizontalSpacing),
        m_verticalCounter(),
        m_horizontalCounter()
    {
    }

    Repeater::~Repeater()
    {
    }


    void Repeater::SetVerticalRepeat(int count, float spacing)
    {
        m_verticalCount   = count;
        m_verticalSpacing = spacing;
    }

    void Repeater::SetHorizontalRepeat(int count, float spacing)
    {
        m_horizontalCount   = count;
        m_horizontalSpacing = spacing;
    }

    int Repeater::GetRepeatCount() const
    {
        return m_verticalCount + m_horizontalCount;
    }

    int Repeater::GetVerticalCount() const
    {
        return m_verticalCount;
    }

    int Repeater::GetHorizontalCount() const
    {
        return m_horizontalCount;
    }

    float Repeater::GetVerticalSpacing() const
    {
        return m_verticalSpacing;
    }

    float Repeater::GetHorizontalSpacing() const
    {
        return m_horizontalSpacing;
    }

    void Repeater::Apply(std::function<void(Control *)> fun)
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

    void Repeater::AddChild(Control *control)
    {
        if (!control || m_horizontalCounter >= m_horizontalCount)
            return;

        control->SetPosition(sf::Vector2f(m_horizontalSpacing * m_horizontalCounter, m_verticalSpacing * m_verticalCounter));
        m_verticalCounter++;
        if (m_verticalCounter >= m_verticalCount)
        {
            m_verticalCounter = 0;
            m_horizontalCounter++;
        }

        Control::AddChild(control);
    }

    void Repeater::RemoveChild(Control *control)
    {
        Control::RemoveChild(control);
    }

    void Repeater::Invalidate()
    {
        UiContainer::Invalidate();
    }

    void Repeater::ClearChildren()
    {
        Node::ClearChildren();
        m_verticalCounter   = 0;
        m_horizontalCounter = 0;
    }
}
