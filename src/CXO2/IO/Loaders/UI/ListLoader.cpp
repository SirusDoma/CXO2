#include <CXO2/IO/Loaders/UI/ListLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/TransformLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>

#include <CXO2/Metadata/UI/ListMetadata.hpp>

#include <CXO2/UI/Room/RoomList.hpp>

#include <Genode/Audio/AudioMixer.hpp>
#include <Genode/IO/ResourceManager.hpp>
#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    void ListLoader::OnRegistered(const std::string& id, const Builder& builder)
    {
        ResourceLoader<Gx::List>::OnRegistered(id, builder);

        Gx::ResourceLoaderFactory::Map<Gx::List, RoomList>();
        Gx::ResourceLoaderFactory::Map<Gx::Node, RoomList>();
    }

    Gx::ResourcePtr<Gx::List> ListLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = ListMetadata();
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (const auto transform = attributes.find("transform"); transform != attributes.end())
                TransformLoader::ParseMetadata(transform.value(), metadata, context);

            if (const auto layouts = attributes.find("layouts"); layouts != attributes.end())
            {
                for (const auto& transform : layouts->items())
                {
                    if (auto layout = TransformMetadata(); TransformLoader::ParseMetadata(transform.value(), layout, context))
                        metadata.Layouts.push_back(layout);
                }
            }
            else
            {
                const auto vertical = attributes.find("vertical");
                const auto horizontal = attributes.find("horizontal");

                if (vertical != attributes.end())
                {
                    metadata.VerticalCount   = vertical->at("count").get<unsigned int>();
                    metadata.VerticalSpacing = vertical->at("spacing").get<float>();
                }
                else
                {
                    metadata.VerticalCount   = 1;
                    metadata.VerticalSpacing = 0.f;
                }

                if (horizontal != attributes.end())
                {
                    metadata.HorizontalCount   = horizontal->at("count").get<unsigned int>();
                    metadata.HorizontalSpacing = horizontal->at("spacing").get<float>();
                }
                else
                {
                    metadata.HorizontalCount   = 1;
                    metadata.HorizontalSpacing = 0.f;
                }

                if (vertical == attributes.end() && horizontal == attributes.end())
                {
                    metadata.VerticalCount   = 0;
                    metadata.HorizontalCount = 0;
                }

                if (const auto order = attributes.find("order"); order != attributes.end())
                {
                    if (const auto parsed = magic_enum::enum_cast<Gx::List::Order>(order->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                        metadata.Order = parsed.value();
                }
                else
                    metadata.Order = Gx::List::Order::Vertical;
            }
        }

        auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto bound = ctx.Require<sf::IntRect>(metadata); bound && !metadata.Position.has_value())
        {
            metadata.Position = {
                static_cast<float>(bound->position.x),
                static_cast<float>(bound->position.y),
            };
        }

        if (const auto it = metadata.Require.find("template"); it != metadata.Require.end())
        {
            const auto prefab = it->second;
            if (const auto data = prefab.find("count"); data != prefab.end())
                metadata.ItemCount = data->get<int>();

            if (const auto data = prefab.find("name"); data != prefab.end())
                metadata.ItemName = data->get<std::string>();

            if (const auto data = prefab.find("source"); data != prefab.end())
                metadata.ItemSource = data.value();

            // TODO: Support local bounds?
            if (const auto data = prefab.find("bounds"); data != prefab.end() && metadata.Layouts.empty())
            {
                auto base = sf::IntRect();
                for (const auto& field : { "bndID", "globalBndID" })
                {
                    if (auto bnd = metadata.Require.find(std::string(field)); bnd != metadata.Require.end())
                    {
                        if (bnd->second.is_string())
                            base = context.Acquire<sf::IntRect>(bnd->second.get<std::string>());

                        break;
                    }
                }

                for (const auto& boundID : data->items())
                {
                    auto bound = context.Acquire<sf::IntRect>(boundID.value().get<std::string>());
                    auto transform = TransformMetadata{};
                    transform.Position = {
                        static_cast<float>(bound.position.x) - base.position.x,
                        static_cast<float>(bound.position.y) - base.position.y,
                    };

                    metadata.Layouts.push_back(std::move(transform));
                }

                metadata.ItemCount = metadata.Layouts.size();
            }
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Gx::List> ListLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const ListMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);
    
        auto list = Instantiate(context);
        list->SetName(metadata->Name);
        list->SetVerticalRepeat(metadata->VerticalCount, metadata->VerticalSpacing);
        list->SetHorizontalRepeat(metadata->HorizontalCount, metadata->HorizontalSpacing);
        list->SetOrigin(metadata->Origin);
        list->SetPosition(metadata->Position.value_or(sf::Vector2f()));
        list->SetScale(metadata->Scale);
        list->SetRotation(metadata->Rotation);
        list->SetOrder(metadata->Order);

        if (!metadata->Layouts.empty())
        {
            for (const auto& layout : metadata->Layouts)
                list->AddLayout({ layout.Origin, layout.Position.value_or(sf::Vector2f()), layout.Rotation, layout.Scale });
        }

        if (context.Available())
        {
            auto container = SceneComposer::Compose(*list);
            if (!metadata->ItemSource.empty())
            {
                list->SetBatchingEnabled(true);
                for (unsigned int i = 0; i < metadata->ItemCount; i++)
                {
                    auto name = fmt::format("{}/{}{}", meta.Name, metadata->ItemName, i + 1);
                    auto ctx  = Gx::ResourceContext::Rebind(context, name);

                    container.Add(name, metadata->ItemSource, ctx);
                }
            }
            else if (!metadata->Objects.empty())
            {
                list->SetBatchingEnabled(false);
                LoadChildren(container, meta, context);
            }
        }

        return list;
    }
}
