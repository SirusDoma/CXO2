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

        m_promptText = label;
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
            RemoveChild(m_acceptButton);

        m_acceptButton = acceptButton;
        m_acceptButton->SetClickCallback([this](auto _) { OnAccepted(); });
        AddChild(m_acceptButton);
    }

    void Dialog::SetCancelButton(Button *cancelButton)
    {
        if (!cancelButton)
            return;

        if (m_cancelButton)
            RemoveChild(m_cancelButton);

        m_cancelButton = cancelButton;
        m_cancelButton->SetClickCallback([this](auto _) { OnCancelled(); });
        AddChild(m_cancelButton);
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
            float x = (screenSize.x / 2.f) - (GetLocalBounds().width / 2.f);
            float y = (screenSize.y / 2.f) - (GetLocalBounds().height / 2.f);

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
        }
    }

    void Dialog::Close()
    {
        if (!m_shown)
            return;

        m_scene->CloseOverlay();
        m_shown = false;
    }

    sf::RenderStates Dialog::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        if (m_backdrop.GetSize().x > 0 && m_backdrop.GetSize().y > 0)
            target.draw(m_backdrop, sf::Transform::Identity);

        states.transform *= GetTransform();
        target.draw(m_sprite, states);
        if (m_promptText)
            target.draw(*m_promptText, states);

        return RenderableContainer::Render(target, states);
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

    void Dialog::Invalidate()
    {
    }
}