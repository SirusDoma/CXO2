#ifndef GENODE_CHECKBOX_HPP
#define GENODE_CHECKBOX_HPP

#include <Genode/UI/Button.hpp>
#include <Genode/Graphics.hpp>

#include <functional>
#include <unordered_map>

namespace Gx
{
    class CheckBox : public Button
    {
    public:
        using Button::Button;

        const bool IsChecked() const;
        void SetCheckedState(bool checked);

    protected:
        virtual sf::RenderStates Render(sf::RenderTarget& target, sf::RenderStates states) const;

        virtual void OnMouseButtonUp(sf::Event::MouseButtonEvent ev);

    private:
        bool m_isChecked;
        std::function<void()> m_stateChanged;
    };
}

#endif
