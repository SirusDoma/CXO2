#ifndef GENODE_UI_REPEATER_HPP
#define GENODE_UI_REPEATER_HPP

#include <Genode/UI/UiContainer.hpp>
#include <Genode/Graphics/SpriteBatch.hpp>
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

        ~List() override = default;

        void UseBatching(bool batching);
        void SetBatchMode(SpriteBatch::BatchMode batchMode) const;

        Order GetOrder() const;
        void SetOrder(Order order);

        void SetVerticalRepeat(int count, float spacing);
        void SetHorizontalRepeat(int count, float spacing);

        int GetRepeatCount() const;
        int GetVerticalCount() const;
        int GetHorizontalCount() const;

        float GetVerticalSpacing() const;
        float GetHorizontalSpacing() const;

        void AddLayout(const LayoutItem &layout);
        void ClearLayouts();

        void AddChild(Gx::Node *node) override;
        void RemoveChild(Gx::Node *node) override;

        void AddChild(Control *control) override;
        void RemoveChild(Control *control) override;

        using Control::AddChild;
        using Control::RemoveChild;

        void ClearChildren() override;

        void Apply(const std::function<void(Control*)>& fun) const;

    protected:
        bool IsAvailable() const;
        sf::Vector2f GetNextItemPosition() const;
        void IncreaseSpacingCounter();

        void Update(const double delta) override;
        RenderStates Render(RenderSurface &surface, RenderStates states) const override;

        void Invalidate() override;

    private:
        Order m_order;
        int m_verticalCount, m_horizontalCount;
        float m_verticalSpacing, m_horizontalSpacing;

        int m_verticalCounter, m_horizontalCounter;
        std::vector<LayoutItem> m_layouts;
        mutable SpriteBatch m_batcher;
        bool m_useBatching{true};
    };
}

#endif
