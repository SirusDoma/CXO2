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
        ~RadioButton() override = default;

        using CheckBox::CheckBox;

        virtual void SetCheckStateChangeCallback(std::function<void(RadioButton*)> callback);

        void SetCheckedState(bool checked) override;
        virtual void Pair(RadioButton *radio);
        virtual void Unpair(RadioButton *radio);

        template<typename... Args>
        void Pair(RadioButton *first, Args... args);

        void UnpairAll();

    protected:
        void OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev) override;

    private:
        std::vector<RadioButton*> m_pairs;
        std::function<void(RadioButton*)> m_onCheckStateChanged;
    };
}

#include <Genode/UI/RadioButton.inl>
#endif
