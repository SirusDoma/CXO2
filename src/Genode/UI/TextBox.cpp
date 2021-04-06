#include <Genode/UI/TextBox.hpp>
#include <clip/clip.h>

namespace Gx
{
    TextBox::TextBox() :
        m_text(),
        m_caret(*this),
        m_bounds(),
        m_maxLength()
    {
    }

    TextBox::TextBox(const sf::String &string, const sf::Font &font, unsigned int characterSize, sf::FloatRect bounds) :
        m_text(string, font, characterSize),
        m_caret(*this),
        m_bounds(bounds),
        m_maxLength(0)
    {
        if (m_bounds == sf::FloatRect())
            m_bounds = m_text.GetLocalBounds();

        SetFocus(true);
        SetHighlightBackColor(sf::Color::White);
        SetHighlightTextColor(sf::Color::Black);
    }

    const sf::FloatRect TextBox::GetLocalBounds() const
    {
        return m_bounds;
    }

    void TextBox::SetLocalBounds(sf::FloatRect bounds)
    {
        m_bounds = bounds;
    }

    sf::Vector2f TextBox::FindCharacterPosition(std::size_t index) const
    {
        return m_text.FindCharacterPosition(index);
    }

    void TextBox::SetString(const sf::String &string)
    {
        m_text.SetString(string);
    }

    void TextBox::SetFont(const sf::Font &font)
    {
        m_text.SetFont(font);
    }

    void TextBox::SetCharacterSize(unsigned int size)
    {
        m_text.SetCharacterSize(size);
    }

    void TextBox::SetLineSpacing(float spacingFactor)
    {
        m_text.SetLetterSpacing(spacingFactor);
    }

    void TextBox::SetLetterSpacing(float spacingFactor)
    {
        m_text.SetLetterSpacing(spacingFactor);
    }

    void TextBox::SetStyle(Uint32 style)
    {
        m_text.SetStyle(style);
    }

    void TextBox::SetColor(const sf::Color &color)
    {
        m_text.SetColor(color);
    }

    void TextBox::SetFillColor(const sf::Color &color)
    {
        m_text.SetFillColor(color);
    }

    void TextBox::SetHighlightBackColor(const sf::Color &color)
    {
        m_caret.SetHighlightColor(color);
    }

    void TextBox::SetHighlightTextColor(const sf::Color &color)
    {
        m_highlightColor = color;
    }

    void TextBox::SetOutlineColor(const sf::Color &color)
    {
        m_text.SetOutlineColor(color);
    }

    void TextBox::SetOutlineThickness(float thickness)
    {
        m_text.SetOutlineThickness(thickness);
    }

    const sf::String &TextBox::GetString() const
    {
        return m_text.GetString();
    }

    const sf::Font *TextBox::GetFont() const
    {
        return m_text.GetFont();
    }

    unsigned int TextBox::GetCharacterSize() const
    {
        return m_text.GetCharacterSize();
    }

    float TextBox::GetLetterSpacing() const
    {
        return m_text.GetLetterSpacing();
    }

    float TextBox::GetLineSpacing() const
    {
        return m_text.GetLineSpacing();
    }

    Uint32 TextBox::GetStyle() const
    {
        return m_text.GetStyle();
    }

    const sf::Color &TextBox::GetColor() const
    {
        return m_text.GetColor();
    }

    const sf::Color &TextBox::GetFillColor() const
    {
        return m_text.GetFillColor();
    }
    const sf::Color &TextBox::GetHighlightBackColor() const
    {
        return m_caret.GetHighlight().GetColor();
    }

    const sf::Color &TextBox::GetHighlightTextColor() const
    {
        return m_highlightColor;
    }

    const sf::Color &TextBox::GetOutlineColor() const
    {
        return m_text.GetOutlineColor();
    }

    float TextBox::GetOutlineThickness() const
    {
        return m_text.GetOutlineThickness();
    }

    unsigned int TextBox::GetMaximumTextLength() const
    {
        return m_maxLength;
    }

    void TextBox::SetMaximumTextLength(unsigned int maxLength)
    {
        m_maxLength = maxLength;
    }

    void TextBox::SetTextEnteredCallback(std::function<void(TextBox&, sf::String)> callback)
    {
        m_onTextEntered = callback;
    }

    bool TextBox::IsNextCharacterFit()
    {
        auto string = m_text.GetString();
        auto index  = m_caret.Index;

        if (m_caret.SelectionLength != 0)
            index = Erase(index - 1, m_caret.SelectionLength);

        auto newString = m_text.GetString();
        newString.insert(index, " ");
        m_text.SetString(newString);
        bool fit = m_text.GetLocalBounds().width <= m_bounds.width;

        m_text.SetString(string);
        return fit;
    }

