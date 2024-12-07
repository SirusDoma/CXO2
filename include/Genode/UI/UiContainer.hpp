#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/Control.hpp>
#include <Genode/SceneGraph/RenderBatchContainer.hpp>
#include <Genode/Graphics/Shapes/Rectangle.hpp>

#include <vector>
#include <functional>

namespace Gx
{
    class RadioButton;
    class UiContainer : public virtual Control, public virtual RenderBatchContainer
    {
    public:
        UiContainer();

        sf::FloatRect GetLocalBounds() const override;
        virtual void SetLocalBounds(const sf::FloatRect& bounds);

        virtual bool IsBatchingEnabled() const;
        virtual void SetBatchingEnabled(bool batchingEnabled);

        void Apply(const std::function<void(Control&)>& fun) const;

    protected:
        RenderStates Render(RenderSurface& surface, RenderStates states) const override;
        void Update(double delta) override;

        void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev) override;
        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

        void Invalidate() override;

    private:
        sf::FloatRect m_computedBounds;
        sf::FloatRect m_localBounds;
        bool m_useBatching{false};
    };
}
