#include <Genode/Graphics/Font.hpp>
#include <Genode/UI/InputField.hpp>
#include <Genode/Utilities/StringHelper.hpp>

#include <SFML/Window/Clipboard.hpp>

namespace Gx
{
    InputField::InputField() :
        m_text(),
        m_caret(*this),
        m_bounds(),
        m_maxLength(),
        m_permanentFocus(false),
        m_focused(),
        m_numeric(false),
        m_state(State::Normal)
    {
    }

    InputField::InputField(const Font& font, const sf::String& string, const unsigned int characterSize, sf::FloatRect bounds) :
        m_text(font, string, characterSize),
        m_caret(*this),
        m_bounds(bounds),
        m_maxLength(0),
        m_permanentFocus(false),
        m_focused(),
        m_numeric(false),
        m_state(State::Normal)
    {
        if (m_bounds == sf::FloatRect())
            m_bounds = m_text.GetLocalBounds();

        InputField::SetHighlightBackColor(sf::Color::White);
        SetHighlightTextColor(sf::Color::Black);
    }

    sf::FloatRect InputField::GetLocalBounds() const
    {
        return m_bounds;
    }

    void InputField::SetLocalBounds(const sf::FloatRect bounds)
    {
        m_bounds = bounds;
    }

    sf::Vector2f InputField::FindCharacterPosition(const std::size_t index) const
    {
        return m_text.FindCharacterPosition(index);
    }

    void InputField::SetString(const sf::String& string)
    {
        m_text.SetString(string);
    }

    void InputField::SetFont(const Font& font)
    {
        m_text.SetFont(font);
    }

    void InputField::SetMasked(const bool masked)
    {
        m_text.SetMasked(masked);
    }

    void InputField::SetNumericModeEnabled(const bool enabled)
    {
        if (m_numeric != enabled)
        {
            m_numeric = enabled;
            if (m_numeric)
                SetString("");
        }
    }

    void InputField::SetCharacterSize(const unsigned int size)
    {
        m_text.SetCharacterSize(size);
    }

    void InputField::SetLineSpacing(const float spacingFactor)
    {
        m_text.SetLetterSpacing(spacingFactor);
    }

    void InputField::SetLetterSpacing(const float spacingFactor)
    {
        m_text.SetLetterSpacing(spacingFactor);
    }

    void InputField::SetStyle(const std::uint32_t style)
    {
        m_text.SetStyle(style);
    }

    void InputField::SetColor(const sf::Color& color)
    {
        m_text.SetColor(color);
    }

    void InputField::SetHighlightBackColor(const sf::Color& color)
    {
        m_caret.SetHighlightColor(color);
    }

    void InputField::SetHighlightTextColor(const sf::Color& color)
    {
        m_highlightColor = color;
    }

    void InputField::SetOutlineColor(const sf::Color& color)
    {
        m_text.SetOutlineColor(color);
    }

    void InputField::SetOutlineThickness(const float thickness)
    {
        m_text.SetOutlineThickness(thickness);
    }

    const sf::String& InputField::GetString() const
    {
        return m_text.GetString();
    }

    const Font* InputField::GetFont() const
    {
        return m_text.GetFont();
    }

    bool InputField::IsMasked() const
    {
        return m_text.IsMasked();
    }

    bool InputField::IsNumericMode() const
    {
        return m_numeric;
    }

    unsigned int InputField::GetCharacterSize() const
    {
        return m_text.GetCharacterSize();
    }

    float InputField::GetLetterSpacing() const
    {
        return m_text.GetLetterSpacing();
    }

    float InputField::GetLineSpacing() const
    {
        return m_text.GetLineSpacing();
    }

    std::uint32_t InputField::GetStyle() const
    {
        return m_text.GetStyle();
    }

    const sf::Color& InputField::GetColor() const
    {
        return m_text.GetColor();
    }

    const sf::Color& InputField::GetHighlightBackColor() const
    {
        return m_caret.GetHighlight().GetColor();
    }

    const sf::Color& InputField::GetHighlightTextColor() const
    {
        return m_highlightColor;
    }

    const sf::Color& InputField::GetOutlineColor() const
    {
        return m_text.GetOutlineColor();
    }

