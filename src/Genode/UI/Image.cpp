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

    void Image::AddFrame(const std::string &name, const sf::IntRect &texCoords)
    {
        m_frames[name] = texCoords;
        if (GetTexCoords() == sf::IntRect())
            SetFrame(name);
    }

    void Image::SetFrame(const std::string &name)
    {
        SetTexCoords(m_frames[name]);
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