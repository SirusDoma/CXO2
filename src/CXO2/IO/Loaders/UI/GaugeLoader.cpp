#include <CXO2/IO/Loaders/UI/GaugeLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/SpriteLoader.hpp>
#include <CXO2/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <CXO2/Metadata/UI/GaugeMetadata.hpp>

#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>

namespace Cx
{
    Gx::ResourcePtr<Gx::Gauge> GaugeLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        GaugeMetadata metadata;
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (!SpriteLoader::ParseMetadata(attributes, metadata, context))
                return Instantiate(context);

            if (const auto orientation = attributes.find("orientation"); orientation != attributes.end())
            {
                if (Gx::StringHelper::EqualsCaseInsensitive(orientation->get<std::string>(), "VERTICAL"))
                    metadata.Orientation = Gx::Gauge::Orientation::Vertical;
                else
                    metadata.Orientation = Gx::Gauge::Orientation::Horizontal;
            }

            if (const auto maximum = attributes.find("maximum"); maximum != attributes.end())
                metadata.Maximum = maximum->get<float>();
            else
                metadata.Maximum = 100.0f;

            if (const auto frames = attributes.find("frames"); frames != attributes.end())
            {
                for (const auto& frame : frames->items())
                {
                    SpriteMetadata frameMetadata;
                    if (!SpriteLoader::ParseMetadata(frame.value(), frameMetadata, context))
                        continue;

                    auto transform = frame.value().find("transform");
                    std::optional position = frameMetadata.Position;
                    if (transform == frame.value().end() || transform.value().find("position") == transform->end())
                        position = std::nullopt;

                    std::optional scale = frameMetadata.Scale;
                    if (transform == frame.value().end() || transform.value().find("scale") == transform->end())
                        scale = std::nullopt;

                    std::optional rotation = frameMetadata.Rotation;
                    if (transform == frame.value().end() || transform.value().find("rotation") == transform->end())
                        rotation = std::nullopt;

                    std::optional origin = frameMetadata.Origin;
                    if (transform == frame.value().end() || transform.value().find("origin") == transform->end())
                        origin = std::nullopt;

                    metadata.AnimationFrames.push_back(Gx::Animation::Frame
                    {
                        frameMetadata.TexCoords,
                        origin,
                        position,
                        rotation,
                        scale
                    });
                }
            }

            if (const auto duration = attributes.find("duration"); duration != attributes.end())
                metadata.AnimationDuration = sf::milliseconds(duration->get<unsigned int>());
            else
                metadata.AnimationDuration = sf::milliseconds(metadata.AnimationFrames.size() * 60);

            if (const auto flicker = attributes.find("flicker"); flicker != attributes.end())
                metadata.Flicker = flicker->get<bool>();
            else
                metadata.Flicker = false;

            if (const auto frame = attributes.find("frame"); frame != attributes.end())
                metadata.FrameID = frame->get<std::uint32_t>();
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Gx::Gauge> GaugeLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const GaugeMetadata*>(&meta);
        if (!metadata)
            throw Gx::ResourceLoadException(context.GetID(), "The specified metadata is incompatible");

        auto gauge = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
        {
            gauge->SetTexture(*texture);
            gauge->SetTexCoords(metadata->TexCoords);
            gauge->SetPosition(metadata->Position);

            for (const auto& frame : metadata->AnimationFrames)
                gauge->AddAnimationFrame(frame);
        }
        else
        {
            if (metadata->Position != sf::Vector2f())
            {
                gauge->SetPosition(metadata->Position);
            }
            else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
            {
                gauge->SetPosition({
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y),
                });
            }

            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                auto& sheetTexture = sheet->GetTexture();
                sheetTexture.setRepeated(true);

                gauge->SetTexture(sheetTexture);
                if (metadata->TexCoords != sf::IntRect())
                    gauge->SetTexCoords(metadata->TexCoords);
                else if (metadata->FrameID.has_value())
                    gauge->SetTexCoords(sheet->TexCoords[metadata->FrameID.value()]);
                else
                    gauge->SetTexCoords(sheet->TexCoords[0]);

                if (gauge->GetPosition() == sf::Vector2f() && !sheet->Frames.empty())
                {
                    if (metadata->FrameID.has_value())
                    {
                        gauge->SetPosition({
                            static_cast<float>(sheet->Frames[metadata->FrameID.value()].position.x),
                            static_cast<float>(sheet->Frames[metadata->FrameID.value()].position.y),
                        });
                    }
                    else if (sheet->Frames[0].position != sf::Vector2i())
                    {
                        gauge->SetPosition({
                            static_cast<float>(sheet->Frames[0].position.x),
                            static_cast<float>(sheet->Frames[0].position.y),
                        });
                    }
                }

                if (sheet->TexCoords.size() > 1 && !metadata->FrameID.has_value())
                {
                    for (const auto& frame : sheet->TexCoords)
                        gauge->AddAnimationFrame(Gx::Animation::Frame{frame});
                }
            }
        }

        gauge->SetFlickering(metadata->Flicker);
        gauge->SetAnimationDuration(metadata->AnimationDuration);

        gauge->SetName(metadata->Name);
        gauge->SetOrientation(metadata->Orientation);
        gauge->SetMaximumValue(metadata->Maximum);
        gauge->SetValue(0);
        gauge->SetColor(metadata->Color);
        gauge->SetOrigin(metadata->Origin);
        gauge->SetScale(metadata->Scale);
        gauge->SetRotation(metadata->Rotation);

        auto container = ObjectContainer::Decorate(gauge.get());
        LoadChildren(container, meta, context);

        return gauge;
    }
}
