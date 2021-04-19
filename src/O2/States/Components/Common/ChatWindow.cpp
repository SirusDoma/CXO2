#include <O2/States/Components/Common/ChatWindow.hpp>

ChatWindow::ChatWindow() :
    m_font(),
    m_scroll(),
    m_bounds(),
    m_characterSize(13),
    m_offset()
{
}

ChatWindow::ChatWindow(const sf::Font &font, sf::FloatRect localBounds, unsigned int characterSize) :
    m_font(&font),
    m_scroll(),
    m_bounds(localBounds),
    m_characterSize(characterSize),
    m_offset()
{

}

const sf::FloatRect ChatWindow::GetLocalBounds() const
{
    return m_bounds;
}

void ChatWindow::SetLocalBounds(const sf::FloatRect &bounds)
{
    m_bounds = bounds;
}

const sf::Font *ChatWindow::GetFont() const
{
    return m_font;
}

void ChatWindow::SetFont(const sf::Font &font)
{
    if (m_font != &font)
    {
        m_font = &font;
        Invalidate();
    }
}

unsigned int ChatWindow::GetCharacterSize() const
{
    return m_characterSize;
}

void ChatWindow::SetCharacterSize(unsigned int characterSize)
{
    if (m_characterSize != characterSize)
    {
        m_characterSize = characterSize;
        Invalidate();
    }
}

Gx::ScrollBar *ChatWindow::GetScrollBar() const
{
    return m_scroll;
}

void ChatWindow::SetScrollBar(Gx::ScrollBar &scroll)
{
    if (m_scroll != &scroll)
    {
        m_scroll = &scroll;
        m_scroll->SetValueChangedCallback([=] (auto& sender, float value)
        {
            m_offset = static_cast<unsigned int>(value);
            Invalidate();
        });
    }
}

unsigned int ChatWindow::GetMaximumChatLength() const
{
    return m_maxChatLength;
}

void ChatWindow::SetMaximumChatLength(unsigned int maxLength)
{
    if (maxLength != m_maxChatLength)
    {
        m_maxChatLength = maxLength;
        Invalidate();
    }
}

void ChatWindow::PushMessage(Room::PlayerInfo player, sf::String chat)
{
    auto chatData = Room::ChatData{player, chat};
    // Do something if player is self

    if (m_chats.size() >= m_maxChatLength && m_offset >= m_chats.size() - m_maxChatLength)
        m_offset++;

    m_chats.push_back(chatData);
    Invalidate();
}

sf::RenderStates ChatWindow::Render(sf::RenderTarget &target, sf::RenderStates states) const
{
    states = UiContainer::Render(target, states);
    for (auto& label : m_labels)
        target.draw(*label, states);

    return states;
}

void ChatWindow::OnMouseWheelScrolled(sf::Event::MouseWheelScrollEvent ev)
{
    Control::OnMouseWheelScrolled(ev);

    if (!IsEnabled() || !m_scroll)
        return;

    auto position = sf::Vector2f(ev.x, ev.y);
    float delta   = ev.delta;
    if (m_scroll->GetScrollOrientation() == Gx::ScrollBar::ScrollOrientation::Vertical)
        delta *= -1;

    if (GetGlobalBounds().contains(position))
        m_scroll->SetValue(m_scroll->GetValue() + delta);
}

void ChatWindow::Invalidate()
{
    unsigned int max = m_maxChatLength > 0 ? m_maxChatLength : m_chats.size();
    m_offset = std::min(m_offset, m_chats.size());

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
            label->SetCharacterSize(GetCharacterSize());

            if (i == 0)
            {
                // TODO: Configurable spacing
                SetVerticalRepeat(max, label->GetCharacterSize() + 5.5f);
                SetHorizontalRepeat(1, 0);
            }

            label->SetPosition(GetNextItemPosition());
            m_labels.push_back(std::move(label));

            IncreaseSpacingCounter();
        }
    }

    for (size_t i = m_offset; i < m_offset + max; i++)
    {
        int index = i - m_offset;
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

        if (chat.Player.Administrator)
            m_labels[index]->SetColor(sf::Color(200, 155, 55));
        else
            m_labels[index]->SetColor(sf::Color::White);

        // TODO: Configurable outline
        m_labels[index]->SetOutlineThickness(0.10f);
        m_labels[index]->SetOutlineColor(m_labels[index]->GetColor());
        if (chat.Player.PlayerID != 0)
        {
            size_t nickLength = 16;
            auto nickname = chat.Player.Name;
            if (nickname.getSize() < nickLength)
            {
                for (size_t j = 0; j < nickLength - nickname.getSize(); j++)
                    nickname = " " + nickname;
            }

            m_labels[index]->SetString("[" + nickname + "] " + chat.Message);
        }
        else
            m_labels[index]->SetString(chat.Message);
    }
}
