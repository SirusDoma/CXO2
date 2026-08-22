#pragma once

#include <CXO2/UI/ToggleButton.hpp>

#include <functional>
#include <unordered_map>

namespace Cx
{
    class RadioButton : public ToggleButton
    {
    public:
        using ToggleButton::ToggleButton;

        virtual void SetCheckStateChangeCallback(std::function<void(RadioButton&, Control::Event&)> callback);
        void SetCheckedState(bool checked) override;

    protected:
        void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev) override;

    private:
        std::function<void(RadioButton&, Control::Event&)> m_onCheckStateChanged;
    };
}
