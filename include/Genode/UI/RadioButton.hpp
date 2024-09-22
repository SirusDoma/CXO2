#ifndef GENODE_UI_RADIO_BUTTON_HPP
#define GENODE_UI_RADIO_BUTTON_HPP

#include <Genode/UI/CheckBox.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    class RadioButton : public CheckBox
    {
    public:
        using CheckBox::CheckBox;

        virtual void SetCheckStateChangeCallback(std::function<void(RadioButton&)> callback);
        void SetCheckedState(bool checked) override;

    protected:
        void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev) override;

    private:
        std::function<void(RadioButton&)> m_onCheckStateChanged;
    };
}

#endif
