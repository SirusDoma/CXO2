#pragma once

#include <SFML/Window/Event.hpp>

#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>

#include <functional>

namespace Cx
{
    class Control : public virtual Gx::Node, public virtual Gx::RenderableContainer, public virtual Gx::UpdatableContainer, public virtual Gx::InputableContainer
    {
    public:
        enum class State { Normal, Hover, Active };
        struct Event
        {
            bool Handled;
            Control::State State;
            double Delta;
        };

        [[nodiscard]] virtual sf::FloatRect GetLocalBounds() const = 0;
        [[nodiscard]] virtual sf::FloatRect GetGlobalBounds() const;

        [[nodiscard]] virtual bool IsFocused() const;
        virtual void SetFocus(bool focus);

        void SetFocusChangedCallback(std::function<void(Control&, Event&)> callback);
        void SetGainFocusCallback(std::function<void(Control&, Event&)> callback);
        void SetLostFocusCallback(std::function<void(Control&, Event&)> callback);
        void SetClickCallback(std::function<void(Control&, Event&)> callback);
        void SetHoldClickCallback(std::function<void(Control&, Event&)> callback);
        void SetDoubleClickCallback(std::function<void(Control&, Event&)> callback);
        void SetScrollWheelCallback(std::function<void(Control&, Event&)> callback);

        void SetEnabled(bool enabled);
        [[nodiscard]] bool IsEnabled() const;

        void SetVisible(bool visible) override;

    protected:
        Control();
        Control(const Control&) = default;
        Control(Control&&) = default;

        Control& operator=(const Control&) = default;

        [[nodiscard]] virtual State GetControlState() const;
        virtual void SetControlState(const State& state);

        [[nodiscard]] const std::function<void(Control&, Event&)>& GetFocusChangedCallback() const;
        [[nodiscard]] const std::function<void(Control&, Event&)>& GetGainFocusCallback() const;
        [[nodiscard]] const std::function<void(Control&, Event&)>& GetLostFocusCallback() const;
        [[nodiscard]] const std::function<void(Control&, Event&)>& GetClickCallback() const;
        [[nodiscard]] const std::function<void(Control&, Event&)>& GetHoldClickCallback() const;
        [[nodiscard]] const std::function<void(Control&, Event&)>& GetDoubleClickCallback() const;
        [[nodiscard]] const std::function<void(Control&, Event&)>& GetScrollWheelCallback() const;

        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
        bool Input(const sf::Event& ev) override;

        void OnMouseMoved(const sf::Event::MouseMoved& ev) override;
        void OnMouseButtonPressed(const sf::Event::MouseButtonPressed& ev) override;
        void OnMouseButtonReleased(const sf::Event::MouseButtonReleased& ev) override;
        void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& ev) override;

        void OnChildAdded(Gx::Node& node) override;
        void OnChildRemove(Gx::Node& node) override;

        virtual void OnControlChildAdded(Control& control);
        virtual void OnControlChildRemove(Control& control);

        virtual void OnControlStateChanged(Control& sender, State state);
        virtual void OnControlPress(Control& sender, const sf::Event::MouseButtonPressed& ev);
        virtual void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev);
        virtual void OnControlDoubleClick(Control& sender, const sf::Event::MouseButtonPressed& ev);

        virtual void Invalidate() = 0;

    private:
        constexpr static double DOUBLE_CLICK_THRESHOLD = 500.f;
        constexpr static double HOLD_CLICK_THRESHOLD   = 50.f;

        State  m_state;
        bool   m_enabled, m_focused, m_clicked, m_doubleClicked;
        double m_deltaClickDuration, m_deltaHoldDuration;

        std::function<void(Control&, Event&)> m_onClick, m_onHoldClick, m_onDoubleClick, m_onScrollWheel,
                                              m_onFocusChanged, m_onGainFocus, m_onLostFocus;
    };
}
