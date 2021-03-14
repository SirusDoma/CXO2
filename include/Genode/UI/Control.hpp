#ifndef GENODE_UIELEMENT_HPP
#define GENODE_UIELEMENT_HPP

#include <Genode/Entities.hpp>
#include <Genode/SceneGraph/Node.hpp>

namespace Gx
{
    class Control : public Node, public Renderable, public Updatable, public Inputable
    {
    public:
        virtual ~Control();

        virtual void AddChild(Node *child);

        void SetEnabled(bool enabled);
        const bool& IsEnabled() const;

        void SetVisible(bool visible);
        const bool& IsVislble() const;

    protected:
        Control();

    private:
        bool m_enabled, m_visible;
    };
}

#endif
