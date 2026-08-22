#include <Genode/Graphics/Font.hpp>
#include <CXO2/UI/InputField.hpp>
#include <Genode/System/Platform.hpp>
#include <Genode/Utilities/StringHelper.hpp>

#include <SFML/Window/Clipboard.hpp>

#include <algorithm>

namespace
{
    bool IsSeparator(const std::uint32_t ch)
    {
        if (ch == U' ' || ch == U'\t')
            return true;

        static constexpr std::u32string_view separators = U".,;:!?()[]{}/\\-_@#%&*";
        return separators.find(static_cast<char32_t>(ch)) != std::u32string_view::npos;
    }

    size_t FindWordStart(const sf::String& str, const size_t from)
    {
        size_t left = from;
        while (left > 0)
        {
            const auto ch = str[left - 1];
            if (IsSeparator(ch))
                left--;
            else
                break;
        }
        while (left > 0)
        {
            const auto ch = str[left - 1];
            if (!IsSeparator(ch))
                left--;
            else
                break;
        }
        return left;
    }

    size_t FindWordEnd(const sf::String& str, const size_t from)
    {
        size_t right = from;
        const size_t size = str.getSize();
        while (right < size)
        {
            const auto ch = str[right];
            if (IsSeparator(ch))
                right++;
            else
                break;
        }
        while (right < size)
        {
            const auto ch = str[right];
            if (!IsSeparator(ch))
                right++;
            else
                break;
        }
        return right;
    }
}

namespace Cx
{
    InputField::InputField() :
        m_label(),
        m_caret(*this),
        m_bounds(),
        m_maxLength(),
        m_permanentFocus(false),
        m_focused(),
        m_numeric(false),
        m_state(State::Normal)
    {
    }

    InputField::InputField(const Gx::Font& font, const sf::String& string, const unsigned int characterSize, const sf::FloatRect& bounds) :
        m_label(font, string, characterSize),
        m_caret(*this),
        m_bounds(bounds),
        m_maxLength(0),
        m_permanentFocus(false),
        m_focused(),
        m_numeric(false),
        m_state(State::Normal)
    {
        if (m_bounds == sf::FloatRect())
            m_bounds = m_label.GetLocalBounds();

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
        const auto positions = ComputeCursorPositions();
        return m_label.GetTransform().transformPoint(positions[std::min(index, positions.size() - 1)]);
    }

    std::vector<sf::Vector2f> InputField::ComputeCursorPositions() const
    {
        // Compute the position before each codepoint plus the past-the-end position
        // following the approach demonstrated in the SFML text example
        std::vector<sf::Vector2f> positions;

        const auto& string = m_label.GetString();
        positions.reserve(string.getSize() + 1);

        const auto    characterSize = static_cast<float>(m_label.GetCharacterSize());
        std::uint32_t previousCluster{};
        sf::Vector2f  previousPosition;
        float         previousAdvance{};
        auto          previousDirection = Gx::Text::TextDirection::Unspecified;

        for (const auto& glyph : m_label.GetShapedGlyphs())
        {
            // Combining marks can't be valid cursor positions so
            // we just skip over trailing glyphs that share the same
            // cluster value as the base glyph that precedes them
            if (!positions.empty() && glyph.cluster == previousCluster)
                continue;

            sf::Vector2f position{glyph.position.x, glyph.baseline - characterSize};

            // In right-to-left text, the "before grapheme position" is to the right of the glyph
            if (glyph.textDirection == Gx::Text::TextDirection::RightToLeft)
                position.x += glyph.glyph.bounds.size.x;

            // Interpolate positions of graphemes that were merged into a single glyph e.g. ligatures
            if (!positions.empty())
            {
                const auto subWidth = (position - previousPosition).x / static_cast<float>(glyph.cluster - previousCluster);
                while (glyph.cluster - previousCluster > 1)
                {
                    previousPosition.x += subWidth;
                    ++previousCluster;
                    positions.emplace_back(previousPosition);
                }
            }

            previousCluster   = glyph.cluster;
            previousPosition  = position;
            previousAdvance   = glyph.glyph.advance;
            previousDirection = glyph.textDirection;
            positions.emplace_back(position);
        }

        if (positions.empty())
        {
            positions.emplace_back(0.f, 0.f);
            return positions;
        }

        // Interpolate the remaining positions when the last glyph is itself a ligature
        // and add the past-the-end position based on the direction of the text
        const auto direction = (previousDirection == Gx::Text::TextDirection::RightToLeft) ? -1.f : 1.f;
        const auto subWidth  = direction * (previousAdvance + m_label.GetLetterSpacing()) /
                               static_cast<float>(string.getSize() - previousCluster);

        while (positions.size() < string.getSize() + 1)
        {
            previousPosition.x += subWidth;
            positions.emplace_back(previousPosition);
        }

        return positions;
    }

