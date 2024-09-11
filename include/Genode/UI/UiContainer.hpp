#ifndef GENODE_UI_CONTAINER_HPP
#define GENODE_UI_CONTAINER_HPP

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
        ~UiContainer() override = default;

        sf::FloatRect GetLocalBounds() const override;
        void SetRadioActiveCallback(std::function<void(RadioButton*)> callback);

        virtual bool IsBatchingEnabled() const;
        virtual void SetBatchingEnabled(bool batchingEnabled);

    protected:
        RenderStates Render(RenderSurface &surface, RenderStates states) const override;
        void Update(double delta) override;
        bool Input(const sf::Event& ev) override;

        void OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev) override;
        void OnKeyDown(const sf::Event::KeyEvent& ev) override;

        void Invalidate() override;

    private:
        sf::FloatRect m_localBounds;
        RadioButton *m_activeRadio;
        bool m_radioHandled, m_useBatching{false};
        std::function<void(RadioButton*)> m_radioCallback;
    };
}

#endif
