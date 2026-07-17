#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>

#include <Genode/IO/FileSystem.hpp>

#include <vector>

namespace Cx
{
    SceneComposer::SceneComposer(Gx::Node* target, State* state, const bool instantiate) :
        m_target(target),
        m_state(state),
        m_instantiate(instantiate)
    {
    }

    SceneComposer SceneComposer::Compose(State& target, const bool instantiate)
    {
        return SceneComposer(&target, &target, instantiate);
    }

    SceneComposer SceneComposer::Compose(Gx::Node& target)
    {
        return SceneComposer(&target, nullptr, true);
    }

    void SceneComposer::Register(const std::string& type, Handler handler)
    {
        m_handlers[type] = std::move(handler);
    }

    const std::string& SceneComposer::GetName() const
    {
        return m_target->GetName();
    }

    const Gx::Node& SceneComposer::GetNode() const
    {
        return m_state ? *m_state : *m_target;
    }

    void SceneComposer::Add(const std::string& name, const Gx::Json& json, Gx::ResourceContext& context)
    {
        try
        {
            const auto* data = &json;

            auto parsed = Gx::Json();
            if (json.type() == Gx::Json::value_t::string)
            {
                const auto stream = Gx::FileSystem::Open(json.get<std::string>());
                if (!stream)
                    return;

                const auto size = stream->getSize().value_or(0) - stream->tell().value_or(0);
                auto bytes = std::vector<std::uint8_t>(size);
                if (!stream->read(bytes.data(), size).has_value())
                    return;

                parsed = Gx::Json::parse(std::string(reinterpret_cast<const char*>(bytes.data()), size));
                data = &parsed;
            }

            auto type = std::string();
            if (const auto it = data->find("type"); it != data->end() && it->is_string())
                type = it->get<std::string>();

            if (type.empty())
                return;

            if (const auto handler = m_handlers.find(type); handler != m_handlers.end())
            {
                handler->second(name, json, *this, context);
                return;
            }

            Add<Gx::Node>(name, type, json, context);
        }
        catch (...)
        {
        }
    }
}