    void InputField::SetString(const sf::String& string)
    {
        m_label.SetString(string);
    }

    void InputField::SetFont(const Gx::Font& font)
    {
        m_label.SetFont(font);
    }

    void InputField::AddFallbackFont(const Gx::Font& font) const
    {
        m_label.AddFallbackFont(font);
    }

    void InputField::SetMasked(const bool masked)
    {
        m_label.SetMasked(masked);
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
        m_label.SetCharacterSize(size);
    }

    void InputField::SetLineSpacing(const float spacingFactor)
    {
        m_label.SetLetterSpacing(spacingFactor);
    }

    void InputField::SetLetterSpacing(const float spacingFactor)
    {
        m_label.SetLetterSpacing(spacingFactor);
    }

    void InputField::SetStyle(const std::uint32_t style)
    {
        m_label.SetStyle(style);
    }

    void InputField::SetColor(const sf::Color& color)
    {
        m_label.SetColor(color);
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
        m_label.SetOutlineColor(color);
    }

    void InputField::SetOutlineThickness(const float thickness)
    {
        m_label.SetOutlineThickness(thickness);
    }

    void InputField::SetOutlineOffset(const sf::Vector2f& offset)
    {
        m_label.SetOutlineOffset(offset);
    }

    const sf::String& InputField::GetString() const
    {
        return m_label.GetString();
    }

    const Gx::Font* InputField::GetFont() const
    {
        return m_label.GetFont();
    }

    bool InputField::IsMasked() const
    {
        return m_label.IsMasked();
    }

    bool InputField::IsNumericMode() const
    {
        return m_numeric;
    }

    unsigned int InputField::GetCharacterSize() const
    {
        return m_label.GetCharacterSize();
    }

    float InputField::GetLetterSpacing() const
    {
        return m_label.GetLetterSpacing();
    }

    float InputField::GetLineSpacing() const
    {
        return m_label.GetLineSpacing();
    }

    std::uint32_t InputField::GetStyle() const
    {
        return m_label.GetStyle();
    }

    const sf::Color& InputField::GetColor() const
    {
        return m_label.GetColor();
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
        return m_label.GetOutlineColor();
    }

    float InputField::GetOutlineThickness() const
    {
        return m_label.GetOutlineThickness();
    }

    unsigned int InputField::GetMaximumTextLength() const
    {
        return m_maxLength;
    }

    void InputField::SetMaximumTextLength(const unsigned int maxLength)
    {
        m_maxLength = maxLength;
    }

    void InputField::SetTextEnteredCallback(std::function<void(InputField&, TextEnteredEvent&)> callback)
    {
        m_onTextEntered = std::move(callback);
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

        return m_label.GetString().substring(index, std::abs(length));
    }

    size_t InputField::Insert(size_t index, const std::uint32_t unicode, const int selectionLength)
    {

        if (unicode <= 31 || // backspace, tab, enter, etc
            unicode == 127)  // ctrl+backspace
        {
            return index;
        }

        // Max length validation
        if (m_maxLength > 0 && selectionLength == 0 && m_label.GetString().getSize() >= m_maxLength)
            return index;

        if (m_caret.SelectionLength != 0)
            index = Erase(index - 1, selectionLength);

        auto string = m_label.GetString();
        string.insert(index, sf::String(static_cast<char32_t>(unicode)));
        m_label.SetString(string);

        return ++index;
    }

