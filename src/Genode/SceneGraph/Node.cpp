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

    void Node::Initialize()
    {
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
        return m_children;
    }

    std::vector<Node*> Node::GetChildrenByTag(const std::string &tag) const
    {
        auto nodes = std::vector<Node*>();
        for (auto& node : m_children)
        {
            if (node->m_tag == tag)
                nodes.push_back(node);
        }

        return nodes;
    }

    Node *Node::GetChildByName(const std::string &name) const
    {
        for (auto& node : m_children)
        {
            if (node->m_name == name)
                return node;
        }

        return nullptr;
    }

    Node *Node::GetChildByTag(const std::string &tag) const
    {
        for (auto& node : m_children)
        {
            if (node->m_tag == tag)
                return node;
        }

        return nullptr;
    }

    void Node::AddChild(Node* child)
    {
        if (child && std::find(m_children.begin(), m_children.end(), child) == m_children.end())
        {
            child->m_parent = this;
            child->Initialize();

            m_children.push_back(child);
        }
    }

    void Node::RemoveChild(Node *child)
    {
        if (child)
        {
            if (child->m_parent == this)
                child->m_parent = nullptr;

            auto iterator = std::find(m_children.begin(), m_children.end(), child);
            if (iterator != m_children.end())
                m_children.erase(iterator);
        }
    }

    void Node::ClearChildren()
    {
        m_children.clear();
    }
}
