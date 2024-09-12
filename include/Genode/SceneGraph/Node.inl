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

    template<typename... Args>
    void Node::AddChild(Node* first, Args... args)
    {
        AddChild(first);
        AddChild(args...);
    }

    template<typename... Args>
    void Node::RemoveChild(Node* first, Args... args)
    {
        RemoveChild(first);
        RemoveChild(args...);
    }
}