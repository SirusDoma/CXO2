#pragma once

#include <Genode/UI/Button.hpp>

#include <functional>

namespace Gx
{
    class ToggleButton : public Button
    {
    public:
        ToggleButton() = default;

        virtual bool IsChecked() const;
        virtual void SetCheckedState(bool checked);

        virtual void SetCheckStateChangeCallback(std::function<void(ToggleButton&)> callback);

    protected:
        void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev) override;
        Frame GetCurrentFrame() const override;

    private:
        bool m_isChecked = false;
        std::function<void(ToggleButton&)> m_onCheckStateChanged = nullptr;
    };
}
