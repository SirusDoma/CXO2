#include <CXO2/IO/Loaders/UI/Components/Planet/ChannelBoardLoader.hpp>
#include <CXO2/IO/Loaders/UI/ImageLoader.hpp>
#include <CXO2/IO/Loaders/UI/Components/Planet/ChannelButtonLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>

#include <CXO2/Metadata/UI/Components/Planet/ChannelBoardMetadata.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/StringTable/Identifiers/Planet.hpp>

#include <fmt/format.h>

namespace Cx
{
    using namespace StringTable::Identifiers;

    Gx::ResourcePtr<ChannelBoard> ChannelBoardLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        ChannelBoardMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (!ImageLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (const auto count = attributes.find("channelCount"); count != attributes.end())
                metadata.ChannelCount = count->get<unsigned int>();
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<ChannelBoard> ChannelBoardLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const ChannelBoardMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);

        auto channelBoard = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);

        if (const auto texture = ctx.Require<sf::Texture>(*metadata))
        {
            channelBoard->SetTexture(*texture);
            if (metadata->Frames.size() > 0)
            {
                for (const auto& frame : metadata->Frames)
                    channelBoard->AddFrame(frame.Name, frame.Value);
            }
            else
                channelBoard->SetTexCoords(metadata->TexCoords);

            channelBoard->SetPosition(metadata->Position.value_or(sf::Vector2f()));
        }
        else
        {
            auto position = sf::Vector2f();
            if (metadata->Position.has_value())
            {
                position = *metadata->Position;
            }
            else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
            {
                position = {
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y),
                };
            }

            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                channelBoard->SetTexture(sheet->GetTexture());
                if (sheet->TexCoords.size() > 0)
                {
                    std::size_t i = 0;
                    const std::vector<std::string> names = { "Notice", "ChannelList" };

                    for (const auto& frame : sheet->TexCoords)
                    {
                        channelBoard->AddFrame(names[i], { frame, {}, position, {}, {1.f, 1.f} });
                        if (i < names.size() - 1)
                            i++;
                    }

                    if (sheet->TexCoords.size() < 2)
                        channelBoard->AddFrame(names[1], Gx::Image::Frame{ {}, {}, position, {}, {1.f, 1.f} });
                }
                else if (metadata->Frames.size() > 0)
                {
                    for (const auto& frame : metadata->Frames)
                        channelBoard->AddFrame(frame.Name, frame.Value);
                }
                else
                    channelBoard->SetTexCoords(metadata->TexCoords);
            }

            channelBoard->SetPosition(position);
        }

        channelBoard->SetName(metadata->Name);
        channelBoard->SetChannelsPerPage(metadata->ChannelCount);
        channelBoard->SetColor(metadata->Color);
        channelBoard->SetOrigin(metadata->Origin);
        channelBoard->SetScale(metadata->Scale);
        channelBoard->SetRotation(metadata->Rotation);

        auto container = SceneComposer::Compose(*channelBoard);
        LoadChildren(container, meta, context);

        return channelBoard;
    }
}
