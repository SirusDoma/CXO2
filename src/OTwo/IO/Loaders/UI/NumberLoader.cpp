#include <OTwo/IO/Loaders/UI/NumberLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Metadata/UI/NumberMetadata.hpp>
#include <OTwo/Decorators/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

#include <magic_enum.hpp>

Gx::ResourcePtr<Gx::Number> NumberLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    auto metadata = NumberMetadata();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!TransformLoader::ParseMetadata(attributes["transform"], metadata, context))
        return nullptr;

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
        metadata.Duration = sf::milliseconds(duration->get<float>());

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
        if (auto parsed = magic_enum::enum_cast<Gx::Number::Alignment>(alignment->get<std::string>(), magic_enum::case_insensitive); parsed.has_value())
            metadata.Alignment = parsed.value();
        else
            metadata.Alignment = Gx::Number::Alignment::None;
    }
    else
        metadata.Alignment = Gx::Number::Alignment::None;

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

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Number> NumberLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    const auto metadata = dynamic_cast<const NumberMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    auto number = std::make_unique<Gx::Number>();
    const auto ctx = ResourceContextDecorator::Decorate(context);
    if (const auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        number->SetTexture(*texture);

    number->SetDigitsSize(metadata->DigitSize);
    number->SetDigitCount(metadata->DigitCount);
    for (auto [digit, frames] : metadata->DigitFrames)
    {
        number->SetDigitFrames(digit, frames);
        if (metadata->Duration != sf::Time::Zero)
            number->SetAnimationDuration(digit, metadata->Duration);
    }

    number->SetName(metadata->Name);
    number->SetOrigin(metadata->Origin);
    number->SetPosition(metadata->Position);
    number->SetScale(metadata->Scale);
    number->SetRotation(metadata->Rotation);

    number->SetColor(metadata->Color);
    number->SetKerning(metadata->Kerning);
    number->SetValue(metadata->Value);
    number->SetAlignment(metadata->Alignment);
    number->SetBlendMode(metadata->BlendMode);

    auto populator = ObjectPopulator::Decorate(number.get());
    if (!metadata->Objects.empty())
    {
        for (auto [key, object] : metadata->Objects)
        {
            auto name = meta.Name + "/" + key;
            auto objectCtx = Gx::ResourceContext::Rebind(name, context);

            ObjectLoader::Load(name, object, populator, objectCtx);
        }
    }

    return number;
}
