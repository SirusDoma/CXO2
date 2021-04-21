#ifndef GENODE_SCENEGRAPH_SCENE_HPP
#define GENODE_SCENEGRAPH_SCENE_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Window/Event.hpp>

#include <Genode/Audio/Mixer.hpp>
#include <Genode/SceneGraph/Node.hpp>
#include <Genode/SceneGraph/RenderableContainer.hpp>
#include <Genode/SceneGraph/UpdatableContainer.hpp>
#include <Genode/SceneGraph/InputableContainer.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/Tasks/TaskContainer.hpp>

#include <queue>
#include <Genode/System/Application.hpp>

namespace Gx
{
    class SceneDirector;
    class Scene : public virtual Node, public RenderableContainer, public virtual UpdatableContainer, public InputableContainer, public TaskContainer
    {
    public:
        friend SceneDirector;

        Scene();
        Scene(const std::string& name);
        Scene(ResourceManager& resources);
        Scene(const std::string& name, ResourceManager& resources);

        virtual ~Scene();

        Application &GetApplication() const;
        SceneDirector &GetDirector() const;
        sf::View GetView() const;

        template<typename R>
        R* Create(const std::string &source, ResourceScope scope = ResourceScope::Local);

        template<typename R>
        R& RegisterLocalResource();

        bool Destroy(Node* node);
        bool Destroy(sf::SoundSource *source);

        ResourceManager &GetLocalResources() const;
        virtual ResourceManager &GetSharedResources() const;

        void PushOverlay(Node *overlay);
        void CloseOverlay();

        void QueueSceneEvent(std::function<void()> evt);

    protected:
        using EntityContainer      = std::vector<ResourcePtr<Node>>;
        using SoundSourceContainer = std::vector<ResourcePtr<sf::SoundSource>>;

        virtual void Initialize();
        virtual bool Close(bool quit = false);

        virtual sf::RenderStates Render(sf::RenderTarget &target, sf::RenderStates states) const;
        virtual void Update(double delta);
        virtual bool Input(sf::Event ev);

        virtual void ProcessSceneEvents();

    private:
        template<typename R>
        R* CreateNode(const std::string &source, ResourceScope scope = ResourceScope::Local);

        mutable sf::View m_view;
        std::unique_ptr<ResourceManager> m_resources;

        EntityContainer      m_entities;
        SoundSourceContainer m_sources;

        SceneDirector     *m_director;
        std::vector<Node*> m_overlays;

        sf::Event m_lastInput;
        std::queue<std::function<void()>> m_events;

        void SetDirector(SceneDirector& director);
        void SetView(const sf::View &view);
    };
}

#include <Genode/SceneGraph/Scene.inl>
#endif