    float InputField::GetOutlineThickness() const
    {
        return m_text.GetOutlineThickness();
    }

    unsigned int InputField::GetMaximumTextLength() const
    {
        return m_maxLength;
    }

    void InputField::SetMaximumTextLength(const unsigned int maxLength)
    {
        m_maxLength = maxLength;
    }

    void InputField::SetTextEnteredCallback(std::function<void(InputField&, const sf::String&)> callback)
    {
        m_onTextEntered = std::move(callback);
    }

    bool InputField::IsNextCharacterFit()
    {
        const auto string = m_text.GetString();
        auto index  = m_caret.Index;

        if (m_caret.SelectionLength != 0)
            index = static_cast<int>(Erase(index - 1, m_caret.SelectionLength));

        auto newString = m_text.GetString();
        newString.insert(index, " ");
        m_text.SetString(newString);
        const bool fit = m_text.GetLocalBounds().size.x <= m_bounds.size.x;

        m_text.SetString(string);
        return fit;
    }

    void InputField::Select(const std::size_t index, const int selectionLength)
    {
        m_caret.Index = static_cast<int>(index);
        m_caret.SelectionLength = selectionLength;
        SetFocus(true);

        Invalidate();
    }

    void InputField::SelectAll()
    {
        const int length = static_cast<int>(GetString().getSize());
        Select(length, -length);
    }

    sf::String InputField::GetSelectedText() const
    {
        auto index  = m_caret.Index - 1;
        const auto length = m_caret.SelectionLength;
        if (length < 0)
            index += length + 1;
        else if (length > 0)
            index++;
        else
            return {};

        return m_text.GetString().substring(index, std::abs(length));
    }

    size_t InputField::Insert(size_t index, const std::uint32_t unicode, const int selectionLength)
    {
        // backspace, tab, enter, etc
        if (unicode <= 31)
            return index;

        // Max length validation
        if (m_maxLength > 0 && selectionLength == 0 && m_text.GetString().getSize() >= m_maxLength)
            return index;

        // Max visual bounds validation
        if (IsNextCharacterFit())
        {
            if (m_caret.SelectionLength != 0)
                index = Erase(index - 1, selectionLength);

            auto string = m_text.GetString();
            string.insert(index, sf::String(static_cast<char32_t>(unicode)));
            m_text.SetString(string);

            return ++index;
        }

        return index;
    }

    std::size_t InputField::Erase(std::size_t index, const int length)
    {
        if (length < 0)
            index += length + 1;
        else if (length > 0)
            index++;
        else
            return index;

        auto str = m_text.GetString();
        str.erase(index, std::abs(length));
        m_text.SetString(str);

        return index;
    }

    bool InputField::IsPermanentFocus() const
    {
        return m_permanentFocus;
    }

    void InputField::SetPermanentFocusEnabled(const bool enable)
    {
        if (m_permanentFocus != enable)
        {
            m_permanentFocus = enable;
            if (m_permanentFocus)
                SetFocus(true);
        }
    }

    bool InputField::IsFocused() const
    {
        if (m_permanentFocus)
            return true;

        return m_focused;
    }

    void InputField::SetFocus(bool focus)
    {
        if (m_permanentFocus)
            focus = true;

        if (m_focused != focus)
        {
            m_focused = focus;
            auto uiEvent = Event{false, GetControlState()};
            if (GetFocusChangedCallback())
                GetFocusChangedCallback()(*this, uiEvent);

            if (m_focused && GetGainFocusCallback())
                GetGainFocusCallback()(*this, uiEvent);
            else if (!m_focused && GetLostFocusCallback())
                GetLostFocusCallback()(*this, uiEvent);

            SetControlState(uiEvent.State);
            if (!m_focused)
                m_caret.SelectionLength = 0;

            Invalidate();
        }
    }

    Control::State InputField::GetControlState() const
    {
        return m_state;
    }

    void InputField::SetControlState(const State& state)
    {
        if (m_state != state)
        {
            m_state = state;
            if (IsEnabled())
                OnControlStateChanged(*this, m_state);
        }
    }

    void InputField::Update(const double delta)
    {
        m_caret.Update(delta);
        Control::Update(delta);
    }

