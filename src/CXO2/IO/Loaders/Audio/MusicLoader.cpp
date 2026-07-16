#include <CXO2/IO/Loaders/Audio/MusicLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/Metadata/Audio/MusicMetadata.hpp>

#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/FileSystem/FileSystem.hpp>
#include <Genode/Utilities/StringHelper.hpp>

namespace Cx
{
    void MusicLoader::OnRegistered(const std::string& id, const Builder&)
    {
        SceneComposer::Register(id, [] (const std::string& name, const Gx::Json& json, SceneComposer& composer, Gx::ResourceContext& context)
        {
            composer.Add<sf::Music>(name, json, context);
        });
    }

    Gx::ResourcePtr<sf::Music> MusicLoader::LoadFromFile(const std::filesystem::path& fileName, const Gx::ResourceContext& ctx) const
    {
        if (Gx::StringHelper::IsGlobMatch(fileName.string(), "*.json", false))
            return ResourceLoader::LoadFromFile(fileName, ctx);

        auto metadata = MusicMetadata();
        metadata.Source = fileName.string();

        return LoadFromMetadata(metadata, ctx);
    }

    Gx::ResourcePtr<sf::Music> MusicLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = MusicMetadata();
        if (!MetadataLoader::Parse(json, metadata, context))
            return nullptr;

        if (const auto it = metadata.Require.find("music"); it != metadata.Require.end())
            metadata.Source = it->second.get<std::string>();

        if (const auto attributes = json.find("attributes"); attributes != json.end())
        {
            if (const auto loop = attributes->find("loop"); loop != attributes->end())
                metadata.IsLoop = loop->get<bool>();
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<sf::Music> MusicLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const MusicMetadata*>(&meta);
        if (!metadata)
            throw Gx::ResourceLoadException(context.GetID(), "The specified metadata is incompatible");

        const auto size = Gx::FileSystem::GetFileSize(metadata->Source);
        if (!size.has_value())
            throw Gx::ResourceLoadException(context.GetID(), "Failed to determine resource size.");

        auto data = new std::uint8_t[size.value()];
        if (!Gx::FileSystem::ReadFile(metadata->Source, data, size.value()).has_value())
        {
            delete[] data;
            return nullptr;
        }

        auto music = Gx::ResourcePtr<sf::Music>(new sf::Music(), [data] (auto ptr)
        {
            delete ptr;
            delete[] data;
        });

        if (!music->openFromMemory(data, size.value()))
            return nullptr;

        music->setLooping(metadata->IsLoop);
        return music;
    }

    bool MusicLoader::IsStreaming() const
    {
        return true;
    }
}
