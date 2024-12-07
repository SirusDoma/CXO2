#pragma once

namespace Gx
{
    template<typename T>
    T* Node::GetParent() const
    {
        auto node = GetParent();
        T* parent = nullptr;

        while (node)
        {
            parent = dynamic_cast<T*>(node);
            if (parent)
                return parent;

            node = node->GetParent();
        }

        return nullptr;
    }

    template<typename T>
    T* Node::FindChild(const std::string& name) const
    {
        auto child = GetChildByName(name);
        if (!child)
            child = GetChildByName(GetName() + "/" + name);

        return dynamic_cast<T*>(child);
    }

    template<typename... Nodes>
    std::enable_if_t<std::conjunction_v<std::is_base_of<Node, std::remove_reference_t<Nodes>>...>, void>
    Node::AddChild(Nodes&... nodes)
    {
        (AddChild(static_cast<Node&>(nodes)), ...);
    }

    template<typename... Nodes>
    std::enable_if_t<std::conjunction_v<std::is_base_of<Node, std::remove_reference_t<Nodes>>...>, void>
    Node::RemoveChild(Nodes&... nodes)
    {
        (RemoveChild(static_cast<Node&>(nodes)), ...);
    }
}