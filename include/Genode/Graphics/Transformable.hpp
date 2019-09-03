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

#ifndef GENODE_TRANSFORMABLE_HPP
#define GENODE_TRANSFORMABLE_HPP

#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Transform.hpp>

namespace Gx
{
    class Transformable
    {
    public:
        Transformable();
        virtual ~Transformable();

        void SetPosition(float x, float y);
        void SetPosition(const sf::Vector2f& position);
        void SetRotation(float angle);
        void SetScale(float factorX, float factorY);
        void SetScale(const sf::Vector2f& factors);

        void SetOrigin(float x, float y);
        void SetOrigin(const sf::Vector2f& origin);

        const sf::Vector2f& GetPosition() const;
        float GetRotation() const;
        const sf::Vector2f& GetScale() const;
        const sf::Vector2f& GetOrigin() const;

        void Move(float offsetX, float offsetY);
        void Move(const sf::Vector2f& offset);
        void Rotate(float angle);
        void Scale(float factorX, float factorY);
        void Scale(const sf::Vector2f& factor);

        const sf::Transform& GetTransform() const;
        const sf::Transform& GetInverseTransform() const;

    private:
        sf::Vector2f          m_origin;                     
        sf::Vector2f          m_position;                   
        float                 m_rotation;                   
        sf::Vector2f          m_scale;                      
        mutable sf::Transform m_transform;                  
        mutable bool          m_transformNeedUpdate;        
        mutable sf::Transform m_inverseTransform;           
        mutable bool          m_inverseTransformNeedUpdate;
    };

}


#endif