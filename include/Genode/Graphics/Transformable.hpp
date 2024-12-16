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

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/Export.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>

namespace Gx
{
    ////////////////////////////////////////////////////////////
    /// @brief Decomposed transform defined by a position, a rotation and a scale
    ////////////////////////////////////////////////////////////
    class Transformable
    {
    public:
        ////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ////////////////////////////////////////////////////////////
        Transformable() = default;

        ////////////////////////////////////////////////////////////
        /// @brief Virtual destructor
        ////////////////////////////////////////////////////////////
        virtual ~Transformable() = default;

        ////////////////////////////////////////////////////////////
        /// @brief set the position of the object
        ///
        /// This function completely overwrites the previous position.
        /// See the move function to apply an offset based on the previous position instead.
        /// The default position of a transformable object is (0, 0).
        ///
        /// @param x New x coordinate position
        /// @param y New y coordinate position
        ///
        /// @see `Move`, `GetPosition`
        ////////////////////////////////////////////////////////////
        void SetPosition(float x, float y);

        ////////////////////////////////////////////////////////////
        /// @brief set the position of the object
        ///
        /// This function completely overwrites the previous position.
        /// See the move function to apply an offset based on the previous position instead.
        /// The default position of a transformable object is (0, 0).
        ///
        /// @param position New position
        ///
        /// @see `Move`, `GetPosition`
        ////////////////////////////////////////////////////////////
        void SetPosition(const sf::Vector2f& position);

        ////////////////////////////////////////////////////////////
        /// @brief set the orientation of the object
        ///
        /// This function completely overwrites the previous rotation.
        /// See the rotate function to add an angle based on the previous rotation instead.
        /// The default rotation of a transformable object is 0.
        ///
        /// @param angle New rotation
        ///
        /// @see `Rotate`, `GetRotation`
        ////////////////////////////////////////////////////////////
        void SetRotation(float angle);

        ////////////////////////////////////////////////////////////
        /// @brief set the orientation of the object
        ///
        /// This function completely overwrites the previous rotation.
        /// See the rotate function to add an angle based on the previous rotation instead.
        /// The default rotation of a transformable object is 0.
        ///
        /// @param angle New rotation
        ///
        /// @see `Rotate`, `getRotation`
        ////////////////////////////////////////////////////////////
        void SetRotation(const sf::Angle& angle);

        ////////////////////////////////////////////////////////////
        /// @brief set the scale factors of the object
        ///
        /// This function completely overwrites the previous scale.
        /// See the scale function to add a factor based on the previous scale instead.
        /// The default scale of a transformable object is (1, 1).
        ///
        /// @param factorX New horizontal scale factors
        /// @param factorY New vertical scale factors
        ///
        /// @see `Scale`, `GetScale`
        ////////////////////////////////////////////////////////////
        void SetScale(float factorX, float factorY);

        ////////////////////////////////////////////////////////////
        /// @brief set the scale factors of the object
        ///
        /// This function completely overwrites the previous scale.
        /// See the scale function to add a factor based on the previous scale instead.
        /// The default scale of a transformable object is (1, 1).
        ///
        /// @param factors New scale factors
        ///
        /// @see `Scale`, `GetScale`
        ////////////////////////////////////////////////////////////
        void SetScale(const sf::Vector2f& factors);

        ////////////////////////////////////////////////////////////
        /// @brief set the local origin of the object
        ///
        /// The origin of an object defines the center point for
        /// all transformations (position, scale, rotation).
        /// The coordinates of this point must be relative to the
        /// top-left corner of the object, and ignore all
        /// transformations (position, scale, rotation).
        /// The default origin of a transformable object is (0, 0).
        ///
        /// @param x New x coordinate origin
        /// @param y New y coordinate origin
        ///
        /// @see `GetOrigin`
        ////////////////////////////////////////////////////////////
        void SetOrigin(float x, float y);

        ////////////////////////////////////////////////////////////
        /// @brief set the local origin of the object
        ///
        /// The origin of an object defines the center point for
        /// all transformations (position, scale, rotation).
        /// The coordinates of this point must be relative to the
        /// top-left corner of the object, and ignore all
        /// transformations (position, scale, rotation).
        /// The default origin of a transformable object is (0, 0).
        ///
        /// @param origin New origin
        ///
        /// @see `GetOrigin`
        ////////////////////////////////////////////////////////////
        void SetOrigin(const sf::Vector2f& origin);

        ////////////////////////////////////////////////////////////
        /// @brief Get the position of the object
        ///
        /// @return Current position
        ///
        /// @see `SetPosition`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Vector2f& GetPosition() const;
        
