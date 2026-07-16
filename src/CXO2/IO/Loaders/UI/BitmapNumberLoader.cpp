#include <CXO2/IO/Loaders/UI/BitmapNumberLoader.hpp>
#include <CXO2/IO/Loaders/Graphics/TransformLoader.hpp>
#include <CXO2/IO/Loaders/MetadataLoader.hpp>
#include <CXO2/Metadata/UI/BitmapNumberMetadata.hpp>
#include <CXO2/Decorators/IO/ResourceContextDecorator.hpp>
#include <CXO2/IO/Loaders/SceneGraph/SceneComposer.hpp>

#include <magic_enum/magic_enum.hpp>

namespace Cx
{
    Gx::ResourcePtr<Gx::BitmapNumber> BitmapNumberLoader::LoadFromJson(const Gx::Json& json, const Gx::ResourceContext& context) const
    {
        auto metadata = BitmapNumberMetadata();
        if (!MetadataLoader::Parse(json, metadata, context))
            return Instantiate(context);

        if (const auto it = json.find("attributes"); it != json.end())
        {
            const auto& attributes = it.value();
            if (const auto transform = attributes.find("transform"); transform != attributes.end())
                TransformLoader::ParseMetadata(transform.value(), metadata, context);

            if (auto digitSize = attributes.find("digitSize"); digitSize != attributes.end())
            {
                unsigned int w, h;
                digitSize->at("width").get_to(w);
                digitSize->at("height").get_to(h);
                metadata.DigitSize = sf::Vector2u(w, h);
            }

            if (auto digitCount = attributes.find("digitCount"); digitCount != attributes.end())
                metadata.DigitCount = digitCount->get<unsigned int>();
            else
                metadata.DigitCount = 1;

            if (auto duration = attributes.find("duration"); duration != attributes.end())
                metadata.Duration = sf::milliseconds(duration->get<std::int32_t>());

            if (auto digits = attributes.find("digits"); digits != attributes.end())
            {
                for (auto [digit, data] : digits->items())
                {
                    if (data.is_array())
                    {
                        auto frames = std::vector<sf::IntRect>();
                        for (auto frame : data)
                        {
                            unsigned int x, y, w, h;
                            frame.at("x").get_to(x);
                            frame.at("y").get_to(y);
                            frame.at("width").get_to(w);
                            frame.at("height").get_to(h);

                            frames.push_back(sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h)));
                        }

                        if (metadata.Duration == sf::Time::Zero)
                            metadata.Duration = sf::milliseconds(frames.size() * 60);

                        metadata.DigitFrames[std::stoi(digit)] = frames;
                    }
                    else
                    {
                        unsigned int x, y, w, h;
                        data.at("x").get_to(x);
                        data.at("y").get_to(y);
                        data.at("width").get_to(w);
                        data.at("height").get_to(h);
                        metadata.DigitFrames[std::stoi(digit)] = { sf::IntRect(sf::Vector2i(x, y), sf::Vector2i(w, h)) };
                    }
                }
            }

            if (auto color = attributes.find("color"); color != attributes.end())
            {
                unsigned int a, r, g, b;
                color->at("a").get_to(a);
                color->at("r").get_to(r);
                color->at("g").get_to(g);
                color->at("b").get_to(b);
                metadata.Color = sf::Color(r, g, b, a);
            }
            else
                metadata.Color = sf::Color::White;

            if (auto kerning = attributes.find("kerning"); kerning != attributes.end())
                metadata.Kerning = kerning->get<float>();
            else
                metadata.Kerning = 0.f;

