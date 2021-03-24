#ifndef GENODE_CHECKBOX_HPP
#define GENODE_CHECKBOX_HPP

#include <Genode/UI/Button.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    class CheckBox : public Button
    {
    public:
        using Button::Button;

        virtual bool IsChecked() const;
        virtual void SetCheckedState(bool checked);

        virtual void SetCheckStateChangeCallback(std::function<void(CheckBox*)> callback);

    protected:
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev);

        virtual void Invalidate();

    private:
        bool m_isChecked = false;
        std::function<void(CheckBox*)> m_onCheckStateChanged = nullptr;
    };
}

#endif
