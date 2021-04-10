#ifndef GENODE_UI_CONTROL_HPP
#define GENODE_UI_CONTROL_HPP

#include <SFML/Window/Event.hpp>

#include <Genode/Entities.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>

#include <functional>

namespace Gx
{
    class Control : public virtual Node, public RenderableContainer, public UpdatableContainer, public InputableContainer
    {
    public:
        enum State { Normal, Hover, Active };
        struct Event
        {
            bool Handled;
            const State State;
        };

        virtual ~Control();

        virtual const sf::FloatRect GetLocalBounds() const = 0;
        const sf::FloatRect GetGlobalBounds() const;

        virtual bool IsFocused() const;
        virtual void SetFocus(bool focus);

        void SetFocusChangedCallback(std::function<void(Control&, Event&)> callback);
        void SetGainFocusCallback(std::function<void(Control&, Event&)> callback);
        void SetLostFocusCallback(std::function<void(Control&, Event&)> callback);
        void SetClickCallback(std::function<void(Control&, Event&)> callback);
        void SetDoubleClickCallback(std::function<void(Control&, Event&)> callback);

        void SetEnabled(bool enabled);
        bool IsEnabled() const;

        void SetVisible(bool visible);
        bool IsVislble() const;

        virtual void AddChild(Control *node);
        virtual void RemoveChild(Control *node);

        template<typename... Args>
        void AddChild(Control* first, Args... args);

        template<typename... Args>
        void RemoveChild(Control* first, Args... args);

    protected:
        friend class UiContainer;

        Control();

        const State GetControlState() const;
        void SetControlState(const State &state);

        const std::function<void(Control&, Event&)>& GetFocusChangedCallback();
        const std::function<void(Control&, Event&)>& GetGainFocusCallback();
        const std::function<void(Control&, Event&)>& GetLostFocusCallback();
        const std::function<void(Control&, Event&)>& GetClickCallback();
        const std::function<void(Control&, Event&)>& GetDoubleClickCallback();

        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
        virtual bool Input(sf::Event ev);

        virtual void OnMouseMove(sf::Event::MouseMoveEvent ev);
        virtual void OnMouseButtonDown(sf::Event::MouseButtonEvent ev);
        virtual void OnMouseButtonUp(sf::Event::MouseButtonEvent ev);
        virtual void OnMouseWheelScrolled(sf::Event::MouseWheelScrollEvent ev);

        virtual void OnControlChildAdded(Control *control);
        virtual void OnControlChildRemove(Control *control);

        virtual void OnControlStateChanged(Control *sender, State state);
        virtual void OnControlPress(Control *sender, sf::Event::MouseButtonEvent ev);
        virtual void OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev);
        virtual void OnControlDoubleClick(Control *sender, sf::Event::MouseButtonEvent ev);

        virtual void Invalidate() = 0;

    private:
        constexpr static const double DOUBLE_CLICK_THRESHOLD = 250.f;

        State  m_state;
        bool   m_enabled, m_visible, m_focused, m_clicked;
        double m_deltaClickDuration;

        std::function<void(Control&, Event&)> m_onClick, m_onDoubleClick, m_onFocusChanged, m_onGainFocus, m_onLostFocus;
    };
}

#include <Genode/UI/Control.inl>
#endif
