#pragma once

#include <any>
#include <Genode/Entities.hpp>
#include <Genode/System/Context.hpp>
#include <Genode/IO/Resource.hpp>

#include <SFML/Graphics.hpp>

#include <typeindex>
#include <stack>
#include <Genode/IO/ResourceContext.hpp>

namespace Gx
{
    class ResourceContext;
    template <typename T>
    using SceneDeserializer = std::function<ResourcePtr<T>(const ResourceContext&)>;

    class Scene;
    class Application;
    class SceneDirector final : public Renderable, public Updatable, public Inputable
    {
    public:
        explicit SceneDirector(Application& app);
        explicit SceneDirector(RenderSurface& surface);

        template<typename T, typename... Args>
        SceneDirector(RenderSurface& surface, T& scene, Args&&... args);

        template<typename T, typename... Args>
        SceneDirector(RenderSurface& surface, T&& scene, Args&&... args) = delete;

        template<typename T, typename... Args>
        SceneDirector(Application& app, T& scene, Args&&... args);

        template<typename T, typename... Args>
        SceneDirector(Application& app, T&& scene, Args&&... args) = delete;

        RenderStates Render(RenderSurface& surface, RenderStates states) const override;
        void Update(double delta) override;
        bool Input(const sf::Event& ev) override;
        void Focus(bool focused) const;
        void Resize(const sf::Vector2u& size) const;
        bool Close() const;

        Context& GetContext() const;
        Scene& GetPresentingScene() const;

        const sf::View& GetView() const;
        const sf::View& GetDefaultView() const;
        void SetView(const sf::View& view);

        template<typename T>
        bool IsPresenting() const;

        template<typename T, typename... Args>
        std::enable_if_t<std::is_base_of_v<Scene, T>, void>
        Present(T& scene, Args&&... args);

        template <typename T, typename Ctx, typename... Args>
        std::enable_if_t<std::is_base_of_v<Scene, T> && std::is_base_of_v<ResourceContext, Ctx>, void>
        Present(const Ctx& context, Args&&... args);

        template<typename T, typename... Args>
        std::enable_if_t<
            std::is_base_of_v<Scene, T> &&
            (sizeof...(Args) == 0 || (
                !std::is_base_of_v<ResourceContext, std::decay_t<std::tuple_element_t<0, std::tuple<Args..., void>>>> &&
                !std::is_base_of_v<T, std::decay_t<std::tuple_element_t<0, std::tuple<Args..., void>>>>
            )),
        void>
        Present(Args&&... args);

        template<typename T>
        void Present(T&& scene) = delete;

        template <typename T, typename... Args>
        std::enable_if_t<std::is_base_of_v<Scene, T>, bool>
        Dismiss(const ResourceContext& context, Args&&... args);

        template<typename T, typename... Args>
        std::enable_if_t<
            std::is_base_of_v<Scene, T> &&
            (
                sizeof...(Args) == 0 ||
                !std::is_base_of_v<ResourceContext, std::decay_t<std::tuple_element_t<0, std::tuple<Args..., void>>>>
            ),
        bool>
        Dismiss(Args&&... args);

        bool Dismiss();

        bool Dismiss(const ResourceContext& context);

        void ProcessEvents() const;

        void Reset();

        Application& GetApplication() const;

        template<typename T>
        std::enable_if_t<std::is_base_of_v<Scene, T>, void>
        Register();

        template<typename T>
        std::enable_if_t<std::is_base_of_v<Scene, T>, void>
        Register(const SceneDeserializer<T>& deserializer);

    private:
        struct ScenePresentationData
        {
            std::type_index                   Type;
            std::function<void()>             Initializer{nullptr};
            std::shared_ptr<ResourceContext>  Context{nullptr};
            SceneDeserializer<Scene>          Deserializer{nullptr};

            template<typename Ctx>
            ScenePresentationData(
                std::type_index type,
                const std::function<void()>& initializer,
                const Ctx& context,
                const SceneDeserializer<Scene>& deserializer
            );
        };

        using SceneDeserializerMap     = std::unordered_map<std::type_index, std::any>;
        using SceneGenericDeserializer = std::function<ResourcePtr<Scene>(const ResourceContext&)>;
        using ScenePresentationStack   = std::stack<ScenePresentationData>;

        void Stage();
        void Unstage() const;

        RenderSurface&          m_surface;
        SceneDeserializerMap    m_deserializers{};
        ScenePresentationStack  m_stack{};
        ResourcePtr<Scene>      m_currentScene{};
        ResourcePtr<Scene>      m_nextScene{};
        std::function<void()>   m_initializer{};;
        mutable Context         m_context{};
        mutable bool            m_staged{false};
    };
}

#include <Genode/SceneGraph/SceneDirector.inl>