    RenderStates InputField::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states.transform *= GetTransform();
        if (m_caret.SelectionLength != 0)
            surface.Render(m_caret.GetHighlight(), states);

        surface.Render(m_text, states);
        if (IsFocused() && IsEnabled())
            surface.Render(m_caret, states);

        return Control::Render(surface, states);
    }

    void InputField::OnControlStateChanged(Control& sender, State state)
    {
    }

    void InputField::OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev)
    {
        float minDistance  = -1;
        size_t selectIndex = m_caret.Index;

        const auto bounds = GetGlobalBounds();
        for (size_t index = 0; index <= m_text.GetString().getSize(); index++)
        {
            const float distance = std::abs((FindCharacterPosition(index).x + bounds.position.x) - static_cast<float>(ev.position.x));
            if (minDistance == -1 || distance < minDistance)
            {
                selectIndex = index;
                minDistance = distance;
            }
            else if (distance > minDistance)
                break;
        }

        Select(selectIndex, 0);
        Control::OnControlClick(sender, ev);
    }

    void InputField::OnMouseMoved(const sf::Event::MouseMoved& ev)
    {
        Control::OnMouseMoved(ev);
    }

    void InputField::OnMouseButtonPressed(const sf::Event::MouseButtonPressed& ev)
    {
        Control::OnMouseButtonPressed(ev);
    }

    void InputField::OnMouseButtonReleased(const sf::Event::MouseButtonReleased& ev)
    {
        Control::OnMouseButtonReleased(ev);

        if (GetControlState() == State::Normal)
        {
            SetFocus(false);
            Invalidate();
        }
    }

    void InputField::OnKeyPressed(const sf::Event::KeyPressed& ev)
    {
        if (!IsEnabled() || !IsFocused())
            return;

        bool control = ev.control;
#ifdef __APPLE__
        control = ev.system;
#endif

        if (ev.code == sf::Keyboard::Key::Backspace || (ev.code == sf::Keyboard::Key::Delete && m_caret.SelectionLength != 0))
        {
            if (m_caret.Index == 0 && m_caret.SelectionLength == 0)
                return;

            const int length    = m_caret.SelectionLength;
            m_caret.Index = static_cast<int>(Erase(m_caret.Index - 1, length == 0 ? -1 : length));
            m_caret.SelectionLength = 0;
        }
        else if (ev.code == sf::Keyboard::Key::Delete)
        {
            if (m_caret.Index >= m_text.GetString().getSize())
                return;

            auto str = m_text.GetString();
            str.erase(m_caret.Index);

            m_text.SetString(str);
            m_caret.SelectionLength = 0;
        }
        else if (ev.code == sf::Keyboard::Key::Enter)
        {
            // Trim front and back string from whitespaces
            const sf::String string = StringHelper::Trim(m_text.GetString());
            if (!string.isEmpty() && m_onTextEntered)
                m_onTextEntered(*this, string);

            m_text.SetString("");
            m_caret.SelectionLength = 0;
        }
        else if (control || ev.shift)
        {
            if (ev.shift)
            {
                if (ev.code == sf::Keyboard::Key::Left)
                {
                    if (m_caret.Index <= 0)
                        return;

                    m_caret.Index--;
                    m_caret.SelectionLength++;
                }
                else if (ev.code == sf::Keyboard::Key::Right)
                {
                    if (m_caret.Index >= m_text.GetString().getSize())
                        return;

                    m_caret.Index++;
                    m_caret.SelectionLength--;
                }
            }

            if (control)
            {
                if (ev.code == sf::Keyboard::Key::C || ev.code == sf::Keyboard::Key::X)
                {
                    sf::Clipboard::setString(GetSelectedText());
                    if (ev.code == sf::Keyboard::Key::X)
                    {
                        m_caret.Index = static_cast<int>(Erase(m_caret.Index - 1, m_caret.SelectionLength));
                        m_caret.SelectionLength = 0;
                    }
                }
                else if (ev.code == sf::Keyboard::Key::V)
                {
                    std::string input = sf::Clipboard::getString();
                    auto string = sf::String::fromUtf8(input.begin(), input.end());
                    for (size_t index = 0; index < string.getSize(); index++)
                        m_caret.Index = static_cast<int>(Insert(m_caret.Index, string[index]));
                }
            }
        }
        else if (ev.code == sf::Keyboard::Key::Left)
        {
            m_caret.Index--;
            m_caret.SelectionLength = 0;
            m_text.SetColor(m_text.GetColor());
        }
        else if (ev.code == sf::Keyboard::Key::Right)
        {
            m_caret.Index++;
            m_caret.SelectionLength = 0;
            m_text.SetColor(m_text.GetColor());
        }
        else
            return;

        Invalidate();
    }

    void InputField::OnTextEntered(const sf::Event::TextEntered& ev)
    {
        if (!IsEnabled() || !IsFocused())
            return;

        // backspace, tab, enter, etc
        if (ev.unicode <= 31)
            return;

#ifdef __APPLE__
        // Delete key
        if (ev.unicode == 127)
            return;
#endif

        // Only accept numbers
        if (m_numeric && (ev.unicode < 48 || ev.unicode > 57))
            return;

        m_caret.Index = static_cast<int>(Insert(m_caret.Index, ev.unicode, m_caret.SelectionLength));
        m_caret.SelectionLength = 0;

        Invalidate();
    }

    void InputField::Invalidate()
    {
        m_caret.Invalidate();
        m_caret.Reset(true);

        size_t start = m_caret.Index;
        if (m_caret.SelectionLength < 0)
            start += m_caret.SelectionLength;

        m_text.SetColor(m_text.GetColor());
        if (m_caret.SelectionLength != 0)
        {
            for (size_t index = 0; index < m_text.GetString().getSize(); index++)
            {
                if (index >= start && index < start + std::abs(m_caret.SelectionLength))
                    m_text.SetColor(m_highlightColor, index);
            }
        }
    }
}

