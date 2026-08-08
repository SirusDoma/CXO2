#pragma once

#include <CXO2/States/State.hpp>

#include <Genode/SceneGraph/Node.hpp>

#include <SFML/System/String.hpp>

#include <string>

namespace O2CX::Ui
{
    Gx::Node* FindDescendant(const Gx::Node& root, const std::string& name);

    template <typename T>
    T* FindDescendant(const Gx::Node& root, const std::string& name)
    {
        return dynamic_cast<T*>(FindDescendant(root, name));
    }

    template <typename T>
    T* FindByIdentifier(const Gx::Node& root, const std::string& id)
    {
        if (const auto node = root.FindChild<T>(id))
            return node;

        const auto separator = id.rfind('/');
        return FindDescendant<T>(root, separator == std::string::npos ? id : id.substr(separator + 1));
    }

    sf::Vector2f GlobalPosition(const Gx::Node& node);

    sf::String ToLower(const sf::String& string);
}
