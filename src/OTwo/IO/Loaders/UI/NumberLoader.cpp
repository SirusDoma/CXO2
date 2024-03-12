#include <OTwo/IO/Loaders/UI/NumberLoader.hpp>
#include <OTwo/IO/Loaders/Graphics/TransformLoader.hpp>
#include <OTwo/IO/Loaders/MetadataLoader.hpp>
#include <OTwo/Metadata/UI/NumberMetadata.hpp>
#include <OTwo/IO/ResourceContextDecorator.hpp>
#include <OTwo/IO/Loaders/SceneGraph/ObjectLoader.hpp>

Gx::ResourcePtr<Gx::Number> NumberLoader::LoadFromJson(const Gx::Json &json, const Gx::ResourceContext &context) const
{
    auto metadata = NumberMetadata();
    if (!MetadataLoader::Parse(json, metadata, context))
        return nullptr;

    auto attributes = json.at("attributes");
    if (!TransformLoader::ParseMetadata(attributes["transform"], metadata, context))
        return nullptr;

    
    auto digitSize = attributes.find("digitSize");
    if (digitSize != attributes.end())
    {
        unsigned int w, h;
        digitSize->at("width").get_to(w);
        digitSize->at("height").get_to(h);
        metadata.DigitSize = sf::Vector2u(w, h);
    }

    auto digitCount = attributes.find("digitCount");
    if (digitCount != attributes.end())
        metadata.DigitCount = digitCount->get<unsigned int>();
    else
        metadata.DigitCount = 1;

    auto frames = attributes.find("characters");
    if (frames != attributes.end())
    {
        for (auto [digit, frame] : frames->items())
        {
            unsigned int x, y, w, h;
            frame.at("x").get_to(x);
            frame.at("y").get_to(y);
            frame.at("width").get_to(w);
            frame.at("height").get_to(h);
            metadata.DigitFrames[std::stoi(digit)] = sf::IntRect(x, y, w, h);
        }
    }

    auto color = attributes.find("color");
    if (color != attributes.end())
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

    auto spacing = attributes.find("letterSpacing");
    if (spacing != attributes.end())
        metadata.LetterSpacing = spacing->get<float>();
    else
        metadata.LetterSpacing = 0.f;

    auto value = attributes.find("value");
    if (value != attributes.end())
        metadata.Value = value->get<unsigned int>();
    else
        metadata.Value = 0;

    return LoadFromMetadata(metadata, context);
}

Gx::ResourcePtr<Gx::Number> NumberLoader::LoadFromMetadata(const ResourceMetadata &meta, const Gx::ResourceContext &context) const
{
    auto metadata = dynamic_cast<const NumberMetadata*>(&meta);
    if (!metadata)
        throw Gx::ResourceLoadException("The specified metadata is incompatible.");
    
    auto number = std::make_unique<Gx::Number>();
    auto ctx = ResourceContextDecorator::Decorate(context);
    if (auto texture = ctx.Find<sf::Texture>(*metadata); texture)
        number->SetTexture(*texture);

    number->SetDigitsSize(metadata->DigitSize);
    number->SetDigitCount(metadata->DigitCount);
    for (auto [digit, frame] : metadata->DigitFrames)
        number->SetDigitFrame(digit, frame);

    number->SetColor(metadata->Color);
    number->SetLetterSpacing(metadata->LetterSpacing);
    number->SetValue(metadata->Value);

    number->SetName(metadata->Name);
    number->SetOrigin(metadata->Origin);
    number->SetPosition(metadata->Position);
    number->SetScale(metadata->Scale);
    number->SetRotation(metadata->Rotation);

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
