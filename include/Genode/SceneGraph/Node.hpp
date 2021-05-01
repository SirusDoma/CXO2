#ifndef GENODE_SCENEGRAPH_NODE_HPP
#define GENODE_SCENEGRAPH_NODE_HPP

#include <string>
#include <vector>
#include <memory>

#include <Genode/Graphics/Transformable.hpp>

namespace Gx
{
    class Scene;
    class Node : public Transformable
    {
    public:
        virtual ~Node();

        std::string GetName() const;
        void SetName(const std::string& name);

        std::string GetTag() const;
        void SetTag(const std::string& tag);

        Scene* GetScene() const;
        Node* GetParent() const;
        std::vector<Node*> GetChildren() const;
        std::vector<Node*> GetChildrenByTag(const std::string& tag) const;
        Node* GetChildByName(const std::string& name) const;
        Node* GetChildByTag(const std::string& tag) const;

        virtual void AddChild(Node* child);
        virtual void RemoveChild(Node* child);
        virtual void ClearChildren();

        template<typename... Args>
        void AddChild(Node* first, Args... args);

        template<typename... Args>
        void RemoveChild(Node* first, Args... args);

        protected:
            Node();
            virtual void SetParent(Node* node);
            virtual void Initialize();

        private:
            Node* m_parent;
            std::string m_name;
            std::string m_tag;
            std::vector<Node*> m_children;
    };
}

#include <Genode/SceneGraph/Node.inl>
#endif