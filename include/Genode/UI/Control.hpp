#ifndef GENODE_UIELEMENT_HPP
#define GENODE_UIELEMENT_HPP

#include <Genode/Entities.hpp>
#include <Genode/SceneGraph/Node.hpp>

#include <functional>

namespace Gx
{
    class Control : public Node, public Renderable, public Updatable, public Inputable
    {
    public:
        enum State { Normal, Hover, Active };

        Control();
        virtual ~Control();

        virtual const sf::FloatRect GetLocalBounds() const = 0;
        const sf::FloatRect GetGlobalBounds() const;

        virtual void AddChild(Node *child);

        void SetEnabled(bool enabled);
        const bool& IsEnabled() const;

        void SetVisible(bool visible);
        const bool& IsVislble() const;

        void SetClickCallback(std::function<void()> callback);

    protected:
        virtual void Update(double delta);

        const State GetControlState() const;
        void SetControlState(const State &state);

        virtual void OnMouseMove(sf::Event::MouseMoveEvent ev);
        virtual void OnMouseButtonClick(sf::Event::MouseButtonEvent ev);
        virtual void OnMouseButtonUp(sf::Event::MouseButtonEvent ev);

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

#endif
