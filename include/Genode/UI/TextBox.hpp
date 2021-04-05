#ifndef GENODE_UI_TEXTBOX_HPP
#define GENODE_UI_TEXTBOX_HPP

#include <Genode/UI/Control.hpp>

#include <Genode/Graphics/Shapes/Rectangle.hpp>
#include <Genode/Graphics/Text.hpp>

namespace Gx
{
    class TextBox : public Control
    {
    public:
        TextBox();
        TextBox(const sf::String& string, const sf::Font& font, unsigned int characterSize = 30, sf::FloatRect bounds = sf::FloatRect());

        sf::Vector2f FindCharacterPosition(std::size_t index) const;
        virtual const sf::FloatRect GetLocalBounds() const;
        virtual void SetLocalBounds(sf::FloatRect bounds);

        void SetString(const sf::String& string);
        void SetFont(const sf::Font& font);

        void SetCharacterSize(unsigned int size);
        void SetLineSpacing(float spacingFactor);
        void SetLetterSpacing(float spacingFactor);
        void SetStyle(Uint32 style);

        virtual void SetColor(const sf::Color& color);
        void SetFillColor(const sf::Color& color);
        virtual void SetHighlightBackColor(const sf::Color& color);
        void SetHighlightTextColor(const sf::Color& color);
        void SetOutlineColor(const sf::Color& color);
        void SetOutlineThickness(float thickness);

        const sf::String& GetString() const;
        const sf::Font* GetFont() const;

        unsigned int GetCharacterSize() const;
        float GetLetterSpacing() const;
        float GetLineSpacing() const;
        Uint32 GetStyle() const;

        virtual const sf::Color& GetColor() const;
        const sf::Color& GetFillColor() const;
        virtual const sf::Color& GetHighlightBackColor() const;
        const sf::Color& GetHighlightTextColor() const;
        const sf::Color& GetOutlineColor() const;
        float GetOutlineThickness() const;

        unsigned int GetMaximumTextLength() const;
        void SetMaximumTextLength(unsigned int maxLength);

        void SetTextEnteredCallback(std::function<void(TextBox&, sf::String)> callback);

    protected:
        virtual void Update(double delta);
        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

        virtual void OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev);
        virtual void OnMouseMove(sf::Event::MouseMoveEvent ev);
        virtual void OnMouseButtonUp(sf::Event::MouseButtonEvent ev);
        virtual void OnKeyDown(sf::Event::KeyEvent ev);
        virtual void OnKeyType(sf::Event::TextEvent ev);

        virtual void Invalidate();

    private:
        class Caret : public Renderable, public Updatable
        {
        public:
            TextBox& Instance;
            int Index;
            int SelectionLength;

            Caret(TextBox &instance);
            void Reset(bool visible = false);
            const Rectangle &GetHighlight() const;
            void SetHighlightColor(sf::Color color);

            virtual void Update(double delta);
            virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;

            void Invalidate();

        private:
            const double BLINK_THRESHOLD = 500.f;

            Rectangle m_cursor, m_highlight;
            bool      m_visible;
            double    m_elapsed;
        };

        Text  m_text;
        Caret m_caret;
        sf::Color m_highlightColor;
        sf::FloatRect m_bounds;
        unsigned int m_maxLength;

        std::function<void(TextBox&, sf::String)> m_onTextEntered;
    };
}

#endif
