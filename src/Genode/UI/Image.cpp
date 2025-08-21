#include <Genode/UI/Image.hpp>

#include <cmath>

namespace Gx
{
    Image::Image():
        m_vertices(),
        m_texture(nullptr),
        m_texcoords(),
        m_bounds(),
        m_sizeMode(SizeMode::Normal),
        m_blendMode(BlendMode::Auto),
        m_frameName(),
        m_frameIndex(0),
        m_currentFrame(nullptr),
        m_indices(),
        m_frames()
    {
    }

    Image::Image(const sf::Texture& texture) :
        m_vertices(),
        m_texture(nullptr),
        m_texcoords(),
        m_bounds(),
        m_sizeMode(SizeMode::Normal),
        m_blendMode(BlendMode::Auto),
        m_frameName(),
        m_frameIndex(0),
        m_currentFrame(nullptr),
        m_indices(),
        m_frames()
    {
        SetTexture(texture);
    }

    Image::Image(const sf::Texture& texture, const sf::IntRect& rectangle):
        m_vertices(),
        m_texture(nullptr),
        m_texcoords(),
        m_bounds(),
        m_sizeMode(SizeMode::Normal),
        m_blendMode(BlendMode::Auto),
        m_frameName(),
        m_frameIndex(0),
        m_currentFrame(nullptr),
        m_indices(),
        m_frames()
    {
        SetTexture(texture);
        SetTexCoords(rectangle);
    }

    sf::FloatRect Image::GetLocalBounds() const
    {
        if (m_bounds != sf::FloatRect())
            return m_bounds;

        const auto width = static_cast<float>(std::abs(m_texcoords.size.x));
        const auto height = static_cast<float>(std::abs(m_texcoords.size.y));

        return {{0.f, 0.f}, {width, height}};
    }

    sf::FloatRect Image::GetGlobalBounds() const
    {
        return Control::GetGlobalBounds();
    }

    const sf::Texture* Image::GetTexture() const
    {
        return m_texture;
    }

    const sf::IntRect& Image::GetTexCoords() const
    {
        return m_texcoords;
    }

    const sf::Color& Image::GetColor() const
    {
        return m_vertices[0].color;
    }

    Image::SizeMode Image::GetSizeMode() const
    {
        return m_sizeMode;
    }

    BlendMode Image::GetBlendMode() const
    {
        return m_blendMode;
    }

    void Image::SetTexture(const sf::Texture& texture, const bool resetRect)
    {
        if (resetRect || (!m_texture && (m_texcoords == sf::IntRect())))
            SetTexCoords(sf::IntRect({0, 0}, sf::Vector2i(texture.getSize())));

        m_texture = &texture;
    }

    void Image::SetTexCoords(const sf::IntRect& rectangle)
    {
        if (rectangle != m_texcoords)
        {
            m_texcoords = rectangle;
            UpdateVertices();
        }
    }

    void Image::SetLocalBounds(const sf::FloatRect& bounds)
    {
        if (bounds != m_bounds)
        {
            m_bounds = bounds;
            UpdateVertices();
        }
    }

    void Image::SetColor(const sf::Color& color)
    {
        for (auto& vertex : m_vertices)
            vertex.color = color;
    }

    void Image::SetSizeMode(const SizeMode sizeMode)
    {
        if (sizeMode != m_sizeMode)
        {
            m_sizeMode = sizeMode;
            UpdateVertices();
        }
    }

    void Image::SetBlendMode(const BlendMode blendMode)
    {
        m_blendMode = blendMode;
    }

    unsigned int Image::GetFrameCount() const
    {
        return !m_frames.empty() ? m_frames.size() : 1;
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

        states.transform *= GetTransform();
        states.coordinateType = sf::CoordinateType::Pixels;
        switch (m_blendMode)
        {
            case BlendMode::Alpha:          states.blendMode = sf::BlendAlpha;    break;
            case BlendMode::Additive:       states.blendMode = sf::BlendAdd;      break;
            case BlendMode::Multiplicative: states.blendMode = sf::BlendMultiply; break;
            case BlendMode::Min:            states.blendMode = sf::BlendMin;      break;
            case BlendMode::Max:            states.blendMode = sf::BlendMax;      break;
            case BlendMode::None:           states.blendMode = sf::BlendNone;     break;
            case BlendMode::Auto:                                                 break;
        }

        if (m_texture)
        {
            states.texture = m_texture;
            surface.Render(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TriangleStrip, states);
        }

        return RenderableContainer::Render(surface, states);
    }

    void Image::UpdateVertices()
    {
        const auto [position, size] = sf::FloatRect(m_texcoords);

        // Absolute value is used to support negative texture rect sizes
        const sf::Vector2f absSize(std::abs(size.x), std::abs(size.y));

        // Compute UV based on size mode
        float left = 0, top = 0, right = 0, bottom = 0;
        if (m_sizeMode == SizeMode::Center && m_bounds != sf::FloatRect())
        {
            // Center the sprite within the bounding box
            const sf::Vector2f offset(
                std::floor((m_bounds.size.x - absSize.x) * 0.5f),
                std::floor((m_bounds.size.y - absSize.y) * 0.5f)
            );

            left   = m_bounds.position.x + offset.x;
            top    = m_bounds.position.y + offset.y;
            right  = left + absSize.x;
            bottom = top  + absSize.y;
        }
        else if (m_sizeMode == SizeMode::Stretch && m_bounds != sf::FloatRect())
        {
            // Scale to fill the entire bounding box
            left   = m_bounds.position.x;
            top    = m_bounds.position.y;
            right  = m_bounds.position.x + m_bounds.size.x;
            bottom = m_bounds.position.y + m_bounds.size.y;
        }
        else
        {
            // Default UV: use the TexCoords
            left   = 0.f;
            top    = 0.f;
            right  = absSize.x;
            bottom = absSize.y;
        }

        // Update positions
        m_vertices[0].position = {left, top};
        m_vertices[1].position = {left, bottom};
        m_vertices[2].position = {right, top};
        m_vertices[3].position = {right, bottom};

        // Update texture coordinates
        m_vertices[0].texCoords = position;
        m_vertices[1].texCoords = position + sf::Vector2f(0.f, size.y);
        m_vertices[2].texCoords = position + sf::Vector2f(size.x, 0.f);
        m_vertices[3].texCoords = position + size;
    }

    void Image::Invalidate()
    {
    }
}