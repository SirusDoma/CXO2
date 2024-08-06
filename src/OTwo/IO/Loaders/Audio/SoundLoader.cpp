#include <OTwo/IO/Loaders/Audio/SoundLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/Metadata/Audio/SoundMetadata.hpp>

#include <OTwo/Metadata/ResourceMetadata.hpp>

Gx::ResourcePtr<sf::Sound> SoundLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    auto metadata = SoundMetadata();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<sf::Sound> SoundLoader::LoadFromMetadata(const ResourceMetadata &metadata, const Gx::ResourceContext &context) const
{
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto buffer = ctx.Find<sf::SoundBuffer>(metadata); buffer)
        return std::make_unique<sf::Sound>(*buffer);

    return nullptr;
}
