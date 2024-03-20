#ifndef GENODE_UI_TEXTBOX_HPP
#define GENODE_UI_TEXTBOX_HPP

#include <Genode/UI/Control.hpp>

#include <Genode/Graphics/Shapes/Rectangle.hpp>
#include <Genode/Graphics/Text.hpp>

namespace Gx
{
    class TextBox : public Control, public virtual Colorable
    {
    public:
        TextBox();
        TextBox(const sf::String& string, const sf::Font& font, unsigned int characterSize = 30, sf::FloatRect bounds = sf::FloatRect());

        sf::Vector2f FindCharacterPosition(std::size_t index) const;
        sf::FloatRect GetLocalBounds() const override;
        virtual void SetLocalBounds(sf::FloatRect bounds);

        void SetString(const sf::String& string);
        void SetFont(const sf::Font& font);
        void SetMasked(bool masked);
        void SetNumericModeEnabled(bool enabled);

        void SetCharacterSize(unsigned int size);
        void SetLineSpacing(float spacingFactor);
        void SetLetterSpacing(float spacingFactor);
        void SetStyle(Uint32 style);

        void SetColor(const sf::Color& color) override;
        void SetFillColor(const sf::Color& color);
        virtual void SetHighlightBackColor(const sf::Color& color);
        void SetHighlightTextColor(const sf::Color& color);
        void SetOutlineColor(const sf::Color& color);
        void SetOutlineThickness(float thickness);

        const sf::String& GetString() const;
        const sf::Font* GetFont() const;
        bool IsMasked() const;
        bool IsNumericMode() const;

        unsigned int GetCharacterSize() const;
        float GetLetterSpacing() const;
        float GetLineSpacing() const;
        Uint32 GetStyle() const;

        const sf::Color& GetColor() const override;
        const sf::Color& GetFillColor() const;
        virtual const sf::Color& GetHighlightBackColor() const;
        const sf::Color& GetHighlightTextColor() const;
        const sf::Color& GetOutlineColor() const;
        float GetOutlineThickness() const;

        bool IsPermanentFocus() const;
        void SetPermanentFocusEnabled(bool enable);

        bool IsFocused() const override;
        void SetFocus(bool focus) override;

        unsigned int GetMaximumTextLength() const;
        void SetMaximumTextLength(unsigned int maxLength);
        void SetTextEnteredCallback(std::function<void(TextBox&, sf::String)> callback);

        void Select(size_t index, int selectionLength);
        void SelectAll();
        sf::String GetSelectedText() const;

        size_t Insert(size_t index, Uint32 unicode, int selectionLength = 0);
        size_t Erase(size_t index, int length);

    private:
        bool IsNextCharacterFit();

        void SetControlState(const State &state) override;
        const State GetControlState() const override;

        void Update(double delta) override;
        sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const override;

        void OnControlStateChanged(Control *sender, State state) override;
        void OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev) override;
        void OnMouseMove(sf::Event::MouseMoveEvent ev) override;
        void OnMouseButtonDown(sf::Event::MouseButtonEvent ev) override;
        void OnMouseButtonUp(sf::Event::MouseButtonEvent ev) override;
        void OnKeyDown(sf::Event::KeyEvent ev) override;
        void OnKeyType(sf::Event::TextEvent ev) override;

        void Invalidate() override;

        class Caret : public Renderable, public Updatable
        {
        public:
            TextBox& Instance;
            int Index;
            int SelectionLength;

            explicit Caret(TextBox &instance);
            void Reset(bool visible = false);
            const Rectangle &GetHighlight() const;
            void SetHighlightColor(sf::Color color);

            void Update(double delta) override;
            sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const override;

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
        bool m_permanentFocus;
        bool m_focused;
        bool m_numeric;
        Control::State m_state;

        std::function<void(TextBox&, sf::String)> m_onTextEntered;
    };
}

#endif
