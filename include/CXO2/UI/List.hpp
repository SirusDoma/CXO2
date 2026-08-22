#pragma once

#include <CXO2/UI/UiContainer.hpp>

#include <functional>

namespace Cx
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

        [[nodiscard]] Order GetOrder() const;
        void SetOrder(Order order);

        void SetVerticalRepeat(int count, float spacing);
        void SetHorizontalRepeat(int count, float spacing);

        [[nodiscard]] int GetRepeatCount() const;
        [[nodiscard]] int GetVerticalCount() const;
        [[nodiscard]] int GetHorizontalCount() const;

        [[nodiscard]] float GetVerticalSpacing() const;
        [[nodiscard]] float GetHorizontalSpacing() const;

        void AddLayout(const LayoutItem& layout);
        void ClearLayouts();

        void OnChildAdded(Gx::Node& node) override;
        void OnChildRemove(Gx::Node& node) override;

    protected:
        [[nodiscard]] bool IsSpaceAvailable() const;

        [[nodiscard]] sf::Vector2f GetNextItemPosition() const;

        void IncreaseSpacingCounter();
        void DecreaseSpacingCounter();

        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void Invalidate() override;

    private:
        Order m_order;
        int m_verticalCount, m_horizontalCount;
        float m_verticalSpacing, m_horizontalSpacing;

        int m_verticalCounter, m_horizontalCounter;
        std::vector<LayoutItem> m_layouts;
    };
}
