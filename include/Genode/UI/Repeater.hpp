#ifndef GENODE_UI_REPEATER_HPP
#define GENODE_UI_REPEATER_HPP

#include <Genode/UI/UiContainer.hpp>
#include <functional>

namespace Gx
{
    class Repeater : public virtual UiContainer
    {
    public:
        Repeater();
        Repeater(int verticalCount, float verticalSpacing);
        Repeater(int verticalCount, float verticalSpacing, int horizontalCount, float horizontalSpacing);

        virtual ~Repeater();

        void SetVerticalRepeat(int count, float spacing);
        void SetHorizontalRepeat(int count, float spacing);

        int GetRepeatCount() const;
        int GetVerticalCount() const;
        int GetHorizontalCount() const;

        float GetVerticalSpacing() const;
        float GetHorizontalSpacing() const;

        virtual void AddChild(Control *control);
        virtual void RemoveChild(Control *control);
        virtual void ClearChildren();

        void Apply(std::function<void(Control*)> fun);

    protected:
        virtual void Invalidate();

    private:
        int m_verticalCount, m_horizontalCount;
        float m_verticalSpacing, m_horizontalSpacing;

        int m_verticalCounter, m_horizontalCounter;
    };
}

#endif
