#pragma once

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Dialog.hpp>
#include <Genode/IO/ResourceManager.hpp>

#include <memory>

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

    template<typename R>
    R* Instantiate(const std::string& source, ResourceScope scope = ResourceScope::Local);

    template<typename R>
    R* Instantiate(const R& prefab, ResourceScope scope = ResourceScope::Immediate);

    template<typename R>
    R* Import(Gx::ResourcePtr<R> resource, ResourceScope scope = ResourceScope::Local);

    template<typename R>
    R* Import(const std::string& id, Gx::ResourcePtr<R> resource, ResourceScope scope = ResourceScope::Local);

    template<typename R, class... Args, std::enable_if_t<!std::is_array_v<R>, int> = 0>
    R& Create(Args&&... args);

    template<typename R>
    R* FindResource(const std::string& id, ResourceScope scope = ResourceScope::Local);

    Gx::ResourceManager& GetResources(ResourceScope scope = ResourceScope::Local);
    bool OnAppClose() override;

protected:
    void Initialize() override;
    void Finalize() override;

    void ShowDialog(const std::string& content, DialogStyle style, bool backdrop = false, std::function<void(bool)> callback = nullptr);
    void ShowDialog(Gx::Node& content, DialogStyle style, bool backdrop = false, std::function<void(bool)> callback = nullptr);

    void OnKeyPressed(const sf::Event::KeyPressed& ev) override;

private:
    void LoadCommonResources();

    std::unique_ptr<Gx::ResourceManager> m_resources;
    std::unique_ptr<Gx::ResourceManager> m_tempResources;

    inline static Gx::Dialog* m_dialogInfo, *m_dialog1, *m_dialog2, *m_exitDialog;
    inline static sf::Sound* m_popupSound, *m_cancelSound;
    inline static bool m_prompted = false;
};

#include <OTwo/States/State.inl>
