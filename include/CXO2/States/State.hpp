#pragma once

#include <CXO2/Events/StateEvents.hpp>
#include <CXO2/States/StateExtension.hpp>

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Dialog.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Clock.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace Cx
{
    enum class ResourceScope
    {
        Immediate,
        Local,
        Shared
    };

    enum class DialogStyle
    {
        Information,
        OkCancel,
        YesNo
    };

    class State : public virtual Gx::Scene
    {
    public:
        State();
        explicit State(const std::string& name);
        State(const std::string& name, Gx::ResourceManager& resources);

        template<typename R, std::enable_if_t<std::is_base_of_v<Gx::Node, R> || std::is_base_of_v<sf::SoundSource, R>, int> = 0>
        R* Instantiate(const std::string& source, ResourceScope scope = ResourceScope::Local);

        template<typename R, std::enable_if_t<std::is_base_of_v<Gx::Node, R>, int> = 0>
        R* Instantiate(const R& prefab, ResourceScope scope = ResourceScope::Immediate);

        template<typename R, std::enable_if_t<std::is_base_of_v<Gx::Node, R>, int> = 0>
        R* Import(Gx::ResourcePtr<R> resource, ResourceScope scope = ResourceScope::Local);

        template<typename R>
        R* Import(const std::string& id, Gx::ResourcePtr<R> resource, ResourceScope scope = ResourceScope::Local);

        template<typename R, class... Args, std::enable_if_t<!std::is_array_v<R>, int> = 0>
        R& Create(Args&&... args);

        template<typename R>
        R* Find(const std::string& id, ResourceScope scope = ResourceScope::Local);

        Gx::ResourceManager& GetResources(ResourceScope scope = ResourceScope::Local);
        bool OnAppClose() override;

        virtual void ShowDialog(const sf::String& content, DialogStyle style, std::function<void(bool)> callback = nullptr);
        virtual void ShowDialog(const sf::String& content, DialogStyle style, bool persist, bool backdrop, std::function<void(bool)> callback = nullptr);
        virtual void ShowDialog(Gx::Node& content, DialogStyle style, bool backdrop = false, std::function<void(bool)> callback = nullptr);

        template<typename T, typename... Args, std::enable_if_t<std::is_base_of_v<StateExtension, T>, int> = 0>
        T& AddExtension(Args&&... args);

        virtual StateExtension& Attach(StateExtensionPtr extension);

        static void Announce(const sf::String& content);

    protected:
        bool Initialize(StateEventArgs&& args);
        void Finalize() override;

        template <typename TKey, typename TSender, typename... TArgs, typename... UArgs>
        bool Dispatch(const Gx::Event<TKey, TSender, TArgs...>& event, UArgs&&... args);

        template <typename TKey, typename TSender, typename... TArgs, typename... UArgs>
        bool Dispatch(const Gx::Event<TKey, TSender, TArgs...>& event, UArgs&&... args) const;

        void Update(const sf::Time& delta) override;
        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
        bool Input(const sf::Event& ev) override;

        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

        void ExitGame(const sf::String& prompt, sf::FloatRect bounds = {});

    private:
        struct PersistentDialog
        {
            sf::String                Content;
            DialogStyle               Style{};
            bool                      Backdrop{};
            std::function<void(bool)> Callback;
        };

        class ExtensionTerminal final : public StateExtension
        {
        public:
            explicit ExtensionTerminal(State& owner);

        protected:
            bool Initialize() override;
            void Finalize() override;
            void Update(const sf::Time& delta) override;
            Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;
            bool Input(const sf::Event& ev) override;

        private:
            State& m_owner;
        };

        bool InternalInitialize();
        void InternalFinalize();
        void InternalUpdate(const sf::Time& delta);
        Gx::RenderStates InternalRender(Gx::RenderSurface& surface, Gx::RenderStates states) const;
        bool InternalInput(const sf::Event& ev);

        StateExtension& GetNextExtension();
        const StateExtension& GetNextExtension() const;

        void LoadCommonResources();

        static Gx::Dialog* GetDialog(DialogStyle style);

        template<typename R>
        R* Locate(Gx::ResourceManager& resources, const std::string& id);

        std::unique_ptr<Gx::ResourceManager> m_resources;
        std::unique_ptr<Gx::ResourceManager> m_tempResources;

        ExtensionTerminal m_terminal{*this};
        std::vector<StateExtensionPtr> m_extensions;

        inline static Gx::Dialog* m_dialogInfo, *m_dialog1, *m_dialog2, *m_dialogNotice, *m_exitDialog;
        inline static sf::Sound* m_popupSound, *m_cancelSound;
        inline static bool m_exitPrompted = false;
        inline static sf::Clock m_noticeTimer{};
        inline static std::optional<PersistentDialog> m_persistentDialog{};
    };

}

#include <CXO2/States/State.inl>