namespace Gx
{
    InputField::Caret::Caret(InputField& instance) :
        Instance(instance),
        Index(),
        SelectionLength(),
        m_cursor(),
        m_highlight(),
        m_visible(true),
        m_elapsed()
    {
        SetHighlightColor(sf::Color::Transparent);
        Invalidate();
    }

    void InputField::Caret::Reset(const bool visible)
    {
        m_elapsed = 0;
        m_visible = visible;
    }

    const Rectangle& InputField::Caret::GetHighlight() const
    {
        return m_highlight;
    }

    void InputField::Caret::SetHighlightColor(const sf::Color color)
    {
        m_highlight.SetColor(color);
    }

    RenderStates InputField::Caret::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!m_visible)
            return states;

        surface.Render(m_cursor, states);
        return states;
    }

    void InputField::Caret::Update(const double delta)
    {
        m_elapsed += delta;
        if (m_elapsed >= BLINK_THRESHOLD)
        {
            m_elapsed = 0;
            m_visible = !m_visible;
        }
    }

    void InputField::Caret::Invalidate()
    {
        if (Index < 0)
            Index = 0;

        if (Index > Instance.GetString().getSize())
            Index = static_cast<int>(Instance.GetString().getSize());

        if (Instance.GetFont())
        {
            const auto glyph = Instance.GetFont()->GetGlyph('|', 0, Instance.GetCharacterSize(), false);
            m_cursor.SetSize(sf::Vector2f(glyph.bounds.size.x * 0.65f, static_cast<float>(Instance.GetCharacterSize())));
        }

        m_cursor.SetPosition(Instance.FindCharacterPosition(Index) + sf::Vector2f(0.f, 1.5f));
        m_cursor.SetColor(Instance.GetColor());

        if (SelectionLength != 0)
        {
            size_t index = Index - 1;
            const int length   = SelectionLength;
            if (length < 0)
                index += length + 1;
            else if (length > 0)
                index++;

            const auto charPos = Instance.FindCharacterPosition(index);
            const auto endPos  = Instance.FindCharacterPosition(index + std::abs(length));
            m_highlight.SetPosition(sf::Vector2f(charPos.x, m_cursor.GetPosition().y));
            m_highlight.SetSize(sf::Vector2f(std::abs(charPos.x - endPos.x), static_cast<float>(Instance.GetCharacterSize())));
        }
        else
            m_highlight.SetSize(sf::Vector2f());
    }
}