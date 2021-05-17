#ifndef O2JAM_COMMON_CHAT_WINDOW_HPP
#define O2JAM_COMMON_CHAT_WINDOW_HPP

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/ScrollBar.hpp>

#include <O2/Data/Room.hpp>

#include <vector>
#include <memory>

class ChatWindow : public Gx::List
{
public:
    ChatWindow();
    ChatWindow(const sf::Font &font, sf::FloatRect localBounds, unsigned int characterSize = 13);

    virtual const sf::FloatRect GetLocalBounds() const;
    void SetLocalBounds(const sf::FloatRect &bounds);

    void SetFont(const sf::Font &font);
    void SetCharacterSize(unsigned int characterSize);

    const sf::Font *GetFont() const;
    unsigned int GetScrollOffset() const;
    unsigned int GetCharacterSize() const;
    unsigned int GetMaximumChatLength() const;

    void SetScrollOffset(unsigned int offset);
    void SetMaximumChatLength(unsigned int max);

    void PushMessage(Room::PlayerInfo player, sf::String chat);

private:
    virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
    virtual void OnMouseWheelScrolled(sf::Event::MouseWheelScrollEvent ev);
    virtual void Invalidate();

    const sf::Font *m_font;
    Gx::ScrollBar *m_scroll;

    sf::FloatRect m_bounds;
    unsigned int m_offset, m_maxChatLength, m_characterSize;

    std::vector<Room::ChatData> m_chats;
    std::vector<std::unique_ptr<Gx::Label>> m_labels;
};

#endif