        ////////////////////////////////////////////////////////////
        /// @brief Get the orientation of the object
        ///
        /// The rotation is always in the range [0, 360].
        ///
        /// @return Current rotation
        ///
        /// @see `SetRotation`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Angle& GetRotation() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the current scale of the object
        ///
        /// @return Current scale factors
        ///
        /// @see `SetScale`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Vector2f& GetScale() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the local origin of the object
        ///
        /// @return Current origin
        ///
        /// @see `SetOrigin`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Vector2f& GetOrigin() const;

        ////////////////////////////////////////////////////////////
        /// @brief Move the object by a given offset
        ///
        /// This function adds to the current position of the object,
        /// unlike `setPosition` which overwrites it.
        /// Thus, it is equivalent to the following code:
        /// @code
        /// object.SetPosition(object.GetPosition() + offset);
        /// @endcode
        ///
        /// @param offsetX x offset
        /// @param offsetY y offset
        ///
        /// @see `setPosition`
        ////////////////////////////////////////////////////////////
        void Move(float offsetX, float offsetY);

        ////////////////////////////////////////////////////////////
        /// @brief Move the object by a given offset
        ///
        /// This function adds to the current position of the object,
        /// unlike `setPosition` which overwrites it.
        /// Thus, it is equivalent to the following code:
        /// @code
        /// object.SetPosition(object.GetPosition() + offset);
        /// @endcode
        ///
        /// @param offset Offset
        ///
        /// @see `SetPosition`
        ////////////////////////////////////////////////////////////
        void Move(const sf::Vector2f& offset);

        ////////////////////////////////////////////////////////////
        /// @brief Rotate the object
        ///
        /// This function adds to the current rotation of the object,
        /// unlike `setRotation` which overwrites it.
        /// Thus, it is equivalent to the following code:
        /// @code
        /// object.SetRotation(object.GetRotation() + angle);
        /// @endcode
        ///
        /// @param angle Angle of rotation
        ////////////////////////////////////////////////////////////
        void Rotate(float angle);

        ////////////////////////////////////////////////////////////
        /// @brief Rotate the object
        ///
        /// This function adds to the current rotation of the object,
        /// unlike `setRotation` which overwrites it.
        /// Thus, it is equivalent to the following code:
        /// @code
        /// object.SetRotation(object.SetRotation() + angle);
        /// @endcode
        ///
        /// @param angle Angle of rotation
        ////////////////////////////////////////////////////////////
        void Rotate(const sf::Angle& angle);

        ////////////////////////////////////////////////////////////
        /// @brief Scale the object
        ///
        /// This function multiplies the current scale of the object,
        /// unlike `setScale` which overwrites it.
        /// Thus, it is equivalent to the following code:
        /// @code
        /// sf::Vector2f scale = object.GetScale();
        /// object.SetScale(scale.x * factor.x, scale.y * factor.y);
        /// @endcode
        ///
        /// @param factorX Horizontal scale factors
        /// @param factorY Vertical scale factors
        ///
        /// @see `SetScale`
        ////////////////////////////////////////////////////////////
        void Scale(float factorX, float factorY);
        
        ////////////////////////////////////////////////////////////
        /// @brief Scale the object
        ///
        /// This function multiplies the current scale of the object,
        /// unlike `setScale` which overwrites it.
        /// Thus, it is equivalent to the following code:
        /// @code
        /// sf::Vector2f scale = object.getScale();
        /// object.setScale(scale.x * factor.x, scale.y * factor.y);
        /// @endcode
        ///
        /// @param factor Scale factors
        ///
        /// @see `SetScale`
        ////////////////////////////////////////////////////////////
        void Scale(const sf::Vector2f& factor);

        ////////////////////////////////////////////////////////////
        /// @brief Get the combined transform of the object
        ///
        /// @return Transform combining the position/rotation/scale/origin of the object
        ///
        /// @see `GetInverseTransform`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Transform& GetTransform() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the inverse of the combined transform of the object
        ///
        /// @return Inverse of the combined transformations applied to the object
        ///
        /// @see `GetTransform`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Transform& GetInverseTransform() const;

    private:
        ////////////////////////////////////////////////////////////
        // Member data
        ////////////////////////////////////////////////////////////
        sf::Vector2f          m_origin;                           //!< Origin of translation/rotation/scaling of the object
        sf::Vector2f          m_position;                         //!< Position of the object in the 2D world
        sf::Angle             m_rotation;                         //!< Orientation of the object
        sf::Vector2f          m_scale{1, 1};                  //!< Scale of the object
        mutable sf::Transform m_transform;                        //!< Combined transformation of the object
        mutable sf::Transform m_inverseTransform;                 //!< Combined transformation of the object
        mutable bool          m_transformNeedUpdate{true};        //!< Does the transform need to be recomputed?
        mutable bool          m_inverseTransformNeedUpdate{true}; //!< Does the transform need to be recomputed?
    };

}
