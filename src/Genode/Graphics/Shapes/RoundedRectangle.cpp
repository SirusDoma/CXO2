////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2018 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
// you must not claim that you wrote the original software.
// If you use this software in a product, an acknowledgment
// in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
// and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#include <Genode/Graphics/Shapes/RoundedRectangle.hpp>
#include <cmath>

namespace Gx
{
    RoundedRectangle::RoundedRectangle(const sf::Vector2f& size, const float cornerRadius, const unsigned int cornerPointCount) :
        m_size(size),
        m_cornerRadius(cornerRadius),
        m_cornerPointCount(cornerPointCount)
    {
    }

    void RoundedRectangle::SetSize(const sf::Vector2f& size)
    {
        m_size = size;
    }

    const sf::Vector2f& RoundedRectangle::GetSize() const
    {
        return m_size;
    }

    void RoundedRectangle::SetCornerRadius(const float radius)
    {
        m_cornerRadius = radius;
    }

    float RoundedRectangle::GetCornerRadius() const
    {
        return m_cornerRadius;
    }

    void RoundedRectangle::SetCornerPointCount(const unsigned int cornerPointCount)
    {
        m_cornerPointCount = cornerPointCount;
    }

    unsigned int RoundedRectangle::GetCornerPointCount() const
    {
        return m_cornerPointCount;
    }

    std::size_t RoundedRectangle::GetPointCount() const
    {
        return m_cornerPointCount * 4;
    }

    sf::Vector2f RoundedRectangle::GetPoint(const std::size_t index) const
    {
        if(index >= m_cornerPointCount * 4)
            return {0,0};

        const float deltaAngle = 90.0f / static_cast<float>(m_cornerPointCount - 1);
        sf::Vector2f center;
        const unsigned int centerIndex = index / m_cornerPointCount;
        static const float pi = 3.141592654f;

        switch(centerIndex)
        {
            default:
            case 0: center.x = m_size.x - m_cornerRadius; center.y = m_cornerRadius; break;
            case 1: center.x = m_cornerRadius; center.y = m_cornerRadius; break;
            case 2: center.x = m_cornerRadius; center.y = m_size.y - m_cornerRadius; break;
            case 3: center.x = m_size.x - m_cornerRadius; center.y = m_size.y - m_cornerRadius; break;
        }

        return { m_cornerRadius * std::cos(deltaAngle * static_cast<float>(index-centerIndex) * pi / 180) + center.x,
                -m_cornerRadius * std::sin(deltaAngle * static_cast<float>(index-centerIndex) * pi / 180) + center.y };
    }
}