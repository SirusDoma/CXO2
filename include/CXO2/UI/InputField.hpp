#pragma once

#include <CXO2/UI/Control.hpp>

#include <Genode/Graphics/Shapes/Rectangle.hpp>
#include <Genode/Graphics/Text.hpp>
#include <CXO2/UI/Label.hpp>

#include <vector>

namespace Cx
{
    class InputField : public Control, public virtual Gx::Colorable
    {
    public:
        struct TextEnteredEvent : Event
        {
            sf::String Text;
        };

        InputField();
        InputField(const Gx::Font& font, const sf::String& string, unsigned int characterSize = 30, const sf::FloatRect& bounds = sf::FloatRect());
        InputField(Gx::Font&& font, const sf::String& string, unsigned int characterSize = 30, sf::FloatRect bounds = sf::FloatRect()) = delete;

        [[nodiscard]] sf::Vector2f FindCharacterPosition(std::size_t index) const;
        [[nodiscard]] sf::FloatRect GetLocalBounds() const override;
        virtual void SetLocalBounds(sf::FloatRect bounds);

        void SetString(const sf::String& string);
        void SetFont(const Gx::Font& font);
        void AddFallbackFont(const Gx::Font& font) const;
        void SetMasked(bool masked);
        void SetNumericModeEnabled(bool enabled);

        void SetCharacterSize(unsigned int size);
        void SetLineSpacing(float spacingFactor);
        void SetLetterSpacing(float spacingFactor);
        void SetStyle(std::uint32_t style);

        void SetColor(const sf::Color& color) override;
        void SetHighlightBackColor(const sf::Color& color);
        void SetHighlightTextColor(const sf::Color& color);
        void SetOutlineColor(const sf::Color& color);
        void SetOutlineThickness(float thickness);
        void SetOutlineOffset(const sf::Vector2f& offset);

        [[nodiscard]] const sf::String& GetString() const;
        [[nodiscard]] const Gx::Font* GetFont() const;
        [[nodiscard]] bool IsMasked() const;
        [[nodiscard]] bool IsNumericMode() const;

        [[nodiscard]] unsigned int GetCharacterSize() const;
        [[nodiscard]] float GetLetterSpacing() const;
        [[nodiscard]] float GetLineSpacing() const;
        [[nodiscard]] std::uint32_t GetStyle() const;

        [[nodiscard]] const sf::Color& GetColor() const override;
        [[nodiscard]] const sf::Color& GetHighlightBackColor() const;
        [[nodiscard]] const sf::Color& GetHighlightTextColor() const;
        [[nodiscard]] const sf::Color& GetOutlineColor() const;
        [[nodiscard]] float GetOutlineThickness() const;

        [[nodiscard]] bool IsPermanentFocus() const;
        void SetPermanentFocusEnabled(bool enable);

        [[nodiscard]] bool IsFocused() const override;
        void SetFocus(bool focus) override;

        [[nodiscard]] unsigned int GetMaximumTextLength() const;
        void SetMaximumTextLength(unsigned int maxLength);
        void SetTextEnteredCallback(std::function<void(InputField&, TextEnteredEvent&)> callback);

        void Select(size_t index, int selectionLength);
        void SelectAll();
        [[nodiscard]] sf::String GetSelectedText() const;

        size_t Insert(size_t index, std::uint32_t unicode, int selectionLength = 0);
        size_t Erase(size_t index, int length);

    private:
        [[nodiscard]] std::vector<sf::Vector2f> ComputeCursorPositions() const;

        void SetControlState(const State& state) override;
        [[nodiscard]] State GetControlState() const override;

        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void OnControlStateChanged(Control& sender, State state) override;
        void OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev) override;
        void OnMouseMoved(const sf::Event::MouseMoved& ev) override;
        void OnMouseButtonPressed(const sf::Event::MouseButtonPressed& ev) override;
        void OnMouseButtonReleased(const sf::Event::MouseButtonReleased& ev) override;
        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;
        void OnTextEntered(const sf::Event::TextEntered& ev) override;

        void Invalidate() override;

        class Caret : public Gx::Renderable, public Gx::Updatable
        {
        public:
            InputField& Instance;
            int Index;
            int SelectionLength;

            explicit Caret(InputField& instance);
            void Reset(bool visible = false);
            [[nodiscard]] const Gx::Rectangle& GetHighlight() const;
            void SetHighlightColor(sf::Color color);

            void Update(const sf::Time& delta) override;
            Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

            void Invalidate();

        private:
            const double BLINK_THRESHOLD = 500.f;

            Gx::Rectangle m_cursor, m_highlight;
            bool      m_visible;
            double    m_elapsed;
        };

        Label m_label;
        Caret m_caret;
        sf::Color m_highlightColor;
        sf::FloatRect m_bounds;
        mutable float m_scroll{0.f};
        unsigned int m_maxLength;
        bool m_permanentFocus;
        bool m_focused;
        bool m_numeric;
        Control::State m_state;

        std::function<void(InputField&, TextEnteredEvent&)> m_onTextEntered;
    };
}
