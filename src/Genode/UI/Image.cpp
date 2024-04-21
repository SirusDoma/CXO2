#include <Genode/UI/Image.hpp>

namespace Gx
{
    sf::FloatRect Image::GetLocalBounds() const
    {
        return Sprite::GetLocalBounds();
    }

    unsigned int Image::GetFrameCount() const
    {
        return !m_frames.empty() ? m_frames.size() : 1;
    }

    const Image::Frame *Image::GetFrame(const std::string &name) const
    {
        if (auto it = m_frames.find(name); it != m_frames.end())
            return &it->second;

        return nullptr;
    }

    const Image::Frame *Image::GetFrame(unsigned int index) const
    {
        if (index < 0 || index >= m_frames.size())
            return nullptr;

        return GetFrame(m_indices[index]);
    }

    const Image::Frame *Image::GetCurrentFrame() const
    {
        return m_currentFrame;
    }

    const std::string &Image::GetCurrentFrameName() const
    {
        return m_frameName;
    }

    bool Image::ContainsFrame(const std::string &name) const
    {
        auto it = m_frames.find(name);
        return it != m_frames.end();
    }

    bool Image::ContainsFrame(unsigned int index) const
    {
        return index >= 0 && index < m_frames.size();
    }

    void Image::AddFrame(const std::string &name, const sf::IntRect &texCoords)
    {
        if (m_frames.find(name) == m_frames.end())
            m_indices.push_back(name);

        m_frames[name] = Frame{texCoords};
        if (GetTexCoords() == sf::IntRect())
            SetFrame(name);
    }

    void Image::AddFrame(const std::string &name, const Image::Frame &frame)
    {
        if (m_frames.find(name) == m_frames.end())
            m_indices.push_back(name);

        m_frames[name] = frame;
        if (GetTexCoords() == sf::IntRect())
            SetFrame(name);
    }

    void Image::SetFrame(const std::string &name)
    {
        if (auto frame = GetFrame(name); frame)
        {
            m_frameName = name;
            ApplyFrame(*frame);
        }
    }

    void Image::SetFrame(unsigned int index)
    {
        if (auto frame = GetFrame(index); frame)
        {
            m_frameName = m_indices[index];
            ApplyFrame(*frame);
        }
    }

    void Image::ApplyFrame(const Image::Frame &frame)
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

    RenderStates Image::Render(sf::RenderTarget &target, RenderStates states) const
    {
        if (!IsVislble())
            return states;

        return Sprite::Render(target, states);
    }

    void Image::Invalidate()
    {
    }
}