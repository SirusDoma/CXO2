#ifndef O2JAM_STATE_HPP
#define O2JAM_STATE_HPP

#include <Genode/SceneGraph/Scene.hpp>
#include <Genode/UI/Dialog.hpp>

#include <memory>

enum ResourceScope
{
    Immediate,
    Local,
    Shared
};


class State : public virtual Gx::Scene
{
public:
    State();
    State(State &state);
    explicit State(const std::string& name);
    State(const std::string& name, Gx::ResourceManager &resources);

    ~State() override;

    template<typename R>
    R* Load(const std::string &source, ResourceScope scope = ResourceScope::Local);

    template<typename R>
    R* Import(const std::string &id, Gx::ResourcePtr<R> resource, ResourceScope scope = ResourceScope::Local);

    template<typename R, class... Args, std::enable_if_t<!std::is_array_v<R>, int> = 0>
    R* Create(Args&&... args);

    template<typename R, class... Args, std::enable_if_t<!std::is_array_v<R>, int> = 0>
    R* Make(const std::string &id, Args&&... args);

    template<typename R>
    R *Instantiate(const std::string &id, ResourceScope scope = ResourceScope::Local);

    template<typename R>
    R *Instantiate(const std::string &source, const std::string &id, ResourceScope scope = ResourceScope::Local);

    template<typename R>
    R* Find(const std::string &id, ResourceScope scope = ResourceScope::Local);

    Gx::ResourceManager &GetLocalResources() const;
    bool Close(bool quit) override;

protected:
    void Initialize() override;

private:
    void LoadCommonResources();

    std::unique_ptr<Gx::ResourceManager> m_resources;
    std::unique_ptr<Gx::ResourceManager> m_tempResources;

    inline static Gx::Dialog* m_dialog;
    inline static sf::Sound*  m_popupSound, *m_cancelSound;
    inline static bool m_prompted = false;
};

#include <OTwo/States/State.inl>
#endif