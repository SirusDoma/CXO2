#include <Genode/UI/Dialog.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/System/Application.hpp>

#include <functional>

namespace Gx
{
    Dialog::Dialog() :
        m_shown(false),
        m_accepted(false),
        m_scene(),
        m_sprite(),
        m_promptText(),
        m_acceptButton(),
        m_cancelButton()
    {
    }

    Dialog::Dialog(const sf::Texture &texture) :
        Dialog()
    {
        m_sprite = Sprite(texture);
    }

    Dialog::Dialog(const sf::Texture &texture, const sf::IntRect &rectangle) :
        Dialog()
    {
        m_sprite = Sprite(texture, rectangle);
    }

    Dialog::~Dialog()
    {
    }

    const sf::FloatRect Dialog::GetLocalBounds() const
    {
        return m_sprite.GetLocalBounds();
    }

    const sf::Texture *Dialog::GetTexture() const
    {
        return m_sprite.GetTexture();
    }

    void Dialog::SetTexture(const sf::Texture &texture)
    {
        m_sprite.SetTexture(texture);
    }

    const sf::IntRect &Dialog::GetTexCoords() const
    {
        return m_sprite.GetTexCoords();
    }

    void Dialog::SetTexCoords(const sf::IntRect &rectangle)
    {
        m_sprite.SetTexCoords(rectangle);
    }

    bool Dialog::IsAccepted() const
    {
        return m_accepted;
    }

    bool Dialog::IsShown() const
    {
        return m_shown;
    }

    void Dialog::SetLabel(Label *label)
    {
        if (!label)
            return;

        if (m_promptText)
            RemoveChild(m_promptText.get());

        m_promptText = std::unique_ptr<Gx::Label>(label);
        AddChild(m_promptText.get());
    }

    void Dialog::SetPromptString(const std::string &prompt)
    {
        if (!m_promptText)
            return;

        m_promptText->SetString(prompt);
    }

    void Dialog::SetAcceptButton(Button *acceptButton)
    {
        if (!acceptButton)
            return;

        if (m_acceptButton)
            RemoveChild(m_acceptButton.get());

        m_acceptButton = std::unique_ptr<Gx::Button>(acceptButton);
        m_acceptButton->SetClickCallback([this] (auto& sender, auto& ev) { OnAccepted(); });

        AddChild(m_acceptButton.get());
    }

    void Dialog::SetCancelButton(Button *cancelButton)
    {
        if (!cancelButton)
            return;

        if (m_cancelButton)
            RemoveChild(m_cancelButton.get());

        m_cancelButton = std::unique_ptr<Gx::Button>(cancelButton);
        m_cancelButton->SetClickCallback([this] (auto& sender, auto& ev) { OnCancelled(); });

        AddChild(m_cancelButton.get());
    }

    void Dialog::SetAcceptCallback(std::function<void()> callback)
    {
        m_onAccepted = callback;
    }

    void Dialog::SetCancelCallback(std::function<void()> callback)
    {
        m_onCancelled = callback;
    }

    void Dialog::Show(Scene *scene, const std::string& prompt, bool enableBackDrop)
    {
        if (m_shown)
            return;

        m_scene = scene;
        if (m_scene)
        {
            auto screenSize = m_scene->GetView().getSize();
            unsigned int x = static_cast<unsigned int>((screenSize.x / 2.f) - (GetLocalBounds().width / 2.f));
            unsigned int y = static_cast<unsigned int>((screenSize.y / 2.f) - (GetLocalBounds().height / 2.f));

            SetOrigin(0.f, 0.f);
            SetPosition(x, y);
            if (enableBackDrop)
            {
                m_backdrop = Rectangle(screenSize);
                m_backdrop.SetColor(sf::Color(0, 0, 0, 255 / 2));
            }
            else
                m_backdrop = Rectangle(sf::Vector2f(0, 0));

            if (m_promptText)
                m_promptText->SetString(prompt);

            m_scene->PushOverlay(this);
            m_shown = true;

            sf::RenderTarget& target = m_scene->GetApplication();
            auto mousePosition = target.mapPixelToCoords(sf::Mouse::getPosition(static_cast<sf::RenderWindow&>(target)));

            m_acceptButton->SetFocus(m_acceptButton->GetGlobalBounds().contains(mousePosition.x, mousePosition.y));
            m_cancelButton->SetFocus(m_cancelButton->GetGlobalBounds().contains(mousePosition.x, mousePosition.y));

            OnShown(*scene);
            Invalidate();
        }
    }

    void Dialog::Close()
    {
        if (!m_shown)
            return;

        m_scene->CloseOverlay();
        m_shown = false;

        OnClose();
    }

    sf::RenderStates Dialog::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        if (m_backdrop.GetSize().x > 0 && m_backdrop.GetSize().y > 0)
            target.draw(m_backdrop, sf::Transform::Identity);

        states.transform *= GetTransform();
        target.draw(m_sprite, states);

        return RenderableContainer::Render(target, states);
    }

    void Dialog::OnKeyDown(sf::Event::KeyEvent ev)
    {
        UiContainer::OnKeyDown(ev);

        if (ev.code == sf::Keyboard::Enter)
            OnAccepted();
        else if (ev.code == sf::Keyboard::Escape)
            OnCancelled();
    }

    void Dialog::OnShown(Scene &scene)
    {
    }

    void Dialog::OnClose()
    {
    }

    void Dialog::OnAccepted()
    {
        m_accepted = true;
        if (m_onAccepted)
            m_onAccepted();

        Close();
    }

    void Dialog::OnCancelled()
    {
        m_accepted = false;
        if (m_onCancelled)
            m_onCancelled();

        Close();
    }

    const Scene *Dialog::GetScene()
    {
        return m_scene;
    }

    void Dialog::Invalidate()
    {
        UiContainer::Invalidate();
    }
}
