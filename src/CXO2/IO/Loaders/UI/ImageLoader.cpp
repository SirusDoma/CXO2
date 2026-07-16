#include <CXO2/IO/Loaders/UI/ImageLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>

#include <CXO2/Metadata/UI/ImageMetadata.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>

#include <CXO2/UI/Room/RoomButton.hpp>
#include <CXO2/UI/Playing/PlayMenu.hpp>

#include <CXO2/Contexts/SessionContext.hpp>
#include <CXO2/Contexts/GameContext.hpp>

#include <Genode/IO/ResourceLoaderFactory.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    void ImageLoader::OnRegistered(const std::string& id, const Builder& builder)
    {
        ResourceLoader<Gx::Image>::OnRegistered(id, builder);

        Gx::ResourceLoaderFactory::Map<Gx::Image,
            RoomButton,
            PlayMenu
        >();

        Gx::ResourceLoaderFactory::Map<Gx::Node,
            RoomButton,
            PlayMenu
        >();
    }

    Gx::ResourcePtr<Gx::Image> ImageLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        ImageMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (!ParseMetadata(attributes, metadata, context))
                return Instantiate(context);
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Gx::Image> ImageLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const ImageMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);
    
        auto image = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);

        if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
        {
            if (!metadata->Frames.empty())
            {
                for (const auto& frame : metadata->Frames)
                    image->AddFrame(frame.Name, frame.Value);
            }
            else
                image->SetTexCoords(metadata->TexCoords);

            image->SetTexture(*texture);
            image->SetSizeMode(metadata->SizeMode);
            image->SetLocalBounds(metadata->Bounds);
        }
        else
        {
            image->SetSizeMode(metadata->SizeMode);
            if (metadata->Bounds != sf::FloatRect())
            {
                image->SetLocalBounds(metadata->Bounds);
            }

            if (metadata->Position.has_value())
            {
                image->SetPosition(*metadata->Position);
            }
            else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
            {
                if (metadata->Bounds == sf::FloatRect())
                {
                    image->SetLocalBounds({
                        {},
                        {
                            static_cast<float>(bound->size.x),
                            static_cast<float>(bound->size.y),
                        }
                    });
                }

                image->SetPosition({
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y),
                });
            }

            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                image->SetTexture(sheet->GetTexture());
                if (!metadata->Frames.empty())
                {
                    for (std::size_t i = 0; i < metadata->Frames.size(); i++)
                    {
                        auto frame = metadata->Frames[i];
                        if (frame.Value.TexCoords == sf::IntRect())
                        {
                            if (frame.ID.has_value())
                            {
                                if (frame.ID.value() < sheet->TexCoords.size())
                                    frame.Value.TexCoords = sheet->TexCoords[frame.ID.value()];
                            }
                            else if (i < sheet->TexCoords.size())
                                frame.Value.TexCoords = sheet->TexCoords[i];
                        }

                        if (frame.Value.Position == sf::Vector2f())
                        {
                            if (frame.ID.has_value())
                            {
                                if (frame.ID.value() < sheet->Frames.size())
                                {
                                    const auto& base = sheet->Frames[frame.ID.value()];
                                    frame.Value.Position = {
                                        static_cast<float>(base.position.x),
                                        static_cast<float>(base.position.y),
                                    };
                                }
                            }
                            else
                                frame.Value.Position = image->GetPosition();
                        }

                        image->AddFrame(frame.Name, frame.Value);
                        if (image->GetFrameCount() == 1)
                            image->SetFrame(0);
                    }
                }
                else if (metadata->TexCoords != sf::IntRect())
                {
                    image->SetTexCoords(metadata->TexCoords);
                }
                else if (sheet->TexCoords.size() > 1)
                {
                    std::size_t index = 0;
                    for (const auto& frame : sheet->TexCoords)
                        image->AddFrame(std::to_string(index++), { frame, {}, image->GetPosition(), {}, {1.f, 1.f} });
                }
                else
                {
                    if (!sheet->TexCoords.empty())
                        image->SetTexCoords(sheet->TexCoords[0]);
                    if (image->GetPosition() == sf::Vector2f() && !sheet->Frames.empty())
                    {
                        image->SetPosition(sf::Vector2f{
                            static_cast<float>(sheet->Frames[0].position.x),
                            static_cast<float>(sheet->Frames[0].position.y),
                        });
                    }
                }
            }
        }

        image->SetName(metadata->Name);
        image->SetColor(metadata->Color);
        image->SetOrigin(metadata->Origin);
        image->SetScale(metadata->Scale);
        image->SetRotation(metadata->Rotation);

        auto container = SceneComposer::Compose(*image);
        LoadChildren(container, meta, context);

        return image;
    }

    bool ImageLoader::ParseMetadata(const Gx::Json& attributes, ImageMetadata& metadata, const Gx::ResourceContext& context)
    {
        if (attributes.empty())
            return false;

        if (const auto mode = attributes.find("sizeMode"); mode != attributes.end())
        {
            if (const auto parsed = magic_enum::enum_cast<Gx::Image::SizeMode>(mode->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                metadata.SizeMode = parsed.value();
        }

        if (const auto bounds = attributes.find("bounds"); bounds != attributes.end())
        {
            if (bounds->type() == Gx::Json::value_t::object)
            {
                float x, y, w, h;
                bounds->at("x").get_to(x);
                bounds->at("y").get_to(y);
                bounds->at("width").get_to(w);
                bounds->at("height").get_to(h);

                metadata.Bounds = sf::FloatRect{ {x, y}, {w, h} };
            }
            else if (bounds->type() == Gx::Json::value_t::string)
            {
                const auto& bound = context.Acquire<sf::IntRect>(bounds.value().get<std::string>());
                metadata.Bounds = {
                    {},
                    {
                        static_cast<float>(bound.size.x),
                        static_cast<float>(bound.size.y),
                    }
                };
            }
        }

        auto frames = attributes.find("frames");
        if (frames == attributes.end())
        {
            metadata.Frames.push_back({ "default", Gx::Image::Frame
                {
                    metadata.TexCoords,
                    metadata.Origin,
                    metadata.Position.value_or(sf::Vector2f()),
                    metadata.Rotation,
                    metadata.Scale
                }
            });

            return true;
        }

        for (auto [frameName, frameAttr] : frames->items())
        {
            auto frame = Gx::Image::Frame();
            auto position = metadata.Position.value_or(sf::Vector2f());
            if (auto p = frameAttr.find("position"); p != frameAttr.end())
            {
                position = sf::Vector2f();
                p->at("x").get_to(position.x);
                p->at("y").get_to(position.y);
            }
            frame.Position = position;

            auto scale = metadata.Scale;
            if (auto s = frameAttr.find("scale"); s != frameAttr.end())
            {
                scale = sf::Vector2f();
                s->at("scale").at("x").get_to(scale.x);
                s->at("scale").at("y").get_to(scale.y);
            }
            frame.Scale = scale;


            float rotation = metadata.Rotation;
            if (auto r = frameAttr.find("rotation"); r != frameAttr.end())
                r->get_to(rotation);
            frame.Rotation = rotation;

            auto origin = metadata.Origin;
            if ( auto o = frameAttr.find("origin"); o != frameAttr.end())
            {
                o->at("x").get_to(origin.x);
                o->at("y").get_to(origin.y);
            }
            frame.Origin = origin;

            auto texCoords = metadata.TexCoords;
            if (auto t = frameAttr.find("texCoords"); t != frameAttr.end())
            {
                unsigned int x, y, w, h;
                t->at("x").get_to(x);
                t->at("y").get_to(y);
                t->at("width").get_to(w);
                t->at("height").get_to(h);

                texCoords = sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h));
            }
            frame.TexCoords = texCoords;

            std::optional<std::uint16_t> id = std::nullopt;
            if (const auto it = frameAttr.find("id"); it != frameAttr.end())
                id = it.value().get<std::uint16_t>();

            metadata.Frames.push_back({frameName, frame, id});
        }

        return true;
    }
}
