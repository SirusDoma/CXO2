#ifndef O2JAM_COMMON_CHAT_WINDOW_HPP
#define O2JAM_COMMON_CHAT_WINDOW_HPP

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/ScrollBar.hpp>

#include <OTwo/Models/Room.hpp>

#include <vector>
#include <memory>

class ChatWindow : public Gx::List
{
public:
    ChatWindow();
    ChatWindow(ChatWindow &&other) noexcept;
    ChatWindow(const Gx::Font &font, const sf::FloatRect &localBounds, unsigned int characterSize = 13);

    sf::FloatRect GetLocalBounds() const override;
    void SetLocalBounds(const sf::FloatRect &bounds);

    void SetFont(const Gx::Font &font);
    void SetCharacterSize(unsigned int characterSize);

    const Gx::Font *GetFont() const;
    unsigned int GetScrollOffset() const;
    unsigned int GetCharacterSize() const;
    unsigned int GetMaximumChatLength() const;

    void SetScrollBar(Gx::ScrollBar &scrollBar);
    void SetScrollOffset(unsigned int offset);
    void SetMaximumChatLength(unsigned int maxLength);

    void PushMessage(const Player &player, const sf::String &chat);
    void PushSystemMessage(const sf::String &chat);

private:
    Gx::RenderStates Render(Gx::RenderSurface &surface, Gx::RenderStates states) const override;
    void OnMouseWheelScrolled(sf::Event::MouseWheelScrollEvent ev) override;
    void Invalidate() override;

    const Gx::Font *m_font;
    Gx::ScrollBar *m_scroll;

    sf::FloatRect m_bounds;
    unsigned int m_offset, m_maxChatLength, m_characterSize;

    std::vector<ChatMessage> m_chats;
    std::vector<std::unique_ptr<Gx::Label>> m_labels;
};

#endif