    sf::String TextBox::GetSelectedText() const
    {
        auto index  = m_caret.Index - 1;
        auto length = m_caret.SelectionLength;
        if (length < 0)
            index += length + 1;
        else if (length > 0)
            index++;
        else
            return sf::String();

        return m_text.GetString().substring(index, std::abs(length));
    }

    size_t TextBox::Insert(size_t index, Uint32 unicode, int selectionLength)
    {
        // backspace, tab, enter, etc
        if (unicode <= 31)
            return index;

        // Max length validation
        if (m_maxLength > 0 && selectionLength == 0 &&  m_text.GetString().getSize() >= m_maxLength)
            return index;

        // Max visual bounds validation
        if (IsNextCharacterFit())
        {
            if (m_caret.SelectionLength != 0)
                index = Erase(index - 1, selectionLength);

            auto string = m_text.GetString();
            string.insert(index, unicode);
            m_text.SetString(string);

            return ++index;
        }

        return index;
    }

    size_t TextBox::Erase(size_t index, int length)
    {
        if (length < 0)
            index += length + 1;
        else if (length > 0)
            index++;
        else
            return index;

        auto str = m_text.GetString();
        str.erase(index, length == 0 ? 1 : std::abs(length));
        m_text.SetString(str);

        return index;
    }

    void TextBox::Update(double delta)
    {
        m_caret.Update(delta);
        Control::Update(delta);
    }

    sf::RenderStates TextBox::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        states.transform *= GetTransform();
        if (m_caret.SelectionLength != 0)
            target.draw(m_caret.GetHighlight(), states);

        target.draw(m_text, states);
        if (IsFocused())
            target.draw(m_caret, states);

        return Control::Render(target, states);
    }

    void TextBox::OnControlClick(Control *sender, sf::Event::MouseButtonEvent ev)
    {
        float minDistance  = -1;
        size_t selectIndex = m_caret.Index;
        for (size_t index = 0; index <= m_text.GetString().getSize(); index++)
        {
            float distance = std::abs((FindCharacterPosition(index).x + GetPosition().x) - ev.x);
            if (minDistance == -1 || distance < minDistance)
            {
                selectIndex = index;
                minDistance = distance;
            }
            else if (distance > minDistance)
                break;
        }

        SetFocus(true);
        m_caret.Index = selectIndex;

        Invalidate();
    }

    void TextBox::OnMouseMove(sf::Event::MouseMoveEvent ev)
    {
        bool focused = IsFocused();
        Control::OnMouseMove(ev);

        SetFocus(focused);
    }

    void TextBox::OnMouseButtonUp(sf::Event::MouseButtonEvent ev)
    {
        Control::OnMouseButtonUp(ev);
        SetFocus(GetControlState() != Control::State::Normal);
    }

    void TextBox::OnKeyDown(sf::Event::KeyEvent ev)
    {
        if (!IsEnabled() || !IsFocused())
            return;

        if (ev.code == sf::Keyboard::Backspace || (ev.code == sf::Keyboard::Delete && m_caret.SelectionLength != 0))
        {
            if (m_caret.Index == 0 && m_caret.SelectionLength == 0)
                return;

            int length   = m_caret.SelectionLength;
            m_caret.Index = Erase(m_caret.Index - 1, length == 0 ? -1 : length);
            m_caret.SelectionLength = 0;
        }
        else if (ev.code == sf::Keyboard::Delete)
        {
            if (m_caret.Index >= m_text.GetString().getSize())
                return;

            auto str = m_text.GetString();
            str.erase(m_caret.Index);

            m_text.SetString(str);
            m_caret.SelectionLength = 0;
        }
        else if (ev.code == sf::Keyboard::Enter)
        {
            if (m_onTextEntered)
                m_onTextEntered(*this, m_text.GetString());

            m_text.SetString("");
            m_caret.SelectionLength = 0;
        }
        else if (ev.control)
        {
            if (ev.code == sf::Keyboard::C || ev.code == sf::Keyboard::X)
            {
                clip::set_text(GetSelectedText());
                if (ev.code == sf::Keyboard::X)
                {
                    m_caret.Index = Erase(m_caret.Index - 1, m_caret.SelectionLength);
                    m_caret.SelectionLength = 0;
                }
            }
            else if (ev.code == sf::Keyboard::V)
            {
                auto string = std::string();
                clip::get_text(string);

                for (size_t index = 0; index < string.size(); index++)
                    m_caret.Index = Insert(m_caret.Index, string[index]);
            }
        }
        else if (!ev.shift)
        {
            if (ev.code == sf::Keyboard::Left)
            {
                m_caret.Index--;
                m_caret.SelectionLength = 0;
                m_text.SetFillColor(m_text.GetFillColor());
            }
            else if (ev.code == sf::Keyboard::Right)
            {
                m_caret.Index++;
                m_caret.SelectionLength = 0;
                m_text.SetFillColor(m_text.GetFillColor());
            }
            else
                return;
        }
        else if (ev.shift)
        {
            if (ev.code == sf::Keyboard::Left)
            {
                if (m_caret.Index <= 0)
                    return;

                m_caret.Index--;
                m_caret.SelectionLength++;
            }
            else if (ev.code == sf::Keyboard::Right)
            {
                if (m_caret.Index >= m_text.GetString().getSize())
                    return;

                m_caret.Index++;
                m_caret.SelectionLength--;
            }
            else
                return;
        }

        Invalidate();
    }

    void TextBox::OnKeyType(sf::Event::TextEvent ev)
    {
        if (!IsEnabled() || !IsFocused())
            return;

        // backspace, tab, enter, etc
        if (ev.unicode <= 31)
            return;

        m_caret.Index = Insert(m_caret.Index, ev.unicode, m_caret.SelectionLength);
        m_caret.SelectionLength = 0;

        Invalidate();
    }

    void TextBox::Invalidate()
    {
        m_caret.Invalidate();
        m_caret.Reset(true);

        size_t start = m_caret.Index;
        if (m_caret.SelectionLength < 0)
            start += m_caret.SelectionLength;

        m_text.SetFillColor(m_text.GetFillColor());
        if (m_caret.SelectionLength != 0)
        {
            for (size_t index = 0; index < m_text.GetString().getSize(); index++)
            {
                if (index >= start && index < start + std::abs(m_caret.SelectionLength))
                    m_text.SetFillColor(m_highlightColor, index);
            }
        }
    }
}

