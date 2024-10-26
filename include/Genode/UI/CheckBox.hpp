#pragma once

#include <Genode/UI/Button.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    class CheckBox : public Button
    {
    public:
        CheckBox() = default;

        virtual bool IsChecked() const;
        virtual void SetCheckedState(bool checked);

        virtual void SetCheckStateChangeCallback(std::function<void(CheckBox&)> callback);

    protected:
        void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev) override;
        Frame GetCurrentFrame() const override;

    private:
        bool m_isChecked = false;
        std::function<void(CheckBox&)> m_onCheckStateChanged = nullptr;
    };
}
