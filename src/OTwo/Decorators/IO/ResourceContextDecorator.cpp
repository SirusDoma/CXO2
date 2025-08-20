#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <fmt/args.h>

ResourceContextDecorator ResourceContextDecorator::Decorate(const Gx::ResourceContext& ctx)
{
    return ResourceContextDecorator(ctx);
}

ResourceContextDecorator ResourceContextDecorator::Decorate(const Gx::ResourceContext& ctx, const ResourceMetadata& parent)
{
    return ResourceContextDecorator(ctx, parent);
}

ResourceContextDecorator ResourceContextDecorator::Rebind(const Gx::ResourceContext& ctx, const std::string& name)
{
    if (const auto decorated = dynamic_cast<const ResourceContextDecorator*>(&ctx); decorated && decorated->m_parent)
        return ResourceContextDecorator(Gx::ResourceContext::Rebind(ctx, name), *decorated->m_parent);

    return ResourceContextDecorator(Gx::ResourceContext::Rebind(ctx, name));
}

const ResourceMetadata* ResourceContextDecorator::GetParent() const
{
    return m_parent;
}

sf::IntRect ResourceContextDecorator::GetParentBound() const
{
    const auto resources = GetResourceManager();
    if (!resources)
        throw Gx::ResourceAccessException(GetID(), "ResourceManager is not set within this context.");

    const auto parent = GetParent();
    if (!parent)
        return sf::IntRect{};

    auto resolve = [] (const std::any& resource) -> std::string
    {
        if (resource.type() == typeid(Gx::Json))
            return std::any_cast<Gx::Json>(resource).get<std::string>();

        if (resource.type() == typeid(std::string))
            return std::any_cast<std::string>(resource);

        return std::string();
    };

    auto parentID = std::string();
    auto& require = parent->Require;

    if (const auto bound = require.find("bndID"); bound != require.end())
        parentID = resolve(bound->second);
    else if (const auto globalBound = require.find("globalBndID"); globalBound != require.end())
        parentID = resolve(globalBound->second);

    if (!parentID.empty())
    {
        if (const auto bound = resources->Find<sf::IntRect>(parentID))
            return *bound;
    }

    return sf::IntRect{};
}

ResourceContextDecorator::ResourceContextDecorator(const Gx::ResourceContext& ctx) :
    Gx::ResourceContext(ctx),
    m_parent(nullptr)
{
    if (const auto parent = dynamic_cast<const ResourceContextDecorator*>(&ctx))
        m_parent = parent->m_parent;
}

ResourceContextDecorator::ResourceContextDecorator(const Gx::ResourceContext& ctx, const ResourceMetadata& parent) :
    Gx::ResourceContext(ctx),
    m_parent(&parent)
{
}

std::string ResourceContextDecorator::Format(const std::string& fileName, const std::string& format) const
{
    const size_t dotIndex = fileName.rfind('.');

    auto store = fmt::dynamic_format_arg_store<fmt::format_context>();
    auto extension = std::string();
    if (dotIndex != std::string::npos)
    {
        extension = fileName.substr(dotIndex + 1);

        store.push_back(fmt::arg("Name", fileName.substr(0, dotIndex)));
        store.push_back(fmt::arg("Extension", extension));
    }
    else
    {
        store.push_back(fmt::arg("Name", fileName));
        store.push_back(fmt::arg("Extension", std::string()));
    }

    store.push_back(fmt::arg("MapID", GetProperty("MapID").value_or(std::string())));
    store.push_back(fmt::arg("FxMapID", GetProperty("FxMapID").value_or(std::string())));
    store.push_back(fmt::arg("FxExtension", GetProperty("FxExtension").value_or(extension)));
    store.push_back(fmt::arg("EffectID", GetProperty("EffectID").value_or(std::string())));
    store.push_back(fmt::arg("MapEffectID", fmt::vformat("{MapID}_{EffectID}", store)));

    return fmt::vformat(format, store);
}