namespace Gx
{
    TextBox::Caret::Caret(TextBox &instance) :
        Instance(instance),
        Index(),
        SelectionLength(),
        m_cursor(),
        m_highlight(),
        m_elapsed(),
        m_visible(true)
    {
        SetHighlightColor(sf::Color::Transparent);
        Invalidate();
    }

    void TextBox::Caret::Reset(bool visible)
    {
        m_elapsed = 0;
        m_visible = visible;
    }

    const Rectangle &TextBox::Caret::GetHighlight() const
    {
        return m_highlight;
    }

    void TextBox::Caret::SetHighlightColor(sf::Color color)
    {
        m_highlight.SetColor(color);
    }

    sf::RenderStates TextBox::Caret::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!m_visible)
            return states;

        target.draw(m_cursor, states);
        return states;
    }

    void TextBox::Caret::Update(double delta)
    {
        m_elapsed += delta;
        if (m_elapsed >= BLINK_THRESHOLD)
        {
            m_elapsed = 0;
            m_visible = !m_visible;
        }
    }

    void TextBox::Caret::Invalidate()
    {
        if (Index < 0)
            Index = 0;

        if (Index > Instance.GetString().getSize())
            Index = Instance.GetString().getSize();

        if (Instance.GetFont())
        {
            auto glyph = Instance.GetFont()->getGlyph('|', Instance.GetCharacterSize(), false);
            m_cursor.SetSize(sf::Vector2f(glyph.bounds.width * 0.65f, Instance.GetCharacterSize()));
        }

        m_cursor.SetPosition(Instance.FindCharacterPosition(Index));
        m_cursor.SetFillColor(Instance.GetColor());

        if (SelectionLength != 0)
        {
            size_t index = Index - 1;
            int length   = SelectionLength;
            if (length < 0)
                index += length + 1;
            else if (length > 0)
                index++;

            auto charPos = Instance.FindCharacterPosition(index);
            auto endPos  = Instance.FindCharacterPosition(index + std::abs(length));
            m_highlight.SetPosition(sf::Vector2f(charPos.x, charPos.y + 0.65f));
            m_highlight.SetSize(sf::Vector2f(std::abs(charPos.x - endPos.x), Instance.GetCharacterSize()));
        }
        else
            m_highlight.SetSize(sf::Vector2f());
    }
}