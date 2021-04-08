#ifndef GENODE_UI_REPEATER_HPP
#define GENODE_UI_REPEATER_HPP

#include <Genode/UI/UiContainer.hpp>
#include <functional>

namespace Gx
{
    class List : public virtual UiContainer
    {
    public:
        List();
        List(int verticalCount, float verticalSpacing);
        List(int verticalCount, float verticalSpacing, int horizontalCount, float horizontalSpacing);

        virtual ~List();

        void SetVerticalRepeat(int count, float spacing);
        void SetHorizontalRepeat(int count, float spacing);

        int GetRepeatCount() const;
        int GetVerticalCount() const;
        int GetHorizontalCount() const;

        float GetVerticalSpacing() const;
        float GetHorizontalSpacing() const;

        virtual void AddChild(Control *control);
        virtual void RemoveChild(Control *control);

        using Control::AddChild;
        using Control::RemoveChild;

        virtual void ClearChildren();

        void Apply(std::function<void(Control*)> fun);

    protected:
        bool IsAvailable() const;
        sf::Vector2f GetNextItemPosition() const;
        void IncreaseSpacingCounter();

        virtual void Invalidate();

    private:
        int m_verticalCount, m_horizontalCount;
        float m_verticalSpacing, m_horizontalSpacing;

        int m_verticalCounter, m_horizontalCounter;
    };
}

#endif
