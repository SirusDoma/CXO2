#include <CXO2/UI/Common/ChatWindow.hpp>
#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Constants/Messages/Chat.hpp>
#include <CXO2/Utilities/StringFormatter.hpp>

#include <fmt/format.h>
// #include <fmt/xchar.h>
#include <utility>

namespace Cx
{

    ChatWindow::ChatWindow(SessionContext& session) :
        m_font(),
        m_textColor(sf::Color::White),
        m_scroll(),
        m_bounds(),
        m_offset(),
        m_maxChatLength(0),
        m_characterSize(13),
        m_lineSpacing(0),
        m_chats(),
        m_session(session)
    {
    }

    sf::FloatRect ChatWindow::GetGlobalBounds() const
    {
        return Control::GetGlobalBounds();
    }

    sf::FloatRect ChatWindow::GetLocalBounds() const
    {
        return m_bounds;
    }

    void ChatWindow::SetLocalBounds(const sf::FloatRect& bounds)
    {
        m_bounds = bounds;
    }

    const Gx::Font* ChatWindow::GetFont() const
    {
        return m_font;
    }

    sf::Color ChatWindow::GetTextColor() const
    {
        return m_textColor;
    }

    void ChatWindow::SetFont(const Gx::Font& font)
    {
        if (m_font != &font)
        {
            m_font = &font;
            m_labels.clear();

            Invalidate();
        }
    }

    unsigned int ChatWindow::GetCharacterSize() const
    {
        return m_characterSize;
    }

    void ChatWindow::SetCharacterSize(const unsigned int characterSize)
    {
        if (m_characterSize != characterSize)
        {
            m_characterSize = characterSize;
            m_labels.clear();

            Invalidate();
        }
    }

    void ChatWindow::AddFallbackFont(const Gx::Font& font)
    {
        m_fallbackFonts.push_back(&font);
        m_labels.clear();
    }

    unsigned int ChatWindow::GetScrollOffset() const
    {
        return m_offset;
    }

    void ChatWindow::SetScrollOffset(const unsigned int offset)
    {
        if (m_offset != offset)
        {
            m_offset = offset;
            Invalidate();
        }
    }

    void ChatWindow::SetTextColor(const sf::Color& textColor)
    {
        if (m_textColor != textColor)
        {
            m_textColor = textColor;
            Invalidate();
        }
    }

    void ChatWindow::SetScrollBar(Gx::ScrollBar& scrollBar)
    {
        if (m_scroll != &scrollBar)
        {
            m_scroll = &scrollBar;
            m_scroll->SetValueChangedCallback([this] (auto& sender, auto& ev) { OnScrollValueChanged(sender, ev); });
        }
    }

    unsigned int ChatWindow::GetMaximumChatLength() const
    {
        return m_maxChatLength;
    }

    float ChatWindow::GetLineSpacing() const
    {
        return m_lineSpacing;
    }

    void ChatWindow::SetMaximumChatLength(const unsigned int maxLength)
    {
        if (maxLength != m_maxChatLength)
        {
            m_maxChatLength = maxLength;
            Invalidate();
        }
    }

    void ChatWindow::SetLineSpacing(const float lineSpacing)
    {
        if (m_lineSpacing != lineSpacing)
        {
            m_lineSpacing = lineSpacing;
            Invalidate();
        }
    }

    void ChatWindow::PushMessage(const sf::String& sender, const Role senderRole, const sf::String& chat)
    {
        const auto chatData = ChatMessage{ sender, senderRole, {}, chat };
        if (m_chats.size() >= m_maxChatLength && m_offset >= m_chats.size() - m_maxChatLength)
            m_offset++;

        m_chats.push_back(chatData);
        Invalidate();
    }

    void ChatWindow::PushWhisper(const sf::String& sender, const sf::String& Recipient, const sf::String& chat)
    {
        const auto chatData = ChatMessage{ sender, Role::Normal, Recipient, chat };
        if (m_chats.size() >= m_maxChatLength && m_offset >= m_chats.size() - m_maxChatLength)
            m_offset++;

        m_chats.push_back(chatData);
        Invalidate();
    }

    void ChatWindow::PushSystemMessage(const sf::String& chat)
    {
        PushMessage(sf::String(), Role::Normal, chat);
    }

    Gx::RenderStates ChatWindow::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        states = UiContainer::Render(surface, states);
        for (auto& label : m_labels)
            surface.Render(*label, states);

