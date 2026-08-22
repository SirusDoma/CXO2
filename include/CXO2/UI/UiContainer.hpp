#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <CXO2/UI/Control.hpp>
#include <Genode/SceneGraph/RenderBatchContainer.hpp>
#include <Genode/Graphics/Shapes/Rectangle.hpp>

#include <vector>
#include <functional>

namespace Cx
{
    class RadioButton;
    class UiContainer : public virtual Control, public virtual Gx::RenderBatchContainer
    {
    public:
        UiContainer();

        [[nodiscard]] sf::FloatRect GetGlobalBounds() const override;

        [[nodiscard]] sf::FloatRect GetLocalBounds() const override;
        virtual void SetLocalBounds(const sf::FloatRect& bounds);

        [[nodiscard]] virtual bool IsBatchingEnabled() const;
        virtual void SetBatchingEnabled(bool batchingEnabled);

        void Apply(const std::function<void(Control&)>& fun) const;

    protected:
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
        void Update(const sf::Time& delta) override;

        void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev) override;
        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

        void Invalidate() override;

    private:
        sf::FloatRect m_computedLocalBounds;
        sf::FloatRect m_computedGlobalBounds;
        sf::FloatRect m_localBounds;
        bool m_useBatching{false};
    };
}
