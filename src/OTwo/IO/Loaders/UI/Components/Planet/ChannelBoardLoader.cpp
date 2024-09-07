#include <OTwo/IO/Loaders/UI/Components/Planet/ChannelBoardLoader.hpp>
#include <OTwo/IO/Loaders/UI/ImageLoader.hpp>
#include <OTwo/IO/Loaders/UI/Components/Planet/ChannelButtonLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <OTwo/Metadata/UI/Components/Planet/ChannelBoardMetadata.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectContainer.hpp>

Gx::ResourcePtr<ChannelBoard> ChannelBoardLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    ChannelBoardMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    if (!ImageLoader::ParseMetadata(attributes, metadata, context))
        return nullptr;

    if (const auto count = attributes.find("channelCount"); count != attributes.end())
        metadata.ChannelCount = count->get<unsigned int>();

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<ChannelBoard> ChannelBoardLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const ChannelBoardMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");

    auto channelBoard = std::make_unique<ChannelBoard>();
    if (metadata->Frames.size() > 0)
    {
        for (const auto& frame : metadata->Frames)
            channelBoard->AddFrame(frame.first, frame.second);
    }
    else
        channelBoard->SetTexCoords(metadata->TexCoords);

    auto acquirer = ResourceContextDecorator::Decorate(context);
    if (auto texture = acquirer.Find<sf::Texture>(*metadata); texture)
        channelBoard->SetTexture(*texture);

    if (auto it = metadata->Require.find("IDC_CHANNEL_BUTTON"); it != metadata->Require.end())
    {
        auto name = meta.Name + "/IDC_CHANNEL_BUTTON";
        auto channelButtonLoader = ChannelButtonLoader();
        if (it->second.type() == typeid(Gx::Json) && it->second.has_value())
        {
            Gx::ResourcePtr<ChannelButton> resource;
            auto channelButtonData = std::any_cast<Gx::Json>(it->second);
            auto ctx = Gx::ResourceContext::Rebind(name, acquirer);

            if (channelButtonData.type() == Gx::Json::value_t::string)
                resource = channelButtonLoader.LoadFromFile(channelButtonData.get<std::string>(), ctx);
            else
                resource = channelButtonLoader.LoadFromJson(channelButtonData, ctx);

            if (resource)
                channelBoard->SetChannelButton(acquirer.Store(name, std::move(resource)));
        }
    }

    channelBoard->SetName(metadata->Name);
    channelBoard->SetChannelsPerPage(metadata->ChannelCount);
    channelBoard->SetColor(metadata->Color);
    channelBoard->SetOrigin(metadata->Origin);
    channelBoard->SetPosition(metadata->Position);
    channelBoard->SetScale(metadata->Scale);
    channelBoard->SetRotation(metadata->Rotation);

    auto container = ObjectContainer::Decorate(channelBoard.get());
    for (auto [key, object] : metadata->Objects)
    {
        auto name = meta.Name + "/" + key;
        auto ctx  = Gx::ResourceContext::Rebind(name, context);

        ObjectLoader::Load(name, object, container, ctx);
    }

    return channelBoard;
}
