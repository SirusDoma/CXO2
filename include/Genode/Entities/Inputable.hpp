#pragma once

#include <SFML/Window/Event.hpp>

namespace Gx
{
    class Inputable
    {
    protected:
        friend class Scene;
        friend class InputableContainer;

        virtual ~Inputable() = default;

        virtual void OnMouseMoved(const sf::Event::MouseMoved& ev)                   {}
        virtual void OnMouseButtonPressed(const sf::Event::MouseButtonPressed& ev)   {}
        virtual void OnMouseButtonReleased(const sf::Event::MouseButtonReleased& ev) {}
        virtual void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& ev)   {}

        virtual void OnKeyPressed(const sf::Event::KeyPressed& ev)   {}
        virtual void OnKeyReleased(const sf::Event::KeyReleased& ev) {}
        virtual void OnTextEntered(const sf::Event::TextEntered& ev) {}

        virtual void OnJoystickConnected(const sf::Event::JoystickConnected& ev)           {}
        virtual void OnJoystickDisconnected(const sf::Event::JoystickDisconnected& ev)     {}
        virtual void OnJoystickMoved(const sf::Event::JoystickMoved& ev)                   {}
        virtual void OnJoystickButtonPressed(const sf::Event::JoystickButtonPressed& ev)   {}
        virtual void OnJoystickButtonReleased(const sf::Event::JoystickButtonReleased& ev) {}

        virtual bool Input(const sf::Event& ev)
        {
            if (const auto e = ev.getIf<sf::Event::MouseMoved>())             { OnMouseMoved(*e);             return true; }
            if (const auto e = ev.getIf<sf::Event::MouseButtonPressed>())     { OnMouseButtonPressed(*e);     return true; }
            if (const auto e = ev.getIf<sf::Event::MouseButtonReleased>())    { OnMouseButtonReleased(*e);    return true; }
            if (const auto e = ev.getIf<sf::Event::MouseWheelScrolled>())     { OnMouseWheelScrolled(*e);     return true; }
            if (const auto e = ev.getIf<sf::Event::KeyPressed>())             { OnKeyPressed(*e);             return true; }
            if (const auto e = ev.getIf<sf::Event::KeyReleased>())            { OnKeyReleased(*e);            return true; }
            if (const auto e = ev.getIf<sf::Event::TextEntered>())            { OnTextEntered(*e);            return true; }
            if (const auto e = ev.getIf<sf::Event::JoystickConnected>())      { OnJoystickConnected(*e);      return true; }
            if (const auto e = ev.getIf<sf::Event::JoystickDisconnected>())   { OnJoystickDisconnected(*e);   return true; }
            if (const auto e = ev.getIf<sf::Event::JoystickMoved>())          { OnJoystickMoved(*e);          return true; }
            if (const auto e = ev.getIf<sf::Event::JoystickButtonPressed>())  { OnJoystickButtonPressed(*e);  return true; }
            if (const auto e = ev.getIf<sf::Event::JoystickButtonReleased>()) { OnJoystickButtonReleased(*e); return true; }

            return false;
        }
    };
}
