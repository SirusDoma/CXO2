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
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#include <Genode/Graphics/Shapes/Circle.hpp>
#include <cmath>

namespace Gx
{
    Circle::Circle(float radius, std::size_t pointCount) :
        m_radius(radius),
        m_pointCount(pointCount)
    {
        Update();
    }

    void Circle::SetRadius(float radius)
    {
        m_radius = radius;
        Update();
    }

    float Circle::GetRadius() const
    {
        return m_radius;
    }

    void Circle::SetPointCount(std::size_t count)
    {
        m_pointCount = count;
        Update();
    }

    std::size_t Circle::GetPointCount() const
    {
        return m_pointCount;
    }

    sf::Vector2f Circle::GetPoint(std::size_t index) const
    {
        static const float pi = 3.141592654f;

        float angle = index * 2 * pi / m_pointCount - pi / 2;
        float x = std::cos(angle) * m_radius;
        float y = std::sin(angle) * m_radius;

        return sf::Vector2f(m_radius + x, m_radius + y);
    }

}