#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

#include <Genode/Graphics/Transformable.hpp>

namespace Gx
{
    class Node : public Transformable
    {
    public:
        ~Node() override = default;

        Node(const Node& other);
        Node& operator=(const Node& other);

        Node(Node&&) noexcept = default;
        Node& operator=(Node&&) = default;

        const std::string& GetName() const;
        void SetName(const std::string& name);

        const std::string& GetTag() const;
        void SetTag(const std::string& tag);

        Node* GetParent() const;
        std::vector<Node*> GetChildren() const;
        std::vector<Node*> GetChildrenByTag(const std::string& tag) const;
        Node* GetChildByName(const std::string& name) const;
        Node* GetChildByTag(const std::string& tag) const;
        std::size_t GetChildrenCount() const;

        void AddChild(Node& child);
        void RemoveChild(Node& child);
        void ClearChildren();

        template<typename T>
        T* GetParent() const;

        template<typename T>
        T* FindChild(const std::string& name) const;

        template<typename... Nodes>
        std::enable_if_t<std::conjunction_v<std::is_base_of<Node, std::remove_reference_t<Nodes>>...>, void>
        AddChild(Nodes&... nodes);

        template<typename... Nodes>
        std::enable_if_t<std::conjunction_v<std::is_base_of<Node, std::remove_reference_t<Nodes>>...>, void>
        RemoveChild(Nodes&... nodes);

    protected:
        Node() = default;

        virtual void Initialize();
        virtual void Finalize();

        virtual void OnChildAdded(Node& node);
        virtual void OnChildRemove(Node& node);

    private:
        enum class State
        {
            Unintialized,
            Initialized,
            Finalized
        };

        Node* m_parent{nullptr};
        State m_state{State::Unintialized};
        std::string m_name{};
        std::string m_tag{};
        std::vector<Node*> m_children{};
        mutable std::unordered_set<Node*> m_pending{};
    };
}

#include <Genode/SceneGraph/Node.inl>
