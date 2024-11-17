#pragma once

#include <Genode/UI/ToggleButton.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    class RadioButton : public ToggleButton
    {
    public:
        using ToggleButton::ToggleButton;

        virtual void SetCheckStateChangeCallback(std::function<void(RadioButton&)> callback);
        void SetCheckedState(bool checked) override;

    protected:
        void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev) override;

    private:
        std::function<void(RadioButton&)> m_onCheckStateChanged;
    };
}
