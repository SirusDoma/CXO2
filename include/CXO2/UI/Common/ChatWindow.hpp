#pragma once

#include <CXO2/Models/Messaging.hpp>

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/List.hpp>
#include <Genode/UI/Label.hpp>
#include <Genode/UI/ScrollBar.hpp>

#include <vector>
#include <memory>

namespace Cx
{
    class SessionContext;
    class ChatWindow : public Gx::List
    {
    public:
        explicit ChatWindow(SessionContext& session);

        sf::FloatRect GetGlobalBounds() const override;
        sf::FloatRect GetLocalBounds() const override;
        void SetLocalBounds(const sf::FloatRect& bounds) override;

        void SetFont(const Gx::Font& font);
        void SetCharacterSize(unsigned int characterSize);
        void AddFallbackFont(const Gx::Font& font);

        const Gx::Font* GetFont() const;
        sf::Color GetTextColor() const;
        unsigned int GetScrollOffset() const;
        unsigned int GetCharacterSize() const;
        unsigned int GetMaximumChatLength() const;
        float GetLineSpacing() const;

        void SetTextColor(const sf::Color& textColor);
        void SetScrollBar(Gx::ScrollBar& scrollBar);
        void SetScrollOffset(unsigned int offset);
        void SetMaximumChatLength(unsigned int maxLength);
        void SetLineSpacing(float lineSpacing);

        void PushMessage(const CharacterInfo& sender, const sf::String& chat);
        void PushWhisper(const CharacterInfo& sender, const CharacterInfo& recepient, const sf::String& chat);
        void PushSystemMessage(const sf::String& chat);

    private:
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
        void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& ev) override;
        void Invalidate() override;

        const Gx::Font* m_font;
        sf::Color m_textColor;
        Gx::ScrollBar* m_scroll;

        sf::FloatRect m_bounds;
        unsigned int m_offset, m_maxChatLength, m_characterSize;
        float m_lineSpacing;

        std::vector<ChatMessage> m_chats;
        std::vector<const Gx::Font*> m_fallbackFonts{};
        std::vector<std::unique_ptr<Gx::Label>> m_labels;

        SessionContext& m_session;
    };
}
