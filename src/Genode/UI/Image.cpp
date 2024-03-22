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

    Image::Frame *Image::GetFrame(const std::string &name) const
    {
        if (auto it = m_frames.find(name); it != m_frames.end())
            return const_cast<Image::Frame*>(&it->second);

        return nullptr;
    }

    Image::Frame *Image::GetFrame(unsigned int index) const
    {
        if (index < 0 || index >= m_indices.size())
            return nullptr;

        if (auto i = m_indices.find(index); i != m_indices.end())
            return GetFrame(i->second);

        return nullptr;
    }

    Image::Frame *Image::GetCurrentFrame() const
    {
        return m_currentFrame;
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
        m_indices[m_indices.size()] = name;
        m_frames[name] = Frame{
            .TexCoords = texCoords,
            .Name      = name
        };
        if (GetTexCoords() == sf::IntRect())
            SetFrame(name);
    }

    void Image::AddFrame(const std::string &name, const Image::Frame &frame)
    {
        m_indices[m_indices.size()] = name;
        m_frames[name] = frame;
        m_frames[name].Name = name;

        if (GetTexCoords() == sf::IntRect())
            SetFrame(name);
    }

    void Image::SetFrame(const std::string &name)
    {
        if (auto frame = GetFrame(name); frame)
            ApplyFrame(*frame);
    }

    void Image::SetFrame(unsigned int index)
    {
        if (auto frame = GetFrame(index); frame)
        {
            m_currentIndex = index;
            ApplyFrame(*frame);
        }
    }

    void Image::NextFrame()
    {
        if (m_currentIndex++; !ContainsFrame(m_currentIndex))
            m_currentIndex = 0;

        SetFrame(m_currentIndex);
    }

    void Image::PreviousFrame()
    {
        if (m_currentIndex--; !ContainsFrame(m_currentIndex))
            m_currentIndex = m_frames.size() - 1;

        SetFrame(m_currentIndex);
    }

    void Image::ApplyFrame(Image::Frame &frame)
    {
        m_currentFrame = &frame;

        SetTexCoords(frame.TexCoords);

        SetOrigin(frame.Origin);
        SetPosition(frame.Position);
        SetRotation(frame.Rotation);
        SetScale(frame.Scale);

        Invalidate();
    }

    void Image::Update(double delta)
    {
        Control::Update(delta);
    }

    sf::RenderStates Image::Render(sf::RenderTarget &target, sf::RenderStates states) const
    {
        if (!IsVislble())
            return states;

        return Sprite::Render(target, states);
    }

    void Image::Invalidate()
    {
    }
}