    std::size_t InputField::Erase(std::size_t index, const int length)
    {
        if (length < 0)
            index += length + 1;
        else if (length > 0)
            index++;
        else
            return index;

        auto str = m_label.GetString();
        str.erase(index, std::abs(length));
        m_label.SetString(str);

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

    void InputField::Update(const sf::Time& delta)
    {
        m_caret.Update(delta);
        Control::Update(delta);
    }

    Gx::RenderStates InputField::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (!IsVisible())
            return states;

        states.transform *= GetTransform();

        if (m_bounds.size.x > 0.f && m_bounds.size.y > 0.f)
        {
            const float boxLeft  = m_bounds.position.x;
            const float boxRight = boxLeft + m_bounds.size.x;
            const float caretX   = FindCharacterPosition(static_cast<std::size_t>(std::max(m_caret.Index, 0))).x;
            const float endX     = FindCharacterPosition(m_label.GetString().getSize()).x;

            m_scroll = std::clamp(m_scroll, 0.f, std::max(0.f, endX - boxRight));
            if (caretX - m_scroll > boxRight)
                m_scroll = caretX - boxRight;
            else if (caretX - m_scroll < boxLeft)
                m_scroll = caretX - boxLeft;

            m_label.ClipQuads(sf::FloatRect(
                m_bounds.position + sf::Vector2f(m_scroll, 0.f) - m_label.GetPosition(),
                m_bounds.size
            ));
        }
        else
            m_scroll = 0.f;

        auto content = states;
        content.transform.translate(sf::Vector2f(-m_scroll, 0.f));

        if (m_caret.SelectionLength != 0)
            surface.Render(m_caret.GetHighlight(), content);

        surface.Render(m_label, content);
        if (IsFocused() && IsEnabled())
            surface.Render(m_caret, content);

        return Control::Render(surface, states);
    }

    void InputField::OnControlStateChanged(Control& sender, State state)
    {
    }