            if (auto alignment = attributes.find("alignment"); alignment != attributes.end())
            {
                if (auto parsed = magic_enum::enum_cast<Gx::BitmapNumber::Alignment>(alignment->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                    metadata.Alignment = parsed.value();
                else
                    metadata.Alignment = Gx::BitmapNumber::Alignment::None;
            }
            else
                metadata.Alignment = Gx::BitmapNumber::Alignment::None;

            if (auto value = attributes.find("value"); value != attributes.end())
                metadata.Value = value->get<unsigned int>();
            else
                metadata.Value = 0;

            metadata.BlendMode = Gx::BlendMode::Auto;
            if (const auto mode = attributes.find("blend"); mode != attributes.end())
            {
                if (const auto parsed = magic_enum::enum_cast<Gx::BlendMode>(mode->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
                    metadata.BlendMode = parsed.value();
            }
        }

        return LoadFromMetadata(metadata, context);
    }

    Gx::ResourcePtr<Gx::BitmapNumber> BitmapNumberLoader::LoadFromMetadata(const ResourceMetadata& meta, const Gx::ResourceContext& context) const
    {
        const auto metadata = dynamic_cast<const BitmapNumberMetadata*>(&meta);
        if (!metadata)
            return Instantiate(context);
    
        auto number = Instantiate(context);
        const auto ctx = ResourceContextDecorator::Decorate(context);
        if (const auto texture = ctx.Require<sf::Texture>(*metadata); texture)
        {
            number->SetTexture(*texture);
            number->SetPosition(metadata->Position.value_or(sf::Vector2f()));

            number->SetDigitsSize(metadata->DigitSize);
            for (auto [digit, frames] : metadata->DigitFrames)
            {
                number->SetDigitFrames(digit, frames);
                if (metadata->Duration != sf::Time::Zero)
                    number->SetAnimationDuration(digit, metadata->Duration);
            }
        }
        else
        {
            if (const auto sheet = ctx.Require<SpriteSheet>(*metadata))
            {
                number->SetTexture(sheet->GetTexture());
                if (sheet->Frames.size() > 1)
                {
                    number->SetPosition(sf::Vector2f{
                         static_cast<float>(sheet->Frames[0].position.x),
                         static_cast<float>(sheet->Frames[0].position.y)
                    });
                }

                if (sheet->TexCoords.size() == 10)
                {
                    for (std::size_t d = 0; d < sheet->TexCoords.size(); d++)
                        number->SetDigitFrame(d, sheet->TexCoords[d]);

                }
                else if (sheet->TexCoords.size() > 10)
                {
                    const std::size_t digitFrameCount = sheet->TexCoords.size() / 10;
                    for (std::size_t d = 0; d < 10; d++)
                    {
                        auto frames = std::vector<sf::IntRect>();
                        for (std::size_t f = 0; f < digitFrameCount; f++)
                        {
                            const std::size_t index = f + (d * digitFrameCount);
                            if (index >= sheet->TexCoords.size())
                                break;

                            frames.push_back(sheet->TexCoords[index]);
                        }

                        number->SetDigitFrames(d, frames);
                        if (metadata->Duration != sf::Time::Zero)
                            number->SetAnimationDuration(d, metadata->Duration);
                    }
                }
            }

            if (metadata->Position.has_value())
            {
                number->SetPosition(*metadata->Position);
            }
            else if (const auto bound = ctx.Require<sf::IntRect>(*metadata))
            {
                number->SetPosition(number->GetPosition() + sf::Vector2f{
                    static_cast<float>(bound->position.x),
                    static_cast<float>(bound->position.y),
                });

                if (metadata->Alignment == Gx::BitmapNumber::Alignment::Right)
                {
                    number->SetPosition(number->GetPosition() + sf::Vector2f{
                        static_cast<float>(bound->size.x),
                        0
                    });
                }
            }
        }

        number->SetName(metadata->Name);
        number->SetOrigin(metadata->Origin);
        number->SetScale(metadata->Scale);
        number->SetRotation(metadata->Rotation);

        number->SetDigitCount(metadata->DigitCount);
        number->SetColor(metadata->Color);
        number->SetKerning(metadata->Kerning);
        number->SetValue(metadata->Value);
        number->SetAlignment(metadata->Alignment);
        number->SetBlendMode(metadata->BlendMode);

        auto container = SceneComposer::Compose(*number);
        LoadChildren(container, meta, context);

        return number;
    }
}
