namespace Gx
{
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