        return states;
    }

    void ChatWindow::OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& ev)
    {
        Control::OnMouseWheelScrolled(ev);

        if (!IsEnabled() || !m_scroll)
            return;

        const auto position = sf::Vector2f(ev.position.x, ev.position.y);
        float delta         = ev.delta;
        if (m_scroll->GetScrollOrientation() == Gx::ScrollBar::ScrollOrientation::Vertical)
            delta *= -1;

        if (GetGlobalBounds().contains(position))
            m_scroll->SetValue(m_scroll->GetValue() + delta);
    }

    void ChatWindow::Invalidate()
    {
        const unsigned int max = m_maxChatLength > 0 ? m_maxChatLength : m_chats.size();
        m_offset = std::min(m_offset, static_cast<unsigned int>(m_chats.size()));

        if (m_scroll)
        {
            m_scroll->SetMaximumValue(m_chats.size() > m_maxChatLength ? m_chats.size() - m_maxChatLength : 0);
            m_scroll->SetValue(m_offset);
        }

        if (m_font && m_labels.size() != max)
        {
            m_labels.clear();
            for (auto i = 0; i < max; i++)
            {
                auto label = std::make_unique<Gx::Label>();
                label->SetFont(*m_font);
                for (const auto font : m_fallbackFonts)
                {
                    if (font)
                        label->AddFallbackFont(*font);
                }

                label->SetColor(m_textColor);
                label->SetCharacterSize(GetCharacterSize());

                if (i == 0)
                {
                    SetVerticalRepeat(max, m_lineSpacing > 0 ? m_lineSpacing : label->GetCharacterHeight() + 5.5f);
                    SetHorizontalRepeat(1, 0);
                }

                label->SetPosition(GetNextItemPosition());
                m_labels.push_back(std::move(label));

                IncreaseSpacingCounter();
            }
        }

        for (size_t i = m_offset; i < m_offset + max; i++)
        {
            const int index = i - m_offset;
            if (index >= m_labels.size())
                break;

            if (i >= m_chats.size())
            {
                if (index < m_labels.size())
                    m_labels[index]->SetVisible(false);

                continue;
            }

            auto chat = m_chats[i];
            m_labels[index]->SetVisible(true);

            if (m_textColor == sf::Color::White)
            {
                if (!chat.Recipient.isEmpty())
                {
                    if (chat.Sender != m_session.GetName() && chat.Recipient == m_session.GetName())
                        m_labels[index]->SetColor(sf::Color(225, 230, 10));
                    else
                        m_labels[index]->SetColor(sf::Color(0, 160, 180));
                }
                else if (chat.SenderRole == Role::Administrator || chat.Sender.isEmpty())
                    m_labels[index]->SetColor(sf::Color(200, 155, 55));
                else
                    m_labels[index]->SetColor(sf::Color::White);

                // TODO: Configurable outline
                m_labels[index]->SetOutlineThickness(0.05f);
                m_labels[index]->SetOutlineColor(m_labels[index]->GetColor());
            }
            else
            {
                m_labels[index]->SetColor(m_textColor);
                m_labels[index]->SetOutlineThickness(0.f);

            }

            if (!chat.Sender.isEmpty() && !chat.Recipient.isEmpty())
            {
                if (chat.Sender != m_session.GetName() && chat.Recipient == m_session.GetName())
                    m_labels[index]->SetString(fmt::format(Constants::Messages::Chat::Lines::WHISPER_RECEIVED, chat.Recipient.toUtf32(), chat.Content.toUtf32()));
                else
                    m_labels[index]->SetString(fmt::format(Constants::Messages::Chat::Lines::WHISPER_SENT, chat.Recipient.toUtf32(), chat.Content.toUtf32()));
            }
            else if (!chat.Sender.isEmpty())
                m_labels[index]->SetString(fmt::format(Constants::Messages::Chat::Lines::MESSAGE, chat.Sender.toUtf32(), chat.Content.toUtf32()));
            else
                m_labels[index]->SetString(chat.Content);
        }
    }

    void ChatWindow::OnScrollValueChanged(Gx::ScrollBar& sender, Gx::ScrollBar::ValueChangedEvent& ev)
    {
        m_offset = static_cast<unsigned int>(ev.Value);
        Invalidate();
    }
}
