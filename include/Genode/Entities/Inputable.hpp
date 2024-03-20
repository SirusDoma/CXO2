#ifndef GENODE_ENTITIES_INPUTABLE_HPP
#define GENODE_ENTITIES_INPUTABLE_HPP

#include <SFML/Window/Event.hpp>

namespace Gx
{
    class Inputable
    {
    protected:
        friend class Scene;
        friend class InputableContainer;

        virtual ~Inputable() = default;

        virtual void OnMouseMove(sf::Event::MouseMoveEvent ev)                 {}
        virtual void OnMouseButtonDown(sf::Event::MouseButtonEvent ev)         {}
        virtual void OnMouseButtonUp(sf::Event::MouseButtonEvent ev)           {}
        virtual void OnMouseWheelScrolled(sf::Event::MouseWheelScrollEvent ev) {}

        virtual void OnKeyDown(sf::Event::KeyEvent ev)  {}
        virtual void OnKeyUp(sf::Event::KeyEvent ev)    {}
        virtual void OnKeyType(sf::Event::TextEvent ev) {}

        virtual bool Input(const sf::Event ev)
        {
            switch (ev.type)
            {
                case sf::Event::MouseMoved:          OnMouseMove(ev.mouseMove);                 return true;
                case sf::Event::MouseButtonPressed:  OnMouseButtonDown(ev.mouseButton);         return true;
                case sf::Event::MouseButtonReleased: OnMouseButtonUp(ev.mouseButton);           return true;
                case sf::Event::MouseWheelScrolled:  OnMouseWheelScrolled(ev.mouseWheelScroll); return true;
                case sf::Event::KeyPressed:          OnKeyDown(ev.key);                         return true;
                case sf::Event::KeyReleased:         OnKeyUp(ev.key);                           return true;
                case sf::Event::TextEntered:         OnKeyType(ev.text);                        return true;
                default: break;
            }

            return false;
        }
    };
}

#endif