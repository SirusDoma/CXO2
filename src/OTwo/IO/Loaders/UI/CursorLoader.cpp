#include <OTwo/IO/Loaders/UI/CursorLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>

#include <OTwo/Metadata/UI/CursorMetadata.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<Gx::Cursor> CursorLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
{
    CursorMetadata metadata;
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    if (const auto it = json.find("attributes"); it != json.end())
    {
        const auto& attributes = it.value();
        if (attributes.empty())
            return LoadFromMetadata(metadata, context);

        auto states = attributes.find("states");
        if (states == attributes.end())
            return LoadFromMetadata(metadata, context);

        for (auto [name, attr] : states->items())
        {
            auto state = CursorMetadata::CursorStateMetadata();
            if (const auto parse = magic_enum::enum_cast<Gx::Cursor::Type>(name); parse.has_value())
                state.Type = parse.value();
            else
                continue;

            if (auto id = attr.find("id"); id != attr.end())
                state.ID = id->get<std::uint32_t>();

            auto hs = attr.find("hotspot");
            auto hotspot = sf::Vector2u();
            if (hs != attr.end())
            {
                hs->at("x").get_to(hotspot.x);
                hs->at("y").get_to(hotspot.y);
            }
            state.Hotspot = hotspot;

            auto t = attr.find("texCoords");
            auto texCoords = sf::IntRect();
            if (t != attr.end())
            {
                unsigned int x, y, w, h;
                t->at("x").get_to(x);
                t->at("y").get_to(y);
                t->at("width").get_to(w);
                t->at("height").get_to(h);

                texCoords = sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h));
            }

            state.TexCoords = texCoords;
            metadata.States.push_back(state);
        }
    }

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Cursor> CursorLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
{
    const auto metadata = dynamic_cast<const CursorMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible");
    
    auto cursor = std::make_unique<Gx::Cursor>();
    const auto ctx = ResourceContextDecorator::Decorate(context);

    if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
    {
        const auto source = texture->copyToImage();
        if (!metadata->States.empty())
        {
            for (const auto& state : metadata->States)
            {
                auto image = sf::Image(sf::Vector2u(state.TexCoords.size.x, state.TexCoords.size.y), sf::Color::Transparent);
                if (!image.copy(source, sf::Vector2u(), state.TexCoords, true))
                    throw Gx::ResourceLoadException(context.GetID(), fmt::format("Failed to load cursor state ({})", magic_enum::enum_name(state.Type)));

                cursor->Register(state.Type, image, state.Hotspot);
            }
        }
        else
        {
            cursor = std::make_unique<Gx::Cursor>(*texture);
        }
    }
    else if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
    {
        const auto source = sheet->GetTexture().copyToImage();
        if (!metadata->States.empty())
        {
            for (const auto& state : metadata->States)
            {
                auto texCoords = state.TexCoords;
                if (state.ID.has_value())
                    texCoords = sheet->TexCoords[state.ID.value()];

                auto image = sf::Image(sf::Vector2u(texCoords.size.x, texCoords.size.y), sf::Color::Transparent);
                if (!image.copy(source, sf::Vector2u(), texCoords, true))
                    throw Gx::ResourceLoadException(context.GetID(), fmt::format("Failed to load cursor state ({})", magic_enum::enum_name(state.Type)));

                cursor->Register(state.Type, image, state.Hotspot);
            }
        }
        else
        {
            cursor = std::make_unique<Gx::Cursor>(sheet->GetTexture());
        }
    }

    return cursor;
}
