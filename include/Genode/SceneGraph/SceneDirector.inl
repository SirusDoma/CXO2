#pragma once

#include <Genode/System/Exception.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/Utilities/StringHelper.hpp>

namespace Gx
{
    template<typename Ctx>
    SceneDirector::ScenePresentationData::ScenePresentationData(
        const std::type_index type,
        const std::function<void()>& initializer,
        const Ctx& context,
        const SceneDeserializer<Scene>& deserializer
    ) :
        Type(type),
        Initializer(initializer),
        Context(std::make_shared<Ctx>(context)),
        Deserializer(deserializer)
    {
        Context->Unbind();
    }

    template<typename T, typename... Args>
    SceneDirector::SceneDirector(RenderSurface& surface, T& scene, Args&&... args) :
        SceneDirector::SceneDirector(surface)
    {
        Present<T>(scene, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    SceneDirector::SceneDirector(Application& app, T& scene, Args&&... args) :
        SceneDirector::SceneDirector(app)
    {
        Present<T>(scene, std::forward<Args>(args)...);
    }

    template<typename T>
    std::enable_if_t<std::is_base_of_v<Scene, T>, void>
    SceneDirector::Register()
    {
        m_deserializers[typeid(T)] = [this] (const ResourceContext&) -> ResourcePtr<T> { return GetContext().Create<T>(); };
    }

    template<typename T>
    std::enable_if_t<std::is_base_of_v<Scene, T>, void>
    SceneDirector::Register(const SceneDeserializer<T>& deserializer)
    {
        m_deserializers[typeid(T)] = deserializer;
    }

    template<typename T, typename... Args>
    std::enable_if_t<std::is_base_of_v<Scene, T>, void>
    SceneDirector::Present(T& scene, Args&&... args)
    {
        m_deserializers[typeid(T)] = [&scene] (const ResourceContext&) -> ResourcePtr<T>
        {
            return std::unique_ptr<T>(&scene, [] (auto) {});
        };

        return Present<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename Ctx, typename... Args>
    std::enable_if_t<std::is_base_of_v<Scene, T> && std::is_base_of_v<ResourceContext, Ctx>, void>
    SceneDirector::Present(const Ctx& context, Args&&... args)
    {
        ResourcePtr<T> scene = nullptr;
        SceneDeserializer<T> deserializer = nullptr;

        if (const auto it = m_deserializers.find(typeid(T)); it != m_deserializers.end())
        {
            if (it->second.type() == typeid(SceneDeserializer<T>))
            {
                deserializer = std::any_cast<SceneDeserializer<T>>(it->second);
                scene        = deserializer(context);
            }
        }

        if (!scene)
        {
            if constexpr (std::is_default_constructible_v<T>)
            {
                Register<T>();
                return Present<T>(context, std::forward<Args>(args)...);
            }
        }

        if (!scene)
            throw ArgumentException(StringHelper::GetTypeName(typeid(T)) + " is not registered");

        if (T::IsTrackable())
        {
            m_initializer = nullptr;
            if constexpr(sizeof...(Args) > 0)
            {
                m_initializer = [=, target = scene.get()]
                {
                    target->Initialize(args...);
                };
            }

            m_stack.emplace(typeid(T), m_initializer, context, [deserializer] (const ResourceContext& ctx)
            {
                return Cast<Scene>(std::move(deserializer(ctx)));
            });
        }

        m_nextScene = Cast<Scene>(std::move(scene));
        Unstage();
    }

    template<typename T, typename... Args>
    std::enable_if_t<
        std::is_base_of_v<Scene, T> &&
        (sizeof...(Args) == 0 || (
            !std::is_base_of_v<ResourceContext, std::decay_t<std::tuple_element_t<0, std::tuple<Args..., void>>>> &&
            !std::is_base_of_v<T, std::decay_t<std::tuple_element_t<0, std::tuple<Args..., void>>>>
        )),
    void>
    SceneDirector::Present(Args&&... args)
    {
        Present<T>(ResourceContext::Default, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    std::enable_if_t<std::is_base_of_v<Scene, T>, bool>
    SceneDirector::Dismiss(const ResourceContext& context, Args&&... args)
    {
        if (m_stack.empty())
            return false;

        auto stack = m_stack;
        stack.pop();

        while (!stack.empty() && stack.top().Type != typeid(T))
            stack.pop();

        if (stack.empty())
            return false;

        m_stack = std::move(stack);
        const auto& presentation = m_stack.top();

        auto scene    = presentation.Deserializer(context);
        m_initializer = presentation.Initializer;
        if constexpr(sizeof...(Args) > 0)
        {
            m_initializer = [=, target = dynamic_cast<T*>(scene.get())]
            {
                target->Initialize(args...);
            };
        }

        m_nextScene = std::move(scene);
        Unstage();

        return true;
    }

    template<typename T, typename... Args>
    std::enable_if_t<
        std::is_base_of_v<Scene, T> &&
        (
            sizeof...(Args) == 0 ||
            !std::is_base_of_v<ResourceContext, std::decay_t<std::tuple_element_t<0, std::tuple<Args..., void>>>>
        ),
    bool>
    SceneDirector::Dismiss(Args&&... args)
    {
        if (m_stack.empty())
            return false;

        auto stack = m_stack;
        stack.pop();

        while (!stack.empty() && stack.top().Type != typeid(T))
            stack.pop();

        if (stack.empty())
            return false;

        m_stack = std::move(stack);
        const auto& presentation = m_stack.top();

        ResourceContext* context = nullptr;
        if (presentation.Context)
        {
            context = presentation.Context.get();
            context->Unbind();
        }

        auto scene    = presentation.Deserializer(context ? *context : ResourceContext::Default);
        m_initializer = presentation.Initializer;
        if constexpr(sizeof...(Args) > 0)
        {
            m_initializer = [=, target = dynamic_cast<T*>(scene.get())]
            {
                target->Initialize(args...);
            };
        }

        m_nextScene = std::move(scene);
        Unstage();

        return true;
    }

    template<typename T>
    bool SceneDirector::IsPresenting() const
    {
        return m_currentScene && typeid(*m_currentScene.get()) == typeid(T);
    }
}
