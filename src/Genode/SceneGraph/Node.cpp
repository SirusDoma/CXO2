#include <Genode/SceneGraph/Node.hpp>
#include <Genode/System/Exception.hpp>

#include <fmt/format.h>
#include <algorithm>
#include <regex>
#include <string>
#include <Genode/Utilities/StringHelper.hpp>

namespace Gx
{
    Node::Node(const Node& other) :
        Transformable(other),
        m_state(other.m_state),
        m_name(other.m_name),
        m_tag(other.m_tag)
    {
        if (other.GetChildrenCount() > 0)
            throw InvalidOperationException("Cannot copy a Node with children.");
    }

    Node& Node::operator=(const Node& other)
    {
        if (this != &other)
        {
            Transformable::operator=(other);
            m_parent = nullptr;
            m_state  = other.m_state;
            m_name   = other.m_name;
            m_tag    = other.m_tag;

            if (other.GetChildrenCount() > 0)
                throw InvalidOperationException("Cannot copy a Node with children.");
        }

        return *this;
    }

    void Node::Initialize()
    {
        m_state = State::Initialized;
    }

    void Node::Finalize()
    {
        m_state = State::Finalized;
        for (const auto child : m_children)
        {
            if (child)
                child->Finalize();
        }
    }

    void Node::OnChildAdded(Node& node)
    {
    }

    void Node::OnChildRemove(Node& node)
    {
    }

    bool Node::Match(const std::string& id, const std::string& pattern)
    {
        const static std::unordered_set reserved =
        {
            '.', '^', '$', '*', '+', '?', '(', ')', '[', ']', '{', '}', '\\', '|'
        };

        std::string rxp;
        for (const char& ch : pattern)
        {
            if (ch == '*')
                rxp += "[^/]*";
            else if (ch == '?')
                rxp += "[^/]";
            else if (reserved.count(ch))
                rxp += fmt::format("\\{}", ch);
            else
                rxp += ch;
        }

        const std::regex regex(fmt::format("^{}$", rxp));
        return std::regex_match(id, regex);
    }

    bool Node::Match(const Node& node, const std::string& pattern)
    {
        return Match(node.m_name, pattern) ||
            (node.m_parent && Match(node.m_name, fmt::format("{}/{}", node.m_parent->m_name, pattern))) ||
            (pattern.find("/") == std::string::npos && StringHelper::EndsWith(node.m_name, pattern));
    }

    bool Node::Match(const std::string& pattern) const
    {
        return Match(*this, pattern);
    }

    const std::string& Node::GetName() const
    {
        return m_name;
    }

    void Node::SetName(const std::string& name)
    {
        m_name = name;
    }

    const std::string& Node::GetTag() const
    {
        return m_tag;
    }

    void Node::SetTag(const std::string& tag)
    {
        m_tag = tag;
    }

    Node* Node::GetParent() const
    {
        return m_parent;
    }

    std::vector<Node*> Node::GetChildren() const
    {
        if (m_state == State::Initialized && !m_pending.empty())
        {
            for (auto& child : m_pending)
            {
                if (child->m_state == State::Initialized)
                    continue;

                child->Initialize();
                child->m_state = State::Initialized;
            }

            m_pending.clear();
        }

        return m_children;
    }

    std::vector<Node*> Node::GetChildrenByTag(const std::string& tag) const
    {
        auto nodes = std::vector<Node*>();
        for (auto& child : m_children)
        {
            if (child->m_tag == tag)
            {
                if (m_state == State::Initialized && child->m_state != State::Initialized)
                {
                    child->Initialize();
                    child->m_state = State::Initialized;

                    m_pending.erase(child);
                }

                nodes.push_back(child);
            }
        }

        return nodes;
    }

    Node* Node::GetChildByName(const std::string& name) const
    {
        for (const auto& child : m_children)
        {
            if (child->Match(name))
            {
                if (m_state == State::Initialized && child->m_state != State::Initialized)
                {
                    child->Initialize();
                    child->m_state = State::Initialized;

                    m_pending.erase(child);
                }

                return child;
            }
        }

        return nullptr;
    }

    Node* Node::GetChildByTag(const std::string& tag) const
    {
        for (const auto& child : m_children)
        {
            if (child->m_tag == tag)
            {
                if (m_state == State::Initialized && child->m_state != State::Initialized)
                {
                    child->Initialize();
                    child->m_state = State::Initialized;

                    m_pending.erase(child);
                }

                return child;
            }
        }

        return nullptr;
    }

    std::size_t Node::GetChildrenCount() const
    {
        return m_children.size();
    }

    void Node::AddChild(Node& child)
    {
        if (std::find(m_children.begin(), m_children.end(), &child) == m_children.end())
        {
            if (child.m_parent)
                child.m_parent->RemoveChild(child);

            child.m_parent = this;
            m_children.push_back(&child);
        }
        else
            child.m_parent = this;

        if (m_state == State::Initialized && child.m_state != State::Initialized)
        {
            child.Initialize();
            child.m_state = State::Initialized;
        }
        else
            m_pending.insert(&child);

        // TODO: Not guaranteed to be initialized?
        OnChildAdded(child);
    }

    void Node::RemoveChild(Node& child)
    {
        if (child.m_parent == this)
            child.m_parent = nullptr;

        const auto iterator = std::find(m_children.begin(), m_children.end(), &child);
        if (iterator != m_children.end())
        {
            if (child.m_state != State::Finalized)
            {
                OnChildRemove(child);

                child.Finalize();
                child.m_state = State::Finalized;
            }

            m_pending.erase(*iterator);
            m_children.erase(iterator);
        }
    }

    void Node::ClearChildren()
    {
        for (const auto child : m_children)
        {
            child->m_parent = nullptr;

            if (child->m_state != State::Finalized)
            {
                OnChildRemove(*child);

                child->Finalize();
                child->m_state = State::Finalized;
            }
        }

        m_pending.clear();
        m_children.clear();
    }
}
