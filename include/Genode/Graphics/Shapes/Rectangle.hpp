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

#include <Genode/Graphics/Shape.hpp>

namespace Gx
{
    ////////////////////////////////////////////////////////////
    /// @brief Specialized shape representing a rectangle
    ////////////////////////////////////////////////////////////
    class Rectangle : public Shape
    {
    public:
        ////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /// @param size Size of the rectangle
        ////////////////////////////////////////////////////////////
        explicit Rectangle(const sf::Vector2f& size = sf::Vector2f(0, 0));

        ////////////////////////////////////////////////////////////
        /// @brief Set the size of the rectangle
        ///
        /// @param size New size of the rectangle
        ///
        /// @see `GetSize`
        ////////////////////////////////////////////////////////////
        void SetSize(const sf::Vector2f& size);

        ////////////////////////////////////////////////////////////
        /// @brief Get the size of the rectangle
        ///
        /// @return Size of the rectangle
        ///
        /// @see `SetSize`
        ////////////////////////////////////////////////////////////
        [[nodiscard]] const sf::Vector2f& GetSize() const;

        ////////////////////////////////////////////////////////////
        /// @brief Get the number of points defining the shape
        ///
        /// @return Number of points of the shape. For rectangle
        ///         shapes, this number is always 4.
        ////////////////////////////////////////////////////////////
        [[nodiscard]] std::size_t GetPointCount() const override;

        ////////////////////////////////////////////////////////////
        /// @brief Get a point of the rectangle
        ///
        /// The returned point is in local coordinates, that is,
        /// the shape's transforms (position, rotation, scale) are
        /// not taken into account.
        /// The result is undefined if \a `index` is out of the valid range.
        ///
        /// @param index Index of the point to get, in range [0 .. 3]
        ///
        /// @return index-th point of the shape
        ////////////////////////////////////////////////////////////
        [[nodiscard]] sf::Vector2f GetPoint(std::size_t index) const override;

    private:
        ////////////////////////////////////////////////////////////
        // Member data
        ////////////////////////////////////////////////////////////
        sf::Vector2f m_size; //!< Size of the rectangle
    };

}
