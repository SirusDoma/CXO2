#include <OTwo/IO/Loaders/UI/ListLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectPopulator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<Gx::List> ListLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &ctx) const
{
    auto metadata = ListMetadata();
    if (!MetadataLoader::Parse(json, metadata, ctx))
        return nullptr;
    
    auto attributes = json.at("attributes");
    if (auto transform = attributes.find("transform"); transform != attributes.end())
        TransformLoader::ParseMetadata(transform.value(), metadata, ctx);

    if (auto layouts = attributes.find("layouts"); layouts != attributes.end())
    {
        for (auto &transform : layouts->items())
        {
            if (auto layout = TransformMetadata(); TransformLoader::ParseMetadata(transform.value(), layout, ctx))
                metadata.Layouts.push_back(layout);
        }
    }
    else
    {
        if (auto vertical = attributes.find("vertical"); vertical != attributes.end())
        {
            metadata.VerticalCount   = vertical->at("count").get<unsigned int>();
            metadata.VerticalSpacing = vertical->at("spacing").get<float>();
        }
        else
        {
            metadata.VerticalCount   = 1;
            metadata.VerticalSpacing = 0.f;
        }

        if (auto horizontal = attributes.find("horizontal"); horizontal != attributes.end())
        {
            metadata.HorizontalCount   = horizontal->at("count").get<unsigned int>();
            metadata.HorizontalSpacing = horizontal->at("spacing").get<float>();
        }
        else
        {
            metadata.HorizontalCount   = 1;
            metadata.HorizontalSpacing = 0.f;
        }

        if (auto order = attributes.find("order"); order != attributes.end())
        {
            if (auto parsed = magic_enum::enum_cast<Gx::List::Order>(order->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                metadata.Order = parsed.value();
        }
        else
            metadata.Order = Gx::List::Order::Vertical;
    }

    if (auto prefab = attributes.find("prefab"); prefab != attributes.end())
    {
        if (auto data = prefab->find("count"); data != prefab->end())
            metadata.ItemCount = data->get<int>();

        if (auto data = prefab->find("name"); data != prefab->end())
            metadata.ItemName = data->get<std::string>();

        if (auto data = prefab->find("source"); data != prefab->end())
            metadata.ItemSource = data.value();
    }

    return LoadFromMetadata(metadata, ctx);
}

Gx::ResourcePtr<Gx::List> ListLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const ListMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    auto list = std::make_unique<Gx::List>(
        metadata->VerticalCount,   metadata->VerticalSpacing,
        metadata->HorizontalCount, metadata->HorizontalSpacing
    );
    
    list->SetName(metadata->Name);
    list->SetOrigin(metadata->Origin);
    list->SetPosition(metadata->Position);
    list->SetScale(metadata->Scale);
    list->SetRotation(metadata->Rotation);
    list->SetOrder(metadata->Order);

    if (!metadata->Layouts.empty())
    {
        for (const auto &layout : metadata->Layouts)
            list->AddLayout({ layout.Origin, layout.Position, layout.Rotation, layout.Scale });
    }

    if (context.Available())
    {
        auto populator = ObjectPopulator::Decorate(list.get());
        if (!metadata->ItemSource.empty())
        {
            list->UseBatching(true);
            for (int i = 0; i < metadata->ItemCount; i++)
            {
                auto name = meta.Name + "/" + metadata->ItemName + std::to_string(i + 1);
                auto ctx  = Gx::ResourceContext::Rebind(name, context);

                ObjectLoader::Load(name, metadata->ItemSource, populator, ctx);
            }
        }
        else if (!metadata->Objects.empty())
        {
            list->UseBatching(false);
            for (auto [key, object] : metadata->Objects)
            {
                auto name = meta.Name + "/" + key;
                auto ctx  = Gx::ResourceContext::Rebind(name, context);

                ObjectLoader::Load(name, object, populator, ctx);
            }
        }
    }

    return list;
}
