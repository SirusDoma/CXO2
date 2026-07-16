#pragma once

#include <CXO2/States/State.hpp>

#include <Genode/IO/Json.hpp>
#include <Genode/IO/Resource.hpp>
#include <Genode/IO/ResourceContext.hpp>
#include <Genode/SceneGraph/Node.hpp>

#include <functional>
#include <string>
#include <unordered_map>

namespace Cx
{
    class SceneComposer
    {
    public:
        using Handler = std::function<void(const std::string&, const Gx::Json&, SceneComposer&, Gx::ResourceContext&)>;

        static SceneComposer Compose(State& target, bool instantiate = true);
        static SceneComposer Compose(Gx::Node& target);

        static void Register(const std::string& type, Handler handler);

        const std::string& GetName() const;
        const Gx::Node& GetNode() const;

        void Add(const std::string& name, const Gx::Json& json, Gx::ResourceContext& context);

        template<typename R>
        void Add(const std::string& name, const Gx::Json& json, Gx::ResourceContext& context);

        template<typename R>
        void Add(const std::string& name, const std::string& type, const Gx::Json& json, Gx::ResourceContext& context);

    private:
        SceneComposer(Gx::Node* target, State* state, bool instantiate);

        template<typename R>
        void Place(const std::string& name, Gx::ResourcePtr<R> object, Gx::ResourceContext& context);

        Gx::Node* m_target;
        State*    m_state;
        bool      m_instantiate;

        inline static std::unordered_map<std::string, Handler> m_handlers;
    };

}

#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.inl>
