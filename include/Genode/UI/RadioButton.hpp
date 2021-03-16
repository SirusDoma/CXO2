#ifndef GENODE_RADIO_BUTTON_HPP
#define GENODE_RADIO_BUTTON_HPP

#include <Genode/UI/CheckBox.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    class RadioButton : public CheckBox
    {
    public:
        using CheckBox::CheckBox;

        virtual void SetCheckedState(bool checked);
        virtual void Pair(RadioButton *radio);
        virtual void Unpair(RadioButton *radio);

        template<typename... Args>
        void Pair(RadioButton *first, Args... args);

        void UnpairAll();

    protected:
        virtual void OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev);

    private:
        std::vector<RadioButton*> m_pairs;
    };
}

#include <Genode/UI/RadioButton.inl>
#endif
