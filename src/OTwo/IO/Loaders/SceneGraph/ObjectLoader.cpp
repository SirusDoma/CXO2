#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/UI/Dialogs/CreateRoomDialog.hpp>
#include <OTwo/UI/Dialogs/OptionDialog.hpp>
#include <OTwo/UI/Dialogs/SelectMusicDialog.hpp>
#include <OTwo/UI/Planet/ChannelBoard.hpp>
#include <OTwo/UI/Room/RoomList.hpp>

void ObjectLoader::LoadFromJson(const std::string& name, const Gx::Json& json, ObjectContainer& container, Gx::ResourceContext& ctx)
{
    const auto metaLoader = MetadataLoader();
    Gx::ResourcePtr<ResourceMetadata> metadata;
    if (json.type() == Gx::Json::value_t::string)
        metadata = metaLoader.LoadFromFile(json.get<std::string>(), ctx);
    else
        metadata = metaLoader.LoadFromJson(json, ctx);

    if (metadata == nullptr)
        throw Gx::ResourceLoadException("Failed to load metadata");

    const auto type = std::string(magic_enum::enum_name(metadata->Type));
    switch (metadata->Type)
    {
        case ResourceMetadata::ResourceType::None:
        {
            break;
        }
        case ResourceMetadata::ResourceType::Sound:
        {
            container.Add(name, LoadFromJson<sf::Sound>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Music:
        {
            container.Add(name, LoadFromJson<sf::Music>(name, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::OptionDialog:
        {
            container.Add(name, LoadFromJson<OptionDialog>(name, type, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::CreateRoomDialog:
        {
            container.Add(name, LoadFromJson<CreateRoomDialog>(name, type, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::SelectMusicDialog:
        {
            container.Add(name, LoadFromJson<SelectMusicDialog>(name, type, json, ctx), ctx);
            break;
        }
        case ResourceMetadata::ResourceType::Dialog:
        {
            container.Add(name, LoadFromJson<Gx::Dialog>(name, type, json, ctx), ctx);
            break;
        }
        default:
        {
            container.Add(name, LoadFromJson<Gx::Node>(name, type, json, ctx), ctx);
            break;
        }
    }
}
