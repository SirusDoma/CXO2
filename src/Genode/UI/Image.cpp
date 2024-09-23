#include <Genode/UI/Image.hpp>

namespace Gx
{
    sf::FloatRect Image::GetLocalBounds() const
    {
        return Sprite::GetLocalBounds();
    }

    sf::FloatRect Image::GetGlobalBounds() const
    {
        return Control::GetGlobalBounds();
    }

    unsigned int Image::GetFrameCount() const
    {
        return !m_frames.empty() ? m_frames.size() : 1;
    }

    const Image::Frame* Image::GetFrame(const std::string& name) const
    {
        if (const auto it = m_frames.find(name); it != m_frames.end())
            return &it->second;

        return nullptr;
    }

    const Image::Frame* Image::GetFrame(const unsigned int index) const
    {
        if (index >= m_frames.size())
            return nullptr;

        return GetFrame(m_indices[index]);
    }

    const Image::Frame* Image::GetCurrentFrame() const
    {
        return m_currentFrame;
    }

    const std::string& Image::GetCurrentFrameName() const
    {
        return m_frameName;
    }

    const unsigned int & Image::GetCurrentFrameIndex() const
    {
        return m_frameIndex;
    }

    bool Image::ContainsFrame(const std::string& name) const
    {
        const auto it = m_frames.find(name);
        return it != m_frames.end();
    }

    bool Image::ContainsFrame(const unsigned int index) const
    {
        return index >= 0 && index < m_frames.size();
    }

    void Image::AddFrame(const std::string& name, const sf::IntRect& texCoords)
    {
        if (m_frames.find(name) == m_frames.end())
            m_indices.push_back(name);

        m_frames[name] = Frame{texCoords};
        if (GetTexCoords() == sf::IntRect())
            SetFrame(name);
    }

    void Image::AddFrame(const std::string& name, const Frame& frame)
    {
        if (m_frames.find(name) == m_frames.end())
            m_indices.push_back(name);

        m_frames[name] = frame;
        if (GetTexCoords() == sf::IntRect())
            SetFrame(name);
    }

    void Image::SetFrame(const std::string& name)
    {
        for (std::size_t i = 0; i < m_indices.size(); i++)
        {
            if (m_indices[i] == name)
            {
                m_frameName = name;
                m_frameIndex = i;
                ApplyFrame(m_frames[m_frameName]);

                return;
            }
        }
    }

    void Image::SetFrame(const unsigned int index)
    {
        if (const auto frame = GetFrame(index); frame)
        {
            m_frameName = m_indices[index];
            m_frameIndex = index;
            ApplyFrame(*frame);
        }
    }

    void Image::ApplyFrame(const Frame& frame)
    {
        m_currentFrame = &frame;

        SetTexCoords(frame.TexCoords);

        SetOrigin(frame.Origin);
        SetPosition(frame.Position);
        SetRotation(frame.Rotation);
        SetScale(frame.Scale);

        Invalidate();
    }

    void Image::Update(const double delta)
    {
        Control::Update(delta);
    }

    RenderStates Image::Render(RenderSurface& surface, RenderStates states) const
    {
        if (!IsVisible())
            return states;

        return Sprite::Render(surface, states);
    }

    void Image::Invalidate()
    {
    }
}