#include <CXO2/IO/Loaders/SceneGraph/ObjectLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>

#include <CXO2/UI/Dialogs/CreateRoomDialog.hpp>
#include <CXO2/UI/Dialogs/OptionDialog.hpp>
#include <CXO2/UI/Dialogs/SelectMusicDialog.hpp>
#include <CXO2/UI/Planet/ChannelBoard.hpp>
#include <CXO2/UI/Room/RoomList.hpp>

namespace Cx
{
    void ObjectLoader::LoadFromJson(const std::string& name, const Gx::Json& json, ObjectContainer& container, Gx::ResourceContext& context)
    {
        const auto metaLoader = MetadataLoader();
        Gx::ResourcePtr<ResourceMetadata> metadata;

        if (json.type() == Gx::Json::value_t::string)
            metadata = metaLoader.LoadFromFile(json.get<std::string>(), context);
        else
            metadata = metaLoader.LoadFromJson(json, context);

        if (metadata == nullptr)
            throw Gx::ResourceLoadException(context.GetID(), "Failed to load metadata");

        const auto type = std::string(magic_enum::enum_name(metadata->Type));
        switch (metadata->Type)
        {
            case ResourceMetadata::ResourceType::None:
            {
                break;
            }
            case ResourceMetadata::ResourceType::Sound:
            {
                container.Add(name, LoadFromJson<sf::Sound>(name, json, context), context);
                break;
            }
            case ResourceMetadata::ResourceType::Music:
            {
                container.Add(name, LoadFromJson<sf::Music>(name, json, context), context);
                break;
            }
            case ResourceMetadata::ResourceType::OptionDialog:
            {
                container.Add(name, LoadFromJson<OptionDialog>(name, type, json, context), context);
                break;
            }
            case ResourceMetadata::ResourceType::CreateRoomDialog:
            {
                container.Add(name, LoadFromJson<CreateRoomDialog>(name, type, json, context), context);
                break;
            }
            case ResourceMetadata::ResourceType::SelectMusicDialog:
            {
                container.Add(name, LoadFromJson<SelectMusicDialog>(name, type, json, context), context);
                break;
            }
            case ResourceMetadata::ResourceType::Dialog:
            {
                container.Add(name, LoadFromJson<Gx::Dialog>(name, type, json, context), context);
                break;
            }
            default:
            {
                container.Add(name, LoadFromJson<Gx::Node>(name, type, json, context), context);
                break;
            }
        }
    }
}
