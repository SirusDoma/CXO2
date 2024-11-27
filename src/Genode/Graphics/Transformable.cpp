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

#include <Genode/Graphics/Transformable.hpp>
#include <cmath>

namespace Gx
{
    Transformable::~Transformable()
    {
    }

    void Transformable::SetPosition(const float x, const float y)
    {
        if (x == m_position.x && y == m_position.y)
            return;

        m_position.x                 = x;
        m_position.y                 = y;
        m_transformNeedUpdate        = true;
        m_inverseTransformNeedUpdate = true;
    }

    void Transformable::SetPosition(const sf::Vector2f& position)
    {
        if (m_position == position)
            return;

        m_position                   = position;
        m_transformNeedUpdate        = true;
        m_inverseTransformNeedUpdate = true;
    }

    void Transformable::SetRotation(const float angle)
    {
        if (m_rotation == sf::degrees(angle))
            return;

        m_rotation                   = sf::degrees(angle);
        m_transformNeedUpdate        = true;
        m_inverseTransformNeedUpdate = true;
    }

    void Transformable::SetRotation(const sf::Angle& angle)
    {
        if (m_rotation == angle)
            return;

        m_rotation                   = angle;
        m_transformNeedUpdate        = true;
        m_inverseTransformNeedUpdate = true;
    }

    void Transformable::SetScale(const float factorX, const float factorY)
    {
        if (factorX == m_position.x && factorY == m_position.y)
            return;

        m_scale.x                    = factorX;
        m_scale.y                    = factorY;
        m_transformNeedUpdate        = true;
        m_inverseTransformNeedUpdate = true;
    }

    void Transformable::SetScale(const sf::Vector2f& factors)
    {
        if (m_scale == factors)
            return;

        m_scale                      = factors;
        m_transformNeedUpdate        = true;
        m_inverseTransformNeedUpdate = true;
    }

    void Transformable::SetOrigin(const float x, const float y)
    {
        if (x == m_origin.x && y == m_origin.y)
            return;

        m_origin.x = x;
        m_origin.y = y;
        m_transformNeedUpdate = true;
        m_inverseTransformNeedUpdate = true;
    }

    void Transformable::SetOrigin(const sf::Vector2f& origin)
    {
        if (m_origin == origin)
            return;

        m_origin                     = origin;
        m_transformNeedUpdate        = true;
        m_inverseTransformNeedUpdate = true;
    }

    const sf::Vector2f& Transformable::GetPosition() const
    {
        return m_position;
    }

    const sf::Angle& Transformable::GetRotation() const
    {
        return m_rotation;
    }

    const sf::Vector2f& Transformable::GetScale() const
    {
        return m_scale;
    }

    const sf::Vector2f& Transformable::GetOrigin() const
    {
        return m_origin;
    }

    void Transformable::Move(const float offsetX, const float offsetY)
    {
        SetPosition(m_position.x + offsetX, m_position.y + offsetY);
    }

    void Transformable::Move(const sf::Vector2f& offset)
    {
        SetPosition(m_position + offset);
    }

    void Transformable::Rotate(const float angle)
    {
        SetRotation(m_rotation + sf::degrees(angle));
    }

    void Transformable::Rotate(const sf::Angle& angle)
    {
        SetRotation(m_rotation + angle);
    }

    void Transformable::Scale(const float factorX, const float factorY)
    {
        SetScale(m_scale.x * factorX, m_scale.y * factorY);
    }

    void Transformable::Scale(const sf::Vector2f& factor)
    {
        SetScale({m_scale.x * factor.x, m_scale.y * factor.y});
    }

    const sf::Transform& Transformable::GetTransform() const
    {
        // Recompute the combined transform if needed
        if (m_transformNeedUpdate)
        {
            const float angle  = -m_rotation.asRadians();
            const float cosine = std::cos(angle);
            const float sine   = std::sin(angle);
            const float sxc    = m_scale.x * cosine;
            const float syc    = m_scale.y * cosine;
            const float sxs    = m_scale.x * sine;
            const float sys    = m_scale.y * sine;
            const float tx     = -m_origin.x * sxc - m_origin.y * sys + m_position.x;
            const float ty     = m_origin.x * sxs - m_origin.y * syc + m_position.y;

            // clang-format off
            m_transform = sf::Transform( sxc, sys, tx,
                                        -sxs, syc, ty,
                                         0.f, 0.f, 1.f);
            // clang-format on
            m_transformNeedUpdate = false;
        }

        return m_transform;
    }

    const sf::Transform& Transformable::GetInverseTransform() const
    {
        // Recompute the inverse transform if needed
        if (m_inverseTransformNeedUpdate)
        {
            m_inverseTransform = GetTransform().getInverse();
            m_inverseTransformNeedUpdate = false;
        }

        return m_inverseTransform;
    }

}
