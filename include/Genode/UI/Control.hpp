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
    class Control : public virtual Node, public virtual RenderableContainer, public virtual UpdatableContainer, public virtual InputableContainer
    {
    public:
        enum class State { Normal, Hover, Active };
        struct Event
        {
            bool Handled;
            const Control::State State;
            double Delta;
        };

        ~Control() override = default;

        virtual sf::FloatRect GetLocalBounds() const = 0;
        sf::FloatRect GetGlobalBounds() const;

        virtual bool IsFocused() const;
        virtual void SetFocus(bool focus);

        void SetFocusChangedCallback(std::function<void(Control&, Event&)> callback);
        void SetGainFocusCallback(std::function<void(Control&, Event&)> callback);
        void SetLostFocusCallback(std::function<void(Control&, Event&)> callback);
        void SetClickCallback(std::function<void(Control&, Event&)> callback);
        void SetHoldClickCallback(std::function<void(Control&, Event&)> callback);
        void SetDoubleClickCallback(std::function<void(Control&, Event&)> callback);
        void SetScrollWheelCallback(std::function<void(Control&, Event&)> callback);

        void SetEnabled(bool enabled);
        bool IsEnabled() const;

        void SetVisible(bool visible);
        bool IsVislble() const;

        virtual void AddChild(Control *node);
        virtual void RemoveChild(Control *node);

        void AddChild(Gx::Node *node) override;
        void RemoveChild(Gx::Node *node) override;

        template<typename... Args>
        void AddChild(Control* first, Args... args);

        template<typename... Args>
        void RemoveChild(Control* first, Args... args);

    protected:
        Control();

        virtual const State GetControlState() const;
        virtual void SetControlState(const State &state);

        const std::function<void(Control&, Event&)>& GetFocusChangedCallback();
        const std::function<void(Control&, Event&)>& GetGainFocusCallback();
        const std::function<void(Control&, Event&)>& GetLostFocusCallback();
        const std::function<void(Control&, Event&)>& GetClickCallback();
        const std::function<void(Control&, Event&)>& GetHoldClickCallback();
        const std::function<void(Control&, Event&)>& GetDoubleClickCallback();
        const std::function<void(Control&, Event&)>& GetScrollWheelCallback();

        void Update(const double delta) override;
        RenderStates Render(RenderSurface &surface, RenderStates states) const override;
        bool Input(sf::Event ev) override;

        void OnMouseMove(const sf::Event::MouseMoveEvent ev) override;
        void OnMouseButtonDown(const sf::Event::MouseButtonEvent ev) override;
        void OnMouseButtonUp(const sf::Event::MouseButtonEvent ev) override;
        void OnMouseWheelScrolled(const sf::Event::MouseWheelScrollEvent ev) override;

        virtual void OnControlChildAdded(Control *control);
        virtual void OnControlChildRemove(Control *control);

        virtual void OnControlStateChanged(Control *sender, State state);
        virtual void OnControlPress(Control *sender, sf::Event::MouseButtonEvent ev);
        virtual void OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev);
        virtual void OnControlDoubleClick(Control *sender, sf::Event::MouseButtonEvent ev);

        virtual void Invalidate() = 0;

    private:
        constexpr static double DOUBLE_CLICK_THRESHOLD = 500.f;
        constexpr static double HOLD_CLICK_THRESHOLD   = 50.f;

        State  m_state;
        bool   m_enabled, m_visible, m_focused, m_clicked;
        double m_deltaClickDuration, m_deltaHoldDuration;

        std::function<void(Control&, Event&)> m_onClick, m_onHoldClick, m_onDoubleClick, m_onScrollWheel, m_onFocusChanged, m_onGainFocus, m_onLostFocus;
    };
}

#include <Genode/UI/Control.inl>
#endif
