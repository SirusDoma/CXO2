#pragma once

#include <Genode/UI/UiContainer.hpp>

#include <functional>

namespace Gx
{
    class List : public virtual UiContainer
    {
    public:
        enum class Order
        {
            Vertical,
            Horizontal
        };

        struct LayoutItem
        {
            sf::Vector2f Origin;
            sf::Vector2f Position;
            float        Rotation;
            sf::Vector2f Scale;
        };

        List();
        List(int verticalCount, float verticalSpacing);
        List(int verticalCount, float verticalSpacing, int horizontalCount, float horizontalSpacing);

        Order GetOrder() const;
        void SetOrder(Order order);

        void SetVerticalRepeat(int count, float spacing);
        void SetHorizontalRepeat(int count, float spacing);

        int GetRepeatCount() const;
        int GetVerticalCount() const;
        int GetHorizontalCount() const;

        float GetVerticalSpacing() const;
        float GetHorizontalSpacing() const;

        void AddLayout(const LayoutItem& layout);
        void ClearLayouts();

        void OnChildAdded(Node& node) override;
        void OnChildRemove(Node& node) override;

    protected:
        bool IsSpaceAvailable() const;

        sf::Vector2f GetNextItemPosition() const;

        void IncreaseSpacingCounter();
        void DecreaseSpacingCounter();

        void Update(double delta) override;
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;

        void Invalidate() override;

    private:
        Order m_order;
        int m_verticalCount, m_horizontalCount;
        float m_verticalSpacing, m_horizontalSpacing;

        int m_verticalCounter, m_horizontalCounter;
        std::vector<LayoutItem> m_layouts;
    };
}
