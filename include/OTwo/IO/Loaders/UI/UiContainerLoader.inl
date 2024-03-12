#include <OTwo/IO/Loaders/UI/UiContainerLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/States/Components/Common/ChatWindow.hpp>
#include <OTwo/States/Components/Common/Marquee.hpp>

#include <Genode/Graphics.hpp>
#include <Genode/UI.hpp>
//
//template<typename R, typename T>
//void UiContainerLoader::ParseObject(const std::string &name, const Gx::Json json, const Gx::ResourceContext &ctx)
//{
//    auto metaLoader = MetadataLoader();
//    Gx::ResourcePtr<ResourceMetadata> metadata;
//    if (json.type() == Gx::Json::value_t::string)
//        metadata = metaLoader.LoadFromFile(json.get<std::string>(), ctx);
//    else
//        metadata = metaLoader.LoadFromJson(json, ctx);
//
//    switch (metadata->Type)
//    {
//        case ResourceMetadata::ResourceType::Sound:       return LoadResource<sf::Sound>(name, json, ctx);
//        case ResourceMetadata::ResourceType::Music:       return LoadResource<sf::Music>(name, json, ctx);
//        case ResourceMetadata::ResourceType::Sprite:      return LoadResource<Gx::Sprite>(name, json, ctx);
//        case ResourceMetadata::ResourceType::Image:       return LoadResource<Gx::Image>(name, json, ctx);
//        case ResourceMetadata::ResourceType::Animation:   return LoadResource<Gx::Animation>(name, json, ctx);
//        case ResourceMetadata::ResourceType::List:        return LoadResource<Gx::List>(name, json, ctx);
//        case ResourceMetadata::ResourceType::Label:       return LoadResource<Gx::Label>(name, json, ctx);
//        case ResourceMetadata::ResourceType::Number:      return LoadResource<Gx::Number>(name, json, ctx);
//        case ResourceMetadata::ResourceType::TextBox:     return LoadResource<Gx::TextBox>(name, json, ctx);
//        case ResourceMetadata::ResourceType::Button:      return LoadResource<Gx::Button>(name, json, ctx);
//        case ResourceMetadata::ResourceType::CheckBox:    return LoadResource<Gx::CheckBox>(name, json, ctx);
//        case ResourceMetadata::ResourceType::RadioButton: return LoadResource<Gx::RadioButton>(name, json, ctx);
//        case ResourceMetadata::ResourceType::ProgressBar: return LoadResource<Gx::ProgressBar>(name, json, ctx);
//        case ResourceMetadata::ResourceType::ScrollBar:   return LoadResource<Gx::ScrollBar>(name, json, ctx);
//        case ResourceMetadata::ResourceType::Marquee:     return LoadResource<Marquee>(name, json, ctx);
//        case ResourceMetadata::ResourceType::ChatWindow:  return LoadResource<ChatWindow>(name, json, ctx);
//        case ResourceMetadata::ResourceType::None:
//        default:
//            break;
//    }
//}
//
//
//template<typename R>
//Gx::ResourcePtr<R> UiContainerLoader::LoadResource(const std::string &name, const Gx::Json &json, const Gx::ResourceContext &ctx)
//{
//    auto baseLoader = Gx::ResourceLoaderFactory::GetLoader<R>();
//    if (!baseLoader)
//        return nullptr;
//
//    auto loader = dynamic_cast<ResourceLoader<R>*>(baseLoader.get());
//    if (!loader)
//        return nullptr;
//
//    Gx::ResourcePtr<R> resource;
//    if (json.type() == Gx::Json::value_t::string)
//        resource = loader->LoadFromFile(json.get<std::string>());
//    else
//        resource = loader->LoadFromJson(json, ctx);
//
//    if (!resource)
//        return nullptr;
//
//    if constexpr (std::is_base_of_v<Gx::Node, R>)
//        resource->SetName(name);
//
//    return resource;
//}
