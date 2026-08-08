#include <Ui.hpp>

namespace O2CX::Ui
{
    Gx::Node* FindDescendant(const Gx::Node& root, const std::string& name)
    {
        if (const auto child = root.GetChildByName(name))
            return child;

        for (const auto child : root.GetChildren())
        {
            if (!child)
                continue;

            if (const auto descendant = FindDescendant(*child, name))
                return descendant;
        }

        return nullptr;
    }

    sf::Vector2f GlobalPosition(const Gx::Node& node)
    {
        auto position = node.GetPosition();
        for (auto parent = node.GetParent(); parent; parent = parent->GetParent())
            position += parent->GetPosition();

        return position;
    }

    sf::String ToLower(const sf::String& string)
    {
        auto result = string;
        for (auto& character : result)
        {
            if (character >= 'A' && character <= 'Z')
                character += 32;
        }

        return result;
    }
}
