#include <Genode/Utilities/StringHelper.hpp>
#include <CXO2/IO/Loaders/Audio/SoundLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/Metadata/Audio/SoundMetadata.hpp>

#include <CXO2/Metadata/ResourceMetadata.hpp>

namespace Cx
{
    Gx::ResourcePtr<sf::Sound> SoundLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& context) const
    {
        if (Gx::StringHelper::IsGlobMatch(fileName.string(), "*.json", false))
            return ResourceLoader::LoadFromFile(fileName, context);

        const auto name = fileName.string();
        return std::make_unique<sf::Sound>(
            context.Acquire<sf::SoundBuffer>(name, name)
        );
    }

    Gx::ResourcePtr<sf::Sound> SoundLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = SoundMetadata();
        if (!MetadataLoader::Parse(json, metadata, context))
            return nullptr;

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<sf::Sound> SoundLoader::LoadFromMetadata(const ResourceMetadata& metadata, const Gx::ResourceContext& context) const
    {
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto buffer = ctx.Require<sf::SoundBuffer>(metadata); buffer)
            return std::make_unique<sf::Sound>(*buffer);

        return nullptr;
    }
}
