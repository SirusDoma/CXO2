#pragma once

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Dialog.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Clock.hpp>

#include <memory>

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

        void ShowDialog(const sf::String& content, DialogStyle style, bool backdrop = false, std::function<void(bool)> callback = nullptr);
        void ShowDialog(Gx::Node& content, DialogStyle style, bool backdrop = false, std::function<void(bool)> callback = nullptr);

        static void Announce(const sf::String& content);

    protected:
        void Initialize() override;
        void Finalize() override;

        Gx::RenderStates Render(Gx::RenderSurface& surface, Gx::RenderStates states) const override;

        void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

        void ExitGame(const sf::String& prompt, sf::FloatRect bounds = {});

    private:
        void LoadCommonResources();

        template<typename R>
        R* Locate(Gx::ResourceManager& resources, const std::string& id);

        std::unique_ptr<Gx::ResourceManager> m_resources;
        std::unique_ptr<Gx::ResourceManager> m_tempResources;

        inline static Gx::Dialog* m_dialogInfo, *m_dialog1, *m_dialog2, *m_dialogNotice, *m_exitDialog;
        inline static sf::Sound* m_popupSound, *m_cancelSound;
        inline static bool m_exitPrompted = false;
        inline static sf::Clock m_noticeTimer{};
    };

}

#include <CXO2/States/State.inl>
