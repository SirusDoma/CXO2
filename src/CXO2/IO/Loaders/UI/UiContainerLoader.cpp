#include <CXO2/IO/Loaders/UI/UiContainerLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/TransformLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>
#include <CXO2/Metadata/UI/UiContainerMetadata.hpp>
#include <CXO2/UI/Room/UserList.hpp>
#include <CXO2/UI/Common/ChatPanel.hpp>
#include <CXO2/UI/Waiting/MapSelector.hpp>
#include <CXO2/UI/Waiting/InstrumentSelector.hpp>

#include <CXO2/StringTable/Identifiers/Room.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Services/ChatService.hpp>
#include <CXO2/Services/ChannelService.hpp>
#include <CXO2/Avatar/ItemFactory.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>

namespace Cx
{
    using namespace StringTable::Identifiers;

    void UiContainerLoader::OnRegistered(const std::string& id, const Builder& builder)
    {
        ResourceLoader<Gx::UiContainer>::OnRegistered(id, builder);

        Gx::ResourceLoaderFactory::Map<Gx::UiContainer,
            ChatPanel,
            UserList,
            MapSelector,
            InstrumentSelector
        >();
        Gx::ResourceLoaderFactory::Map<Gx::Node,
            ChatPanel,
            UserList,
            MapSelector,
            InstrumentSelector
        >();
    }

    Gx::ResourcePtr<Gx::UiContainer> UiContainerLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        UiContainerMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto attributes = json.find("attributes"); attributes != json.end())
        {
            if (const auto transform = attributes->find("transform"); transform != attributes->end())
                TransformLoader::ParseMetadata(transform.value(), metadata, context);

            metadata.Bounds = {};
            if (const auto bounds = attributes->find("bounds"); bounds != attributes->end())
            {
                if (bounds->type() == Gx::Json::value_t::object)
                {
                    metadata.Bounds = {
                        {},
                        {
                            bounds->at("width"),
                            bounds->at("height")
                        }
                    };
                }
                else if (bounds->type() == Gx::Json::value_t::string)
                {
                    const auto& bound = context.Acquire<sf::IntRect>(bounds.value().get<std::string>());
                    metadata.Bounds = sf::IntRect{ {}, bound.size };
                }
            }
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Gx::UiContainer> UiContainerLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const UiContainerMetadata*>(&meta);
        if (metadata == nullptr)
            return Instantiate(context);

        auto container = Instantiate(context);
        auto populator = SceneComposer::Compose(*container);
        const auto ctx = ResourceContextDecorator::Decorate(context);

        auto bound = sf::IntRect();
        if (metadata->Position.has_value())
        {
            container->SetPosition(*metadata->Position);
            container->SetLocalBounds({
                {
                    static_cast<float>(metadata->Bounds.position.x),
                    static_cast<float>(metadata->Bounds.position.y)
                },
                {
                    static_cast<float>(metadata->Bounds.size.x),
                    static_cast<float>(metadata->Bounds.size.y)
                }
            });
        }
        else if (const auto bnd = ctx.Require<sf::IntRect>(*metadata); bnd)
        {
            bound = *bnd;
            container->SetPosition(sf::Vector2f{
                static_cast<float>(bnd->position.x),
                static_cast<float>(bnd->position.y),
            });

            if (metadata->Bounds != sf::IntRect())
            {
                container->SetLocalBounds({
                    {
                        static_cast<float>(metadata->Bounds.position.x),
                        static_cast<float>(metadata->Bounds.position.y)
                    },
                    {
                        static_cast<float>(metadata->Bounds.size.x),
                        static_cast<float>(metadata->Bounds.size.y)
                    }
                });
            }
            else
            {
                container->SetLocalBounds({
                    {},
                    {
                        static_cast<float>(bound.size.x),
                        static_cast<float>(bound.size.y)
                    }
                });
            }
        }

        container->SetName(metadata->Name);
        container->SetOrigin(metadata->Origin);
        container->SetScale(metadata->Scale);
        container->SetRotation(metadata->Rotation);

        LoadChildren(populator, meta, context);

        return container;
    }

}
