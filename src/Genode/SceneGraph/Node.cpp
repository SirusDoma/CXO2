#include <algorithm>
#include <cstdarg>

#include <Genode/SceneGraph/Node.hpp>

namespace Gx
{
    Node::Node() :
        m_parent(nullptr)
    {
    }

    Node::~Node()
    {
        m_children.clear();
    }

    std::string Node::GetName() const
    {
        return m_name;
    }

    void Node::SetName(const std::string &name)
    {
        m_name = name;
    }

    std::string Node::GetTag() const
    {
        return m_tag;
    }

    void Node::SetTag(const std::string &tag)
    {
        m_tag = tag;
    }

    Node *Node::GetParent() const
    {
        return m_parent;
    }

    void Node::SetParent(Node* node)
    {
        m_parent = node;
    }

    std::vector<Node*> Node::GetChildren() const
    {
        std::vector<Node*> children = std::vector<Node*>(m_children.size());
        for (auto& child : m_children)
            children.push_back(child.get());

        return children;
    }

    std::vector<Node*> Node::GetChildrenByTag(const std::string &tag) const
    {
        auto nodes = std::vector<Node*>();
        for (auto& node : m_children)
        {
            if (node->m_tag == tag)
                nodes.push_back(node.get());
        }

        return nodes;
    }

    Node *Node::GetChildByName(const std::string &name) const
    {
        for (auto& node : m_children)
        {
            if (node->m_name == name)
                return node.get();
        }

        return nullptr;
    }

    Node *Node::GetChildByTag(const std::string &tag) const
    {
        for (auto& node : m_children)
        {
            if (node->m_tag == tag)
                return node.get();
        }

        return nullptr;
    }

    void Node::AddChild(Node* child)
    {
        if (child)
        {
            child->m_parent = this;
            m_children.push_back(std::shared_ptr<Node>(child));
        }
    }

    void Node::RemoveChild(Node *child)
    {
        if (child)
        {
            auto iterator = std::find_if(m_children.begin(), m_children.end(), [child](auto node) { return child == node.get(); });
            if (iterator != m_children.end())
            {
                child->m_parent = nullptr;
                m_children.erase(iterator);
            }
        }
    }
}
