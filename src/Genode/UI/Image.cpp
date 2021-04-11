#include <Genode/UI/Image.hpp>

namespace Gx
{
    Image::~Image()
    {
    }

    const sf::FloatRect Image::GetLocalBounds() const
    {
        return Sprite::GetLocalBounds();
    }

    unsigned int Image::GetFrameCount() const
    {
        return m_frames.size() > 0 ? m_frames.size() : 1;
    }

    const sf::IntRect &Image::GetFrame(const std::string &name)
    {
        return m_frames[name];
    }

    const sf::IntRect &Image::GetFrame(unsigned int index)
    {
        if (index < 0 || index >= m_frames.size())
            return GetTexCoords();

        unsigned int it = 0;
        for (auto [name, _] : m_frames)
        {
            if (it == index)
                return GetFrame(name);

            it++;
        }

        return GetTexCoords();
    }

    void Image::AddFrame(const std::string &name, const sf::IntRect &texCoords)
    {
        m_frames[name] = texCoords;
        if (GetTexCoords() == sf::IntRect())
            SetFrame(name);
    }

    void Image::SetFrame(const std::string &name)
    {
        SetTexCoords(GetFrame(name));
        Invalidate();
    }

    void Image::SetFrame(unsigned int index)
    {
        SetTexCoords(GetFrame(index));
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
