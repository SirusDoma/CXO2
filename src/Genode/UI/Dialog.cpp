#include <Genode/UI/Dialog.hpp>

#include <Genode/UI/Label.hpp>
#include <Genode/UI/Button.hpp>
#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/System/Application.hpp>

#include <functional>

namespace Gx
{
    Dialog::Dialog() :
        m_sprite(),
        m_acceptButton(),
        m_cancelButton(),
        m_promptText(),
        m_scene(),
        m_accepted(false),
        m_shown(false)
    {
    }

    Dialog::Dialog(const Dialog& copy) :
        UiContainer(copy),
        Node(copy),
        m_sprite(copy.m_sprite),
        m_acceptButton(copy.m_acceptButton),
        m_cancelButton(copy.m_cancelButton),
        m_promptText(copy.m_promptText),
        m_scene(copy.m_scene),
        m_accepted(copy.m_accepted),
        m_shown(copy.m_shown)
    {
        // Rewire callbacks due to copy constructor
        if (m_acceptButton)
            SetAcceptButton(*m_acceptButton);

        if (m_cancelButton)
            SetCancelButton(*m_cancelButton);
    }

    Dialog::Dialog(const sf::Texture& texture) :
        Dialog()
    {
        m_sprite = Sprite(texture);
    }

    Dialog::Dialog(const sf::Texture& texture, const sf::IntRect& rectangle) :
        Dialog()
    {
        m_sprite = Sprite(texture, rectangle);
    }

    sf::FloatRect Dialog::GetLocalBounds() const
    {
        return m_sprite.GetLocalBounds();
    }

    const sf::Texture* Dialog::GetTexture() const
    {
        return m_sprite.GetTexture();
    }

    void Dialog::SetTexture(const sf::Texture& texture)
    {
        m_sprite.SetTexture(texture);
    }

    const sf::IntRect& Dialog::GetTexCoords() const
    {
        return m_sprite.GetTexCoords();
    }

    void Dialog::SetTexCoords(const sf::IntRect& rectangle)
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

    void Dialog::SetLabel(Label& label)
    {
        if (m_promptText)
            RemoveChild(*m_promptText);

        m_promptText = &label;
        AddChild(label);
    }

    void Dialog::SetPromptString(const std::string& prompt)
    {
        if (!m_promptText)
            return;

        m_promptText->SetString(prompt);
    }

    void Dialog::SetAcceptButton(Button& acceptButton)
    {
        if (m_acceptButton)
            RemoveChild(*m_acceptButton);

        m_acceptButton = &acceptButton;
        m_acceptButton->SetClickCallback([&] (auto& sender, auto& ev) { OnAccepted(); });

        AddChild(acceptButton);
    }

    void Dialog::SetCancelButton(Button& cancelButton)
    {
        if (m_cancelButton)
            RemoveChild(*m_cancelButton);

        m_cancelButton = &cancelButton;
        m_cancelButton->SetClickCallback([&] (auto& sender, auto& ev) { OnCancelled(); });

        AddChild(cancelButton);
    }

    void Dialog::SetAcceptCallback(std::function<void()> callback)
    {
        m_onAccepted = std::move(callback);
    }

    void Dialog::SetCancelCallback(std::function<void()> callback)
    {
        m_onCancelled = std::move(callback);
    }

    void Dialog::Show(Scene* scene)
    {
        Show(scene, std::string(), true);
    }

    void Dialog::Show(Scene* scene, const std::string& prompt, bool enableBackDrop)
    {
        if (m_shown)
            return;

        m_scene = scene;
        if (m_scene)
        {
            const auto view      = m_scene->GetView();
            const auto center    = view.getCenter();
            const unsigned int x = static_cast<unsigned int>(center.x - (GetLocalBounds().size.x / 2.f));
            const unsigned int y = static_cast<unsigned int>(center.y - (GetLocalBounds().size.y / 2.f));

            SetOrigin(0.f, 0.f);
            SetPosition(x, y);
            if (enableBackDrop)
            {
                m_backdrop = Rectangle(view.getSize());
                m_backdrop.SetColor(sf::Color(0, 0, 0, 255 / 2));
            }
            else
                m_backdrop = Rectangle(sf::Vector2f(0, 0));

            if (m_promptText)
                m_promptText->SetString(prompt);

            m_scene->PushOverlay(*this);
            m_shown = true;

            const sf::RenderWindow& target = m_scene->GetApplication();
            const auto mousePosition = target.mapPixelToCoords(sf::Mouse::getPosition(target));

            if (m_acceptButton)
                m_acceptButton->SetFocus(m_acceptButton->GetGlobalBounds().contains(sf::Vector2f(mousePosition.x, mousePosition.y)));

            if (m_cancelButton)
                m_cancelButton->SetFocus(m_cancelButton->GetGlobalBounds().contains(sf::Vector2f(mousePosition.x, mousePosition.y)));

            OnShown(*scene);
            Invalidate();
        }
    }

    void Dialog::Close()
    {
        if (!m_scene || m_scene->GetCurrentOverlay() != this)
            return;

        m_scene->CloseOverlay();
        m_shown = false;

        OnClose();
    }

    RenderStates Dialog::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        if (m_backdrop.GetSize().x > 0 && m_backdrop.GetSize().y > 0)
            surface.Render(m_backdrop, sf::Transform::Identity);

        states.transform *= GetTransform();
        surface.Render(m_sprite, states);

        return RenderableContainer::Render(surface, states);
    }

    void Dialog::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        UiContainer::OnKeyPressed(ev);

        if (ev.code == sf::Keyboard::Key::Enter)
            OnAccepted();
        else if (ev.code == sf::Keyboard::Key::Escape)
            OnCancelled();
    }

    void Dialog::OnShown(Scene& scene)
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

    const Scene* Dialog::GetScene()
    {
        return m_scene;
    }

    Label* Dialog::GetLabel() const
    {
        return m_promptText;
    }

    Button* Dialog::GetAcceptButton() const
    {
        return m_acceptButton;
    }

    Button* Dialog::GetCancelButton() const
    {
        return m_cancelButton;
    }

    void Dialog::Invalidate()
    {
        UiContainer::Invalidate();
    }
}
