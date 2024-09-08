#include <Genode/SceneGraph/Node.hpp>

#include <algorithm>

namespace Gx
{
    Node::Node() :
        Transformable(),
        m_parent(nullptr),
        m_initialized(false),
        m_pristine(true)
    {
    }

    void Node::Initialize()
    {
        m_pristine    = m_initialized;
        m_initialized = true;
    }

    const std::string &Node::GetName() const
    {
        return m_name;
    }

    void Node::SetName(const std::string &name)
    {
        m_name = name;
    }

    const std::string &Node::GetTag() const
    {
        return m_tag;
    }

    void Node::SetTag(const std::string &tag)
    {
        m_tag = tag;
    }

    Node* Node::GetParent() const
    {
        return m_parent;
    }

    void Node::SetParent(Node* node)
    {
        m_parent = node;
    }

    std::vector<Node*> Node::GetChildren() const
    {
        if (m_initialized && !m_pristine)
        {
            for (const auto n : m_children)
            {
                if (n->m_initialized)
                    continue;

                n->Initialize();
                n->m_initialized = true;
            }

            m_pristine = true;
        }

        return m_children;
    }

    std::vector<Node*> Node::GetChildrenByTag(const std::string &tag) const
    {
        auto nodes = std::vector<Node*>();
        for (auto& node : m_children)
        {
            if (node->m_tag == tag)
            {
                if (m_initialized && !node->m_initialized)
                {
                    node->Initialize();
                    node->m_initialized = true;
                }

                nodes.push_back(node);
            }
        }

        return nodes;
    }

    Node* Node::GetChildByName(const std::string &name) const
    {
        for (const auto& node : m_children)
        {
            if (node->m_name == name)
            {
                if (m_initialized && !node->m_initialized)
                {
                    node->Initialize();
                    node->m_initialized = true;
                }

                return node;
            }
        }

        return nullptr;
    }

    Node* Node::GetChildByTag(const std::string &tag) const
    {
        for (const auto& node : m_children)
        {
            if (node->m_tag == tag)
            {
                if (m_initialized && !node->m_initialized)
                {
                    node->Initialize();
                    node->m_initialized = true;
                }

                return node;
            }
        }

        return nullptr;
    }

    void Node::AddChild(Node* child)
    {
        if (child)
        {
            if (std::find(m_children.begin(), m_children.end(), child) == m_children.end())
            {
                if (child->m_parent)
                    child->m_parent->RemoveChild(child);

                child->SetParent(this);
                m_children.push_back(child);
            }
            else
                child->SetParent(this);

            if (m_initialized && !child->m_initialized)
            {
                child->Initialize();
                child->m_initialized = true;
            }
            else
                m_pristine = child->m_initialized;
        }
    }

    void Node::RemoveChild(Node* child)
    {
        if (child)
        {
            if (child->m_parent == this)
                child->m_parent = nullptr;

            const auto iterator = std::find(m_children.begin(), m_children.end(), child);
            if (iterator != m_children.end())
                m_children.erase(iterator);
        }
    }

    void Node::ClearChildren()
    {
        for (const auto child : m_children)
            child->m_parent = nullptr;

        m_children.clear();
    }
}
