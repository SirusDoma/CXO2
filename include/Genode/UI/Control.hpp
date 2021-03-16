#ifndef GENODE_UIELEMENT_HPP
#define GENODE_UIELEMENT_HPP

#include <SFML/Window/Event.hpp>

#include <Genode/Entities.hpp>
#include <Genode/SceneGraph.hpp>

#include <functional>

namespace Gx
{
    class Control : public virtual Node, public RenderableContainer, public UpdatableContainer, public InputableContainer
    {
    public:
        enum State { Normal, Hover, Active };

        virtual ~Control();

        virtual const sf::FloatRect GetLocalBounds() const = 0;
        const sf::FloatRect GetGlobalBounds() const;
        void SetClickCallback(std::function<void()> callback);

        void SetEnabled(bool enabled);
        const bool& IsEnabled() const;

        void SetVisible(bool visible);
        const bool& IsVislble() const;

        virtual void AddChild(Control *node);
        virtual void RemoveChild(Control *node);

        template<typename... Args>
        void AddChild(Control* first, Args... args);

        template<typename... Args>
        void RemoveChild(Control* first, Args... args);

    protected:
        friend class Container;

        Control();

        const State GetControlState() const;
        void SetControlState(const State &state);

        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
        virtual bool Input(sf::Event ev);

        virtual void OnMouseMove(sf::Event::MouseMoveEvent ev);
        virtual void OnMouseButtonClick(sf::Event::MouseButtonEvent ev);
        virtual void OnMouseButtonUp(sf::Event::MouseButtonEvent ev);

        virtual void OnControlChildAdded(Control *control);
        virtual void OnControlChildRemove(Control *control);

        virtual void OnControlStateChanged(State state);
        virtual void OnControlPress(sf::Event::MouseButtonEvent ev);
        virtual void OnControlClick(sf::Event::MouseButtonEvent ev);

        virtual void Invalidate() = 0;

    private:
        State m_state;
        bool  m_enabled, m_visible;

        std::function<void()> m_onClick;
    };
}

#include <Genode/UI/Control.inl>
#endif
