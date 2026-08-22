#pragma once

#include <CXO2/UI/Button.hpp>

#include <functional>

namespace Cx
{
    class ToggleButton : public Button
    {
    public:
        ToggleButton() = default;

        [[nodiscard]] virtual bool IsChecked() const;
        virtual void SetCheckedState(bool checked);

        virtual void SetCheckStateChangeCallback(std::function<void(ToggleButton&, Control::Event&)> callback);

    protected:
        void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev) override;
        [[nodiscard]] Frame GetCurrentFrame() const override;

    private:
        bool m_isChecked = false;
        std::function<void(ToggleButton&, Control::Event&)> m_onCheckStateChanged = nullptr;
    };
}