    void InputField::OnControlClick(Control& sender, const sf::Event::MouseButtonReleased& ev)
    {
        float minDistance  = -1;
        size_t selectIndex = m_caret.Index;

        const auto bounds    = GetGlobalBounds();
        const auto positions = ComputeCursorPositions();
        for (size_t index = 0; index < positions.size(); index++)
        {
            const auto position  = m_label.GetTransform().transformPoint(positions[index]);
            const float distance = std::abs((position.x - m_scroll + bounds.position.x) - static_cast<float>(ev.position.x));
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
        if (Gx::GetCurrentPlatform() == Gx::Platform::macOS)
            control = ev.system;

        if (ev.code == sf::Keyboard::Key::Backspace || (ev.code == sf::Keyboard::Key::Delete && m_caret.SelectionLength != 0))
        {
            if (m_caret.Index == 0 && m_caret.SelectionLength == 0)
                return;

            int eraseLength = m_caret.SelectionLength == 0 ? -1 :  m_caret.SelectionLength;
            if (m_caret.SelectionLength == 0 && control)
            {
                if (!ev.shift)
                {
                    const size_t left = FindWordStart(m_label.GetString(), static_cast<size_t>(m_caret.Index));
                    eraseLength = -static_cast<int>(static_cast<size_t>(m_caret.Index) - left);
                }
                else
                {
                    eraseLength = -static_cast<int>(static_cast<size_t>(m_caret.Index));
                }
            }

            m_caret.Index = static_cast<int>(Erase(m_caret.Index - 1, eraseLength));
            m_caret.SelectionLength = 0;
        }
        else if (ev.code == sf::Keyboard::Key::Delete)
        {
            if (m_caret.Index >= m_label.GetString().getSize())
                return;

            if (control && ev.shift)
            {
                const size_t from = static_cast<size_t>(m_caret.Index);
                const size_t end  = m_label.GetString().getSize();
                const int delta   = static_cast<int>(end - from);
                if (delta <= 0)
                    return;

                m_caret.Index = static_cast<int>(Erase(m_caret.Index - 1, delta));
                m_caret.SelectionLength = 0;
            }
            else if (control)
            {
                const size_t from = static_cast<size_t>(m_caret.Index);
                const size_t right = FindWordEnd(m_label.GetString(), from);
                const int delta = static_cast<int>(right - from);
                if (delta <= 0)
                    return;

                m_caret.Index = static_cast<int>(Erase(m_caret.Index - 1, delta));
                m_caret.SelectionLength = 0;
            }
            else
            {
                auto str = m_label.GetString();
                str.erase(m_caret.Index);

                m_label.SetString(str);
                m_caret.SelectionLength = 0;
            }
        }
        else if (ev.code == sf::Keyboard::Key::Enter)
        {
            // Trim front and back string from whitespaces
            const sf::String string = Gx::StringHelper::Trim(m_label.GetString());
            if (!string.isEmpty() && m_onTextEntered)
            {
                auto textEv = TextEnteredEvent{{false, GetControlState()}, string};
                m_onTextEntered(*this, textEv);
            }

            m_label.SetString("");
            m_caret.SelectionLength = 0;
        }
        else if (control || ev.shift)
        {
            if (ev.shift)
            {
                if (control && ev.code == sf::Keyboard::Key::Left)
                {
                    if (m_caret.Index <= 0)
                        return;

                    const size_t from = static_cast<size_t>(m_caret.Index);
                    const size_t left = FindWordStart(m_label.GetString(), from);
                    const int delta = static_cast<int>(from - left);

                    if (delta <= 0)
                        return;

                    m_caret.Index -= delta;
                    m_caret.SelectionLength += delta;
                }
                else if (control && ev.code == sf::Keyboard::Key::Right)
                {
                    if (m_caret.Index >= m_label.GetString().getSize())
                        return;

                    const size_t from = static_cast<size_t>(m_caret.Index);
                    const size_t right = FindWordEnd(m_label.GetString(), from);
                    const int delta = static_cast<int>(right - from);

                    if (delta <= 0)
                        return;

                    m_caret.Index += delta;
                    m_caret.SelectionLength -= delta;
                }
                else if (ev.code == sf::Keyboard::Key::Left)
                {
                    if (m_caret.Index <= 0)
                        return;

                    m_caret.Index--;
                    m_caret.SelectionLength++;
                }
                else if (ev.code == sf::Keyboard::Key::Right)
                {
                    if (m_caret.Index >= m_label.GetString().getSize())
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
                    if (m_caret.SelectionLength != 0)
                    {
                        m_caret.Index = static_cast<int>(Erase(m_caret.Index - 1, m_caret.SelectionLength));
                        m_caret.SelectionLength = 0;
                    }

                    auto string = sf::Clipboard::getString();
                    for (size_t index = 0; index < string.getSize(); index++)
                        m_caret.Index = static_cast<int>(Insert(m_caret.Index, string[index]));
                }
                else if (ev.code == sf::Keyboard::Key::A)
                {
                    SelectAll();
                }
                else if (!ev.shift && ev.code == sf::Keyboard::Key::Left)
                {
                    const size_t left = FindWordStart(m_label.GetString(), static_cast<size_t>(m_caret.Index));

                    m_caret.Index = static_cast<int>(left);
                    m_caret.SelectionLength = 0;
                }
                else if (!ev.shift && ev.code == sf::Keyboard::Key::Right)
                {
                    const size_t right = FindWordEnd(m_label.GetString(), static_cast<size_t>(m_caret.Index));

                    m_caret.Index = static_cast<int>(right);
                    m_caret.SelectionLength = 0;
                }
            }
        }
        else if (ev.code == sf::Keyboard::Key::Left)
        {
            m_caret.Index--;
            m_caret.SelectionLength = 0;
        }
        else if (ev.code == sf::Keyboard::Key::Right)
        {
            m_caret.Index++;
            m_caret.SelectionLength = 0;
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

        // Delete key
        if (Gx::GetCurrentPlatform() == Gx::Platform::macOS && ev.unicode == 127)
            return;

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

        if (m_caret.SelectionLength != 0)
        {
            const size_t end = start + std::abs(m_caret.SelectionLength);
            m_label.SetGlyphPreProcessor(
                [this, start, end](const Gx::Text::ShapedGlyph& glyph, std::uint32_t&, sf::Color& fillColor, sf::Color&, float&)
                {
                    if (glyph.cluster >= start && glyph.cluster < end)
                        fillColor = m_highlightColor;
                });
        }
        else
        {
            m_label.SetGlyphPreProcessor(nullptr);
        }
    }
}

namespace Cx
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

    const Gx::Rectangle& InputField::Caret::GetHighlight() const
    {
        return m_highlight;
    }

    void InputField::Caret::SetHighlightColor(const sf::Color color)
    {
        m_highlight.SetColor(color);
    }

    Gx::RenderStates InputField::Caret::Render(Gx::RenderSurface& surface, Gx::RenderStates states) const
    {
        if (!m_visible)
            return states;

        surface.Render(m_cursor, states);
        return states;
    }

    void InputField::Caret::Update(const sf::Time& delta)
    {
        m_elapsed += delta.asMilliseconds();
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
