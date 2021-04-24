#ifndef GENODE_UI_CONTAINER_HPP
#define GENODE_UI_CONTAINER_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Genode/UI/Control.hpp>
#include <Genode/Graphics/Shapes/Rectangle.hpp>

#include <vector>
#include <functional>

namespace Gx
{
    class RadioButton;
    class UiContainer : public Control
    {
    public:
        UiContainer();
        virtual ~UiContainer();

        const sf::FloatRect GetLocalBounds() const;
        void SetRadioActiveCallback(std::function<void(RadioButton*)> callback);

    protected:
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
        virtual void Update(double delta);
        virtual bool Input(sf::Event ev);

        virtual void OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev);
        virtual void OnKeyDown(sf::Event::KeyEvent ev);

        virtual void Invalidate();

    private:
        sf::FloatRect m_localBounds;
        RadioButton *m_activeRadio;
        std::function<void(RadioButton*)> m_radioCallback;
    };
}

#endif